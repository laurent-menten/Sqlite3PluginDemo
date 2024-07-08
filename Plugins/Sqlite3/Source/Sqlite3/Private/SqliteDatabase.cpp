// (c)2024+ Laurent Menten

#include "SqliteDatabase.h"
#include "SqliteStatics.h"
#include "Sqlite3Log.h"
#include "Sqlite3Subsystem.h"

#include <shlobj.h>

// NOT IMPLEMENTED:
//	- ResultSet

// ============================================================================
// === 
// ============================================================================

void USqliteDatabase::GetSqliteDatabaseObjectFromAsset( USqliteDatabaseInfo* DatabaseInfo,
	ESqliteDatabaseSimpleExecutionPins& Branch, ESqliteErrorCode& ReturnCode, USqliteDatabase*& DatabaseHandle )
{
	// ---------------------------------------------------------------------------
	// - Shortcut if database object already exists ------------------------------
	// ---------------------------------------------------------------------------

	if( DatabaseInfo->DatabaseInstance )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
		ReturnCode = ESqliteErrorCode::Ok;
		DatabaseHandle = DatabaseInfo->DatabaseInstance;

		return;
	}

	// ---------------------------------------------------------------------------
	// - Get subsystem -----------------------------------------------------------
	// ---------------------------------------------------------------------------

	USqlite3Subsystem* Sqlite3Subsystem = USqlite3Subsystem::GetInstance();
	if( Sqlite3Subsystem == nullptr )
	{
		UE_LOG( LogSqlite, Error, TEXT( "Sqlite3 Subsystem not found !" ) );

		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		ReturnCode = ESqliteErrorCode::Misuse;
		DatabaseHandle = nullptr;
		return;
	}

	// ---------------------------------------------------------------------------
	// - Sanity checks -----------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( !Sqlite3Subsystem->IsSqliteInitialized() )
	{
		UE_LOG( LogSqlite, Error, TEXT( "Sqlite library not initialized!" ) );

		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		ReturnCode = ESqliteErrorCode::Misuse;
		DatabaseHandle = nullptr;
		return;
	}

	// ---------------------------------------------------------------------------
	// - Create USqliteDatabase object -------------------------------------------
	// ---------------------------------------------------------------------------

	USqliteDatabase* db = NewObject<USqliteDatabase>( GetTransientPackage(), DatabaseInfo->DatabaseClass );
	db->Initialize( DatabaseInfo );

	DatabaseInfo->DatabaseInstance = db;
	DatabaseInfo->DatabaseOpenCount = 1;

	Sqlite3Subsystem->RegisterDatabase( db );

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	ReturnCode = ESqliteErrorCode::Ok;
	DatabaseHandle = db;
}

// ============================================================================
// === 
// ============================================================================

// Note: DatabaseInfo asset has been validated in editor.

void USqliteDatabase::Initialize( const USqliteDatabaseInfo* DatabaseInfo )
{
	UE_LOG( LogSqlite, Log, TEXT("%08.8p USqliteDatabase::%s( %08.8p [%s] )"),
		this,
		*FString( __func__ ),
		DatabaseInfo,
		*DatabaseInfo->DatabaseFileName );

	DatabaseInfoAsset = const_cast<USqliteDatabaseInfo*>(DatabaseInfo);

	// ---------------------------------------------------------------------------
	// - Compute open flags ------------------------------------------------------
	// ---------------------------------------------------------------------------

	OpenFlags = 0;

	switch( DatabaseInfoAsset->OpenMode )
	{
		case ESqliteDatabaseOpenMode::READ_ONLY:
			OpenFlags = SQLITE_OPEN_READONLY;
			break;

		case ESqliteDatabaseOpenMode::READ_WRITE:
			OpenFlags = SQLITE_OPEN_READWRITE;
			break;

		case ESqliteDatabaseOpenMode::READ_WRITE_CREATE:
			OpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
			break;
	}

	if( DatabaseInfoAsset->bOpenAsURI )
	{
		OpenFlags |= SQLITE_OPEN_URI;
	}

	if( DatabaseInfoAsset->bInMemory )
	{
		OpenFlags |= SQLITE_OPEN_MEMORY;
	}

	if( DatabaseInfoAsset->ThreadingMode != ESqliteDatabaseThreadingMode::UNSET )
	{
		switch( DatabaseInfoAsset->ThreadingMode )
		{
			case ESqliteDatabaseThreadingMode::NO_MUTEX:
				OpenFlags |= SQLITE_OPEN_NOMUTEX;
				break;

			case ESqliteDatabaseThreadingMode::FULL_MUTEX:
				OpenFlags |= SQLITE_OPEN_FULLMUTEX;
				break;

			case ESqliteDatabaseThreadingMode::UNSET:
				break;
		}
	}

	if( DatabaseInfoAsset->CacheMode != ESqliteDatabaseCacheMode::UNSET )
	{
		switch( DatabaseInfoAsset->CacheMode )
		{
			case ESqliteDatabaseCacheMode::SHARED_CACHE:
				OpenFlags |= SQLITE_OPEN_SHAREDCACHE;
				break;

			case ESqliteDatabaseCacheMode::PRIVATE_CACHE:
				OpenFlags |= SQLITE_OPEN_PRIVATECACHE;
				break;

			case ESqliteDatabaseCacheMode::UNSET:
				break;
		}
	}

	if( DatabaseInfoAsset->bUseExtendedResultCode )
	{
		OpenFlags |= SQLITE_OPEN_EXRESCODE;
	}

	if( DatabaseInfoAsset->bNoFollow )
	{
		OpenFlags |= SQLITE_OPEN_NOFOLLOW;
	}

	// ---------------------------------------------------------------------------
	// - Compute database file path ----------------------------------------------
	// ---------------------------------------------------------------------------

	FString DatabaseDirectory;

	if( !DatabaseInfoAsset->DatabaseFileName.Compare( ":memory:", ESearchCase::IgnoreCase )
		|| (!DatabaseInfoAsset->bInMemory && !DatabaseInfoAsset->bOpenAsURI) )
	{
		if( !DatabaseInfoAsset->DatabaseDirectoryOverride.Path.IsEmpty() )
		{
			if( !DatabaseInfoAsset->DatabaseDirectoryOverride.Path.EndsWith( "/" ) )
			{
				DatabaseDirectory = DatabaseInfoAsset->DatabaseDirectoryOverride.Path.Append( "/" );
			}
			else
			{
				DatabaseDirectory = DatabaseInfoAsset->DatabaseDirectoryOverride.Path;
			}
		}
		else
		{
			DatabaseDirectory = USqlite3Subsystem::GetInstance()->GetDefaultDatabaseDirectory();
		}

		DatabaseFilePath = DatabaseDirectory + DatabaseInfoAsset->DatabaseFileName;

		if( DatabaseInfoAsset->bDeleteFileBeforeOpen )
		{
			UE_LOG( LogSqlite, Log, TEXT("Attempting to delete file '%s'"), *DatabaseFilePath );

			if( FPaths::ValidatePath( DatabaseFilePath ) && FPaths::FileExists( DatabaseFilePath ) )
			{
				IFileManager& FileManager = IFileManager::Get();
				FileManager.Delete( *DatabaseFilePath );
			}
		}
	}
	else
	{
		DatabaseFilePath = DatabaseInfoAsset->DatabaseFileName;
	}

	// ---------------------------------------------------------------------------
	// - Compute attachments -----------------------------------------------------
	// ---------------------------------------------------------------------------

	for( const auto& Attachment : DatabaseInfoAsset->Attachments )
	{
		FString AttachmentFilePath;

		if( Attachment.FileName.Compare( ":memory:", ESearchCase::IgnoreCase ) != 0 )
		{
			AttachmentFilePath = DatabaseDirectory + Attachment.FileName;
		}
		else
		{
			AttachmentFilePath = Attachment.FileName;
		}

		Attachments.Add( Attachment.SchemaName, AttachmentFilePath );
	}

	// ---------------------------------------------------------------------------

	bIsInitialized = true;
}

ESqliteDatabaseOpenExecutionPins USqliteDatabase::DoOpenSqliteDatabase()
{
	// ---------------------------------------------------------------------------
	// - Silently ignore re-opening ----------------------------------------------
	// ---------------------------------------------------------------------------

	if( bIsOpen )
	{
		return ESqliteDatabaseOpenExecutionPins::OnSuccess;
	}
	
	// ---------------------------------------------------------------------------
	// - Paranoid setting --------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( !IsInitialized() )
	{
		LOG_SQLITE_ERROR( __func__, -1, "Database not initialized." );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// ---------------------------------------------------------------------------
	// - Open database -----------------------------------------------------------
	// ---------------------------------------------------------------------------

	LastSqliteReturnCode = sqlite3_open_v2( TCHAR_TO_ANSI(*DatabaseFilePath), &DatabaseConnectionHandler, OpenFlags, "unreal-fs" );
	if( LastSqliteReturnCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, GetErrorCode(), "Open failed." );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// ---------------------------------------------------------------------------
	// - Attach extra databases
	// ---------------------------------------------------------------------------

	sqlite3_stmt* stmt;

	LastSqliteReturnCode = sqlite3_prepare_v2( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *Sql_AttachDatabase ), -1, &stmt, 0 );
	if( LastSqliteReturnCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, GetErrorCode(), "Failed to prepare 'AttachDatabase' statement." );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	DatabaseInfoAsset->DatabaseOpenCount++;
	
	for( const auto& Attachment : Attachments )
	{
		UE_LOG( LogSqlite, Log, TEXT( "Attaching '%s' as '%s'" ),
			*Attachment.Value,
			*Attachment.Key.ToString() );

		LastSqliteReturnCode = sqlite3_bind_text( stmt, 1, TCHAR_TO_ANSI( *Attachment.Value ), -1, nullptr );
		if( LastSqliteReturnCode != SQLITE_OK )
		{
			break;
		}

		LastSqliteReturnCode = sqlite3_bind_text( stmt, 2, TCHAR_TO_ANSI( *Attachment.Key.ToString() ), -1, nullptr );
		if( LastSqliteReturnCode != SQLITE_OK )
		{
			break;
		}

		LastSqliteReturnCode = sqlite3_step( stmt );
		if( LastSqliteReturnCode != SQLITE_DONE )
		{
			break;
		}

		sqlite3_reset( stmt );
		sqlite3_clear_bindings( stmt );
	}

	if( (LastSqliteReturnCode != SQLITE_OK) && (LastSqliteReturnCode != SQLITE_DONE) )
	{
		LOG_SQLITE_ERROR( __func__, GetErrorCode(), "Failed to bind or execute 'attach' statement." );
	}

	sqlite3_finalize( stmt );
	stmt = nullptr;

	if( (LastSqliteReturnCode != SQLITE_OK) && (LastSqliteReturnCode != SQLITE_DONE) )
	{
		Close();
		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// ---------------------------------------------------------------------------
	// - Check database for create/update ----------------------------------------
	// ---------------------------------------------------------------------------

	GetApplicationId( StoredApplicationId );
	GetUserVersion( StoredUserVersion );

	UE_LOG( LogSqlite, Log, TEXT("Database opened, current version=[%d,%d] stored version=[%d,%d]"),
		DatabaseInfoAsset->ApplicationId,
		DatabaseInfoAsset->UserVersion,
		StoredApplicationId,
		StoredUserVersion );

	// If application_id is zero (default value), database is considered to be
	// newly created. SqliteDatabaseInfos validation forbid zero.

	if( StoredApplicationId == 0 )
	{
		BeginTransaction( "create" );

		if( !OnCreatePrivate() )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnCreatePrivate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "create - private" );

			Close();
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		bParentOnCreateCalled = false;
		if( !OnCreate() )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnCreate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "create - public" );

			Close();
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		if( !bParentOnCreateCalled )
		{
			UE_LOG( LogSqlite, Warning, TEXT("Super::OnCreate not called") );
		}

		bParentOnCreateCalled = false;
		ESqliteReturnCode ResultCode = ESqliteReturnCode::Unset;
		OnCreateEvent( ResultCode );

		if( !bParentOnCreateCalled )
		{
			UE_LOG( LogSqlite, Warning, TEXT("OnCreateEvent did not call parent implementation.") );
		}

		if( ResultCode == ESqliteReturnCode::Unset )
		{
			UE_LOG( LogSqlite, Warning, TEXT("OnCreateEvent did no set its return value") );
		}
		else if( ResultCode == ESqliteReturnCode::Failure )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnCreateEvent failed") );

			Rollback( "create - blueprint" );

			Close();
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		UpdateApplicationId( DatabaseInfoAsset->SchemaName );
		UpdateUserVersion( DatabaseInfoAsset->SchemaName );

		for( const auto& Attachment : Attachments )
		{
			UpdateApplicationId( Attachment.Key );
			UpdateUserVersion( Attachment.Key );
		}

		Commit( "create" );

		bIsOpen = true;

		return ESqliteDatabaseOpenExecutionPins::OnCreate;
	}

	// If file application_id does not match code application_id, it is not
	// our file

	else if( DatabaseInfoAsset->ApplicationId != StoredApplicationId )
	{
		UE_LOG( LogSqlite, Fatal, TEXT("Unexpected ApplicationId value %d"),
			StoredApplicationId );

		Close();
		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// If file user_version does not match code user_version, we need to
	// update the database

	else if( DatabaseInfoAsset->UserVersion != StoredUserVersion )
	{
		BeginTransaction( "update" );

		if( !OnUpdatePrivate( DatabaseInfoAsset->UserVersion, StoredUserVersion ) )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnUpdatePrivate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "update - private" );

			Close();
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		bParentOnUpdateCalled = false;
		if( !OnUpdate( DatabaseInfoAsset->UserVersion, StoredUserVersion ) )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnUpdate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "update - public" );

			Close();
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		if( !bParentOnUpdateCalled )
		{
			UE_LOG( LogSqlite, Warning, TEXT("Super::OnUpdate not called") );
		}

		bParentOnUpdateCalled = false;
		ESqliteReturnCode ResultCode = ESqliteReturnCode::Unset;
		OnUpdateEvent( DatabaseInfoAsset->UserVersion, StoredUserVersion, ResultCode );

		if( !bParentOnUpdateCalled )
		{
			UE_LOG( LogSqlite, Warning, TEXT("OnUpdateEvent did not call parent implementation.") );
		}

		if( ResultCode == ESqliteReturnCode::Unset )
		{
			UE_LOG( LogSqlite, Warning, TEXT("OnUpdateEvent did no set its return value") );
		}
		else if( ResultCode == ESqliteReturnCode::Failure )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnUpdateEvent failed") );

			Rollback( "update - blueprint" );

			Close();
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}
	
		UpdateUserVersion( DatabaseInfoAsset->SchemaName );

		for( const auto& Attachment : Attachments )
		{
			UpdateUserVersion( Attachment.Key );
		}

		Commit( "update" );

		bIsOpen = true;

		return ESqliteDatabaseOpenExecutionPins::OnUpdate;
	}

	bIsOpen = true;

	return ESqliteDatabaseOpenExecutionPins::OnSuccess;
}

// ============================================================================
// === 
// ============================================================================

bool USqliteDatabase::IsInitialized() const
{
	return bIsInitialized;
}

bool USqliteDatabase::IsOpen() const
{
	return bIsOpen;
}

// ============================================================================
// === 
// ============================================================================

void USqliteDatabase::EnableAutovacuumCallback( bool enabled )
{
	if( enabled )
	{
		sqlite3_autovacuum_pages( DatabaseConnectionHandler, USqliteDatabase::AutovacuumCallbackGlue, this, nullptr );
	}
	else
	{
		sqlite3_autovacuum_pages( DatabaseConnectionHandler, nullptr, this, nullptr );
	}
}

unsigned int USqliteDatabase::AutovacuumCallbackGlue( void* DatabaseRawPtr, const char* Schema, unsigned int DbPage, unsigned int FreePage, unsigned int BytePerPage )
{
	USqliteDatabase* Database = (USqliteDatabase*)DatabaseRawPtr;

	return Database->AutovacuumCallback( Schema, DbPage, FreePage, BytePerPage );
}

unsigned int USqliteDatabase::AutovacuumCallback( const FName& Schema, unsigned int DbPage, unsigned int FreePage, unsigned int BytePerPage )
{
	// TODO: create delegate
	return FreePage;
}

// ----------------------------------------------------------------------------

void USqliteDatabase::EnablePreupdateHook( bool enabled )
{
	if( enabled )
	{
		sqlite3_preupdate_hook( DatabaseConnectionHandler, USqliteDatabase::PreupdateHookGlue, this );
	}
	else
	{
		sqlite3_preupdate_hook( DatabaseConnectionHandler, nullptr, this );
	}
}

void USqliteDatabase::PreupdateHookGlue( void* DatabaseRawPtr, sqlite3* db, int Operation, const char* DatabaseName, const char* TableName, int64 RowId1, int64 RowId2 )
{
	USqliteDatabase* Database = (USqliteDatabase*)DatabaseRawPtr;

	Database->PreupdateHook( Operation, FString( DatabaseName ), FName( TableName ), RowId1, RowId2 );
}

void USqliteDatabase::PreupdateHook( int Operation, const FString& DatabaseName, const FName& TableName, int64 RowId1, int64 RowId2 )
{
	// TODO: create delegate
}

// ----------------------------------------------------------------------------

void USqliteDatabase::EnableUpdateHook( bool enabled )
{
	if( enabled )
	{
		sqlite3_update_hook( DatabaseConnectionHandler, USqliteDatabase::UpdateHookGlue, this );
	}
	else
	{
		sqlite3_update_hook( DatabaseConnectionHandler, nullptr, this );
	}
}

void USqliteDatabase::UpdateHookGlue( void* DatabaseRawPtr, int Operation, char const* DatabaseName, char const* TableName, int64 RowId )
{
	USqliteDatabase* Database = (USqliteDatabase*)DatabaseRawPtr;

	Database->UpdateHook( Operation, FString(DatabaseName), FName(TableName), RowId );
}

void USqliteDatabase::UpdateHook( int Operation, const FString& DatabaseName, const FName& TableName, int64 RowId )
{
	// TODO: create delegate
}

// ----------------------------------------------------------------------------

void USqliteDatabase::EnableCommitHook( bool enabled )
{
	if( enabled )
	{
		sqlite3_commit_hook( DatabaseConnectionHandler, USqliteDatabase::CommitHookGlue, this );
	}
	else
	{
		sqlite3_commit_hook( DatabaseConnectionHandler, nullptr, this );
	}
}

int USqliteDatabase::CommitHookGlue( void* DatabaseRawPtr  )
{
	USqliteDatabase* Database = (USqliteDatabase*)DatabaseRawPtr;

	return Database->CommitHook();
}

int USqliteDatabase::CommitHook()
{
	// TODO: create delegate
	return 0;
}

// ----------------------------------------------------------------------------

void USqliteDatabase::EnableRollbackHook( bool enabled )
{
	if( enabled )
	{
		sqlite3_rollback_hook( DatabaseConnectionHandler, USqliteDatabase::RollbackHookGlue, this );
	}
	else
	{
		sqlite3_rollback_hook( DatabaseConnectionHandler, nullptr, this );
	}
}

void USqliteDatabase::RollbackHookGlue( void* DatabaseRawPtr )
{
	USqliteDatabase* Database = (USqliteDatabase*)DatabaseRawPtr;

	Database->RollbackHook();
}

void USqliteDatabase::RollbackHook()
{
	// TODO: create delegate
}

// ============================================================================
// === 
// ============================================================================

void USqliteDatabase::Open( ESqliteDatabaseOpenExecutionPins& Branch, ESqliteErrorCode& ErrorCode )
{
	UE_LOG( LogSqlite, Log, TEXT("Opening database '%s' with class '%s'"),
		*DatabaseFilePath,
		*this->GetClass()->GetFName().ToString() );

	Branch = DoOpenSqliteDatabase();
	ErrorCode = GetLastErrorCode();

	return;
}

// ----------------------------------------------------------------------------

bool USqliteDatabase::OnCreatePrivate()
{
	UE_LOG( LogSqlite, Log, TEXT("%s()"), *FString( __func__ ) );

	bool error = false;

	if( DatabaseInfoAsset->bCreateStoredStatementsTable && !CreateTable( TN_StoredStatements, Sql_CreateStoredStatements ) )
	{
		error |= true;
	}

	if( DatabaseInfoAsset->bCreatePropertiesTable && !CreateTable( TN_Properties, Sql_CreateProperties ) )
	{
		error |= true;
	}

	if( DatabaseInfoAsset->bCreateActorsStoreTable && !CreateTable( TN_ActorsStore, Sql_CreateActorsStore ) )
	{
		error |= true;
	}

	if( DatabaseInfoAsset->bCreateLogTable && !CreateTable( TN_Log, Sql_CreateLog ) )
	{
		error |= true;
	}

	if( error )
	{
		return false;
	}

	return true;
}

bool USqliteDatabase::OnCreate()
{
	UE_LOG( LogSqlite, Log, TEXT("%s() [USqliteDatabase]"), *FString( __func__ ) );

	bParentOnCreateCalled = true;
	return true;
}

void USqliteDatabase::OnCreateEvent_Implementation( ESqliteReturnCode& Result )
{
	UE_LOG( LogSqlite, Log, TEXT("%s() [USqliteDatabase]"),*FString( __func__ ) );

	bParentOnCreateCalled = true;
	Result = ESqliteReturnCode::Success;
}

// ----------------------------------------------------------------------------

bool USqliteDatabase::OnUpdatePrivate( int NewDatabaseVersion, int OldDatabaseVersion )
{
	UE_LOG( LogSqlite, Log, TEXT("%s()"), *FString( __func__ ) );

	return true;
}

bool USqliteDatabase::OnUpdate( int NewDatabaseVersion, int OldDatabaseVersion )
{
	UE_LOG( LogSqlite, Log, TEXT("%s( %d, %d ) [USqliteDatabase]"), *FString( __func__ ),
		NewDatabaseVersion, OldDatabaseVersion );

	bParentOnUpdateCalled = true;
	return true;
}

void USqliteDatabase::OnUpdateEvent_Implementation( int NewDatabaseVersion, int OldDatabaseVersion, ESqliteReturnCode& Result )
{
	UE_LOG( LogSqlite, Log, TEXT("%s() [USqliteDatabase]"), *FString( __func__ ) );

	bParentOnUpdateCalled = true;
	Result = ESqliteReturnCode::Success;
}

// ============================================================================
// === Close ==================================================================
// ============================================================================

void USqliteDatabase::Finalize()
{
	if( IsOpen() )
	{
		Close( true );
	}
	else
	{
		UE_LOG( LogSqlite, Log, TEXT( "Database '%s' already closed." ), *DatabaseFilePath );
	}
}

void USqliteDatabase::Close( const bool bForceClose )
{
	UE_LOG( LogSqlite, Log, TEXT("Closing database '%s'"), *DatabaseFilePath );

	if( !bForceClose && DatabaseInfoAsset->DatabaseOpenCount > 1 )
	{
		DatabaseInfoAsset->DatabaseOpenCount--;

		return;
	}

	if( ! ActiveStatements.IsEmpty() )
	{
		UE_LOG( LogSqlite, Warning, TEXT("Finalizing %d leftover statement(s) before closing."), ActiveStatements.Num() );

		TArray<USqliteStatement*> ActiveStatementsCachedList( ActiveStatements );
		for( const auto& Statement : ActiveStatementsCachedList )
		{
			Statement->Finalize();
		}

		if( ! ActiveStatements.IsEmpty() )
		{
			UE_LOG( LogSqlite, Error, TEXT("Still %d leftover statement(s) before closing, database will no be properly closed."), ActiveStatements.Num() );
		}
	}

	// TODO: close BLOB handlers and finish backup objects
		
	if( sqlite3_close_v2( DatabaseConnectionHandler ) != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Close() failed: (%d) %s"),
			GetErrorCode(),
			*GetErrorMessage() );
	}

	bIsOpen = false;
	DatabaseConnectionHandler = nullptr;

	DatabaseInfoAsset->DatabaseOpenCount = 0;
}

// ============================================================================
// === application_id & user_version ==========================================
// ============================================================================

void USqliteDatabase::GetApplicationId( ESqliteDatabaseSimpleExecutionPins& Branch, int& OutApplicationId ) const
{
	Branch = GetApplicationId( OutApplicationId )
		? ESqliteDatabaseSimpleExecutionPins::OnSuccess
		: ESqliteDatabaseSimpleExecutionPins::OnFail;
}

bool USqliteDatabase::GetApplicationId( int& OutApplicationId ) const
{
	const FString SqlRequest( "PRAGMA application_id");
	bool bReturnValue;

	sqlite3_stmt* Stmt;
	
	int ErrorCode = sqlite3_prepare_v2( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *SqlRequest ), -1, &Stmt, nullptr );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, ErrorCode, TCHAR_TO_ANSI( *SqlRequest ) );
		bReturnValue = false;
	}
	else
	{
		ErrorCode = sqlite3_step( Stmt );
		if( ErrorCode != SQLITE_ROW )
		{
			bReturnValue = false;
		}
		else
		{
			OutApplicationId = sqlite3_column_int( Stmt, 0 );
			bReturnValue = true;
		}

		sqlite3_finalize( Stmt );
	}

	return bReturnValue;
}

bool USqliteDatabase::UpdateApplicationId( const FName Schema )
{
	const FString SqlRequest = FString::Format( TEXT( "PRAGMA \"{0}\".application_id = {1}" ), { Schema.ToString(), DatabaseInfoAsset->ApplicationId } );
	char* ErrorMessage;

	int ErrorCode = sqlite3_exec( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *SqlRequest ), nullptr, nullptr, &ErrorMessage );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, ErrorCode, ErrorMessage );
	}
	else
	{
		StoredApplicationId = DatabaseInfoAsset->ApplicationId;
	}

	if( ErrorMessage != nullptr )
	{
		sqlite3_free( ErrorMessage );
	}

	return ErrorCode == SQLITE_OK;
}

// ----------------------------------------------------------------------------

void USqliteDatabase::GetUserVersion( ESqliteDatabaseSimpleExecutionPins& Branch, int& OutUserVersion ) const
{
	Branch = GetUserVersion( OutUserVersion )
		? ESqliteDatabaseSimpleExecutionPins::OnSuccess
		: ESqliteDatabaseSimpleExecutionPins::OnFail;
}

bool USqliteDatabase::GetUserVersion( int& OutUserVersion ) const
{
	const FString SqlRequest( "PRAGMA user_version");
	bool bReturnValue;

	sqlite3_stmt* Stmt;

	int ErrorCode = sqlite3_prepare_v2( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *SqlRequest ), -1, &Stmt, nullptr );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, ErrorCode, TCHAR_TO_ANSI( *SqlRequest ) );
		bReturnValue = false;
	}
	else
	{
		ErrorCode = sqlite3_step( Stmt );
		if( ErrorCode != SQLITE_ROW )
		{
			bReturnValue = false;
		}
		else
		{
			OutUserVersion = sqlite3_column_int( Stmt, 0 );
			bReturnValue = true;
		}

		sqlite3_finalize( Stmt );
	}

	return bReturnValue;
}

bool USqliteDatabase::UpdateUserVersion( const FName Schema )
{
	const FString SqlRequest = FString::Format( TEXT( "PRAGMA \"{0}\".user_version = {1}" ), { Schema.ToString(), DatabaseInfoAsset->UserVersion } );
	char* ErrorMessage;

	int ErrorCode = sqlite3_exec( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *SqlRequest ), nullptr, nullptr, &ErrorMessage );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, ErrorCode, ErrorMessage );
	}
	else
	{
		StoredUserVersion = DatabaseInfoAsset->UserVersion;
	}

	if( ErrorMessage != nullptr )
	{
		sqlite3_free( ErrorMessage );
	}

	return ErrorCode == SQLITE_OK;
}

// ============================================================================
// === Transactions ===========================================================
// ============================================================================

int USqliteDatabase::BeginTransaction( const FString& Hint )
{
	char* ErrorMessage = nullptr;

	int ErrorCode = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *Sql_BeginTransaction ), nullptr, nullptr, &ErrorMessage );
	if( ErrorCode != SQLITE_OK )
	{
		if( !Hint.IsEmpty() )
		{
			LOG_SQLITE_ERROR( __func__, ErrorCode, TCHAR_TO_ANSI(*Hint), ErrorMessage );
		}
		else
		{
			LOG_SQLITE_ERROR( __func__, ErrorCode, ErrorMessage );
		}
	}

	if( ErrorMessage != nullptr )
	{
		sqlite3_free( ErrorMessage );
	}

	return ErrorCode;
}

int USqliteDatabase::Commit( const FString& Hint )
{
	char* ErrorMessage = nullptr;

	int ErrorCode = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *Sql_Commit ), nullptr, nullptr, &ErrorMessage );
	if( ErrorCode != SQLITE_OK )
	{
		if( !Hint.IsEmpty() )
		{
			LOG_SQLITE_ERROR( __func__, ErrorCode, TCHAR_TO_ANSI(*Hint), ErrorMessage );
		}
		else
		{
			LOG_SQLITE_ERROR( __func__, ErrorCode, ErrorMessage );
		}
	}

	if( ErrorMessage != nullptr )
	{
		sqlite3_free( ErrorMessage );
	}

	return ErrorCode;
}

int USqliteDatabase::Rollback( const FString& Hint )
{
	char* ErrorMessage = nullptr;

	int ErrorCode = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *Sql_Rollback ), nullptr, nullptr, &ErrorMessage );
	if( ErrorCode != SQLITE_OK )
	{
		if( !Hint.IsEmpty() )
		{
			LOG_SQLITE_ERROR( __func__, ErrorCode, TCHAR_TO_ANSI(*Hint), ErrorMessage );
		}
		else
		{
			LOG_SQLITE_ERROR( __func__, ErrorCode, ErrorMessage );
		}
	}

	if( ErrorMessage != nullptr )
	{
		sqlite3_free( ErrorMessage );
	}

	return ErrorCode;
}

// ============================================================================
// === Errors =================================================================
// ============================================================================

int USqliteDatabase::GetErrorCode()
{
	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	int rc = sqlite3_errcode( DatabaseConnectionHandler );

	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rc;
}

int USqliteDatabase::GetExtentedErrorCode()
{
	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	int rc = sqlite3_extended_errcode( DatabaseConnectionHandler );

	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rc;
}

FString USqliteDatabase::GetErrorMessage()
{
	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	FString rs = FString( sqlite3_errmsg( DatabaseConnectionHandler ) );

	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rs;
}

int USqliteDatabase::GetErrorOffset()
{
	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	int rc = sqlite3_error_offset( DatabaseConnectionHandler );

	if( DatabaseInfoAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rc;
}

FString USqliteDatabase::GetErrorString( int ErrorCode )
{
	return FString( sqlite3_errstr( ErrorCode ) );
}

ESqliteErrorCode USqliteDatabase::GetLastErrorCode()
{
	return USqliteStatics::MapNativeErrorCode( LastSqliteReturnCode );
}

int USqliteDatabase::GetLastErrorCodeCxx() const
{
	return LastSqliteReturnCode;
}

USqliteStatement* USqliteDatabase::Prepare( FString sql )
{
	sqlite3_stmt* stmt;
	LastSqliteReturnCode = sqlite3_prepare_v2( DatabaseConnectionHandler, TCHAR_TO_ANSI( *sql ), -1, &stmt, NULL );
	if( LastSqliteReturnCode != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Prepare statement failed: (%d) %s"),
			GetErrorCode(),
			*GetErrorMessage() );

		return nullptr;
	}

	USqliteStatement* Statement = NewObject<USqliteStatement>();
	Statement->Database = this;
	Statement->StatementHandler = stmt;

	ActiveStatements.Add( Statement );
	
	return Statement;
}

void USqliteDatabase::StatementFinalized( USqliteStatement* Statement )
{
	ActiveStatements.Remove( Statement );
}

FString USqliteDatabase::GetDatabaseFileName() const
{
	return DatabaseInfoAsset->DatabaseFileName;
}

FString USqliteDatabase::GetDatabaseFilePath()
{
	return DatabaseFilePath;
}

// ============================================================================
// = UTILITIES ================================================================
// ============================================================================

bool USqliteDatabase::CreateTable( FName TableName, FString Sql ) const
{
	char* ErrorMessage = nullptr;

	const int ErrorCode = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *Sql ), nullptr, nullptr, &ErrorMessage );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITE_ERROR( __func__, ErrorCode, TCHAR_TO_ANSI(*TableName.ToString()), ErrorMessage );
	}

	if( ErrorMessage != nullptr )
	{
		sqlite3_free( ErrorMessage );
	}

	return ErrorCode == SQLITE_OK;
}

// ============================================================================
// = CONSTANTS ================================================================
// ============================================================================

const FString USqliteDatabase::Sql_BeginTransaction = TEXT( "BEGIN TRANSACTION;" );
const FString USqliteDatabase::Sql_Commit = TEXT( "COMMIT;" );
const FString USqliteDatabase::Sql_Rollback = TEXT( "ROLLBACK;" );

// ----------------------------------------------------------------------------

const FName USqliteDatabase::PN_DatabaseName( "@DatabaseName" );
const FName USqliteDatabase::PN_SchemaName( "@SchemaName" );

const FString USqliteDatabase::Sql_AttachDatabase = TEXT( "ATTACH DATABASE @DatabaseName AS @SchemaName;" );
const FString USqliteDatabase::Sql_DetachDatabase = TEXT( "DETACH DATABASE @SchemName;" );

const FString USqliteDatabase::Sql_ListSchemas = TEXT(
	"SELECT name "
	"FROM pragma_database_list "
	"WHERE name NOT IN ('main', 'temp') "
	"ORDER BY name;"
);

const FString USqliteDatabase::Sql_SchemaExists = TEXT(
	"SELECT COUNT(*)"
	"FROM pragma_database_list "
	"WHERE name = @SchemaName"
);

// ----------------------------------------------------------------------------

const FName USqliteDatabase::TN_StoredStatements( "StoredStatements" );
const FString USqliteDatabase::Sql_CreateStoredStatements = TEXT(
	"CREATE TABLE IF NOT EXISTS StoredStatements "
	"("
	"\"Key\" TEXT PRIMARY KEY, "
	"\"Group\" TEXT NOT NULL, "
	"\"Schema\" TEXT NOT NULL, "
	"\"Query\" TEXT NOT NULL "
	");"
);

const FName USqliteDatabase::TN_Properties( "Properties" );
const FString USqliteDatabase::Sql_CreateProperties = TEXT(
	"CREATE TABLE IF NOT EXISTS Properties "
	"("
	"\"Key\" TEXT PRIMARY KEY, "
	"\"Value\" ANY "
	");"

	"INSERT INTO Properties VALUES( \"software\", \"Unreal Engine Sqlite3 plugin by Laurent Menten\" );"
	"INSERT INTO Properties VALUES( \"creation.date\", DATE('now') );"
	"INSERT INTO Properties VALUES( \"creation.time\", TIME('now') );"
);


const FName USqliteDatabase::TN_ActorsStore( "ActorsStore" );
const FString USqliteDatabase::Sql_CreateActorsStore = TEXT(
	"CREATE TABLE IF NOT EXISTS ActorsStore "
	"("
	"\"Id\" INTEGER PRIMARY KEY, "
	"\"Class\" TEXT NOT NULL, "
	"\"Name\" TEXT, "
	"\"Location_X\" REAL, "
	"\"Location_Y\" REAL, "
	"\"Location_Z\" REAL, "
	"\"Rotation_Roll\" REAL, "
	"\"Rotation_Pitch\" REAL, "
	"\"Rotation_Yaw\" REAL, "
	"\"Scale_X\" REAL, "
	"\"Scale_Y\" REAL, "
	"\"Scale_Z\" REAL "
	");"
);

const FName USqliteDatabase::TN_Log( "Log" );
const FString USqliteDatabase::Sql_CreateLog = TEXT(
	""
);
