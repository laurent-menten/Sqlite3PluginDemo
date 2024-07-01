// (c)2024+ Laurent Menten

#include "SqliteDatabase.h"
#include "SqliteStatics.h"
#include "Sqlite3Log.h"
#include "Sqlite3Subsystem.h"

#include <shlobj.h>

// KNOWN BUG:
// - Failure to open database after a first run. Need to restart editor.
//		Exact reason unknown.
//		My code DO call close() successfuly on first run
//		Possible problem with unreal-fs code.

// NOT IMPLEMENTED:
//	- ResultSet

// ============================================================================
// === 
// ============================================================================

// Note: DatabaseInfos asset has already been validated in editor.

void USqliteDatabase::Initialize( const USqliteDatabaseInfo* DatabaseInfos )
{
	UE_LOG( LogSqlite, Log, TEXT("%08.8p USqliteDatabase::%s( %08.8p [%s] )"),
		this,
		*FString( __func__ ),
		DatabaseInfos,
		*DatabaseInfos->DatabaseFileName );

	DatabaseInfosAsset = (USqliteDatabaseInfo *) DatabaseInfos;

	// ---------------------------------------------------------------------------
	// - Compute open flags ------------------------------------------------------
	// ---------------------------------------------------------------------------

	OpenFlags = 0;

	switch( DatabaseInfosAsset->OpenMode )
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

	if( DatabaseInfosAsset->bOpenAsURI )
	{
		OpenFlags |= SQLITE_OPEN_URI;
	}

	if( DatabaseInfosAsset->bInMemory )
	{
		OpenFlags |= SQLITE_OPEN_MEMORY;
	}

	if( DatabaseInfosAsset->ThreadingMode != ESqliteDatabaseThreadingMode::UNSET )
	{
		switch( DatabaseInfosAsset->ThreadingMode )
		{
			case ESqliteDatabaseThreadingMode::NO_MUTEX:
				OpenFlags |= SQLITE_OPEN_NOMUTEX;
				break;

			case ESqliteDatabaseThreadingMode::FULL_MUTEX:
				OpenFlags |= SQLITE_OPEN_FULLMUTEX;
				break;
		}
	}

	if( DatabaseInfosAsset->CacheMode != ESqliteDatabaseCacheMode::UNSET )
	{
		switch( DatabaseInfosAsset->CacheMode )
		{
			case ESqliteDatabaseCacheMode::SHARED_CACHE:
				OpenFlags |= SQLITE_OPEN_SHAREDCACHE;
				break;

			case ESqliteDatabaseCacheMode::PRIVATE_CACHE:
				OpenFlags |= SQLITE_OPEN_PRIVATECACHE;
				break;
		}
	}

	if( DatabaseInfosAsset->bUseExtendedResultCode )
	{
		OpenFlags |= SQLITE_OPEN_EXRESCODE;
	}

	if( DatabaseInfosAsset->bNoFollow )
	{
		OpenFlags |= SQLITE_OPEN_NOFOLLOW;
	}

	// ---------------------------------------------------------------------------
	// - Compute database file path ----------------------------------------------
	// ---------------------------------------------------------------------------

	FString DatabaseDirectory;

	if( !DatabaseInfosAsset->DatabaseFileName.Compare( ":memory:", ESearchCase::IgnoreCase )
		|| (!DatabaseInfosAsset->bInMemory && !DatabaseInfosAsset->bOpenAsURI) )
	{
		if( !DatabaseInfosAsset->DatabaseDirectoryOverride.Path.IsEmpty() )
		{
			if( !DatabaseInfosAsset->DatabaseDirectoryOverride.Path.EndsWith( "/" ) )
			{
				DatabaseDirectory = DatabaseInfosAsset->DatabaseDirectoryOverride.Path.Append( "/" );
			}
			else
			{
				DatabaseDirectory = DatabaseInfosAsset->DatabaseDirectoryOverride.Path;
			}
		}
		else
		{
			DatabaseDirectory = USqlite3Subsystem::GetInstance()->GetDefaultDatabaseDirectory();
		}

		DatabaseFilePath = DatabaseDirectory + DatabaseInfosAsset->DatabaseFileName;

		if( DatabaseInfosAsset->bDeleteFileBeforeOpen )
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
		DatabaseFilePath = DatabaseInfosAsset->DatabaseFileName;
	}

	// ---------------------------------------------------------------------------
	// - Compute attachments -----------------------------------------------------
	// ---------------------------------------------------------------------------

	for( const auto& Attachment : DatabaseInfosAsset->Attachments )
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
	// - Paranoid setting --------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( !IsInitialized() )
	{
		UE_LOG( LogSqlite, Error, TEXT("Database not initialized.") );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// ---------------------------------------------------------------------------
	// - Open database -----------------------------------------------------------
	// ---------------------------------------------------------------------------

	LastSqliteReturnCode = sqlite3_open_v2( TCHAR_TO_ANSI(*DatabaseFilePath), &DatabaseConnectionHandler, OpenFlags, "unreal-fs" );
	if( LastSqliteReturnCode != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Open failed: (%d) %s"),
			GetErrorCode(),
			*GetErrorMessage() );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// ---------------------------------------------------------------------------
	// - Attach extra databases
	// ---------------------------------------------------------------------------

	sqlite3_stmt* stmt;

	LastSqliteReturnCode = sqlite3_prepare_v2( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *SQL_AttachDatabase ), -1, &stmt, 0 );
	if( LastSqliteReturnCode != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Failed to prepare 'AttachDatabase' statement: (%d) %s"),
			GetErrorCode(),
			*GetErrorMessage() );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

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
		UE_LOG( LogSqlite, Error, TEXT("Failed to bind or execute 'attach' statement: (%d) %s"),
			GetErrorCode(),
			*GetErrorMessage() );
	}

	sqlite3_finalize( stmt );
	stmt = nullptr;

	if( (LastSqliteReturnCode != SQLITE_OK) && (LastSqliteReturnCode != SQLITE_DONE) )
	{
		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// ---------------------------------------------------------------------------
	// - Check database for create/update ----------------------------------------
	// ---------------------------------------------------------------------------

	GetApplicationId( StoredApplicationId );
	GetUserVersion( StoredUserVersion );

	UE_LOG( LogSqlite, Log, TEXT("Database opened, current version=[%d,%d] stored version=[%d,%d]"),
		DatabaseInfosAsset->ApplicationId,
		DatabaseInfosAsset->UserVersion,
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
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		bParentOnCreateCalled = false;
		if( !OnCreate() )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnCreate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "create - public" );
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
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		UpdateApplicationId( DatabaseInfosAsset->SchemaName );
		UpdateUserVersion( DatabaseInfosAsset->SchemaName );

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

	else if( DatabaseInfosAsset->ApplicationId != StoredApplicationId )
	{
		UE_LOG( LogSqlite, Fatal, TEXT("Unexpected ApplicationId value %d"),
			StoredApplicationId );

		return ESqliteDatabaseOpenExecutionPins::OnFail;
	}

	// If file user_version does not match code user_version, we need to
	// update the database

	else if( DatabaseInfosAsset->UserVersion != StoredUserVersion )
	{
		BeginTransaction( "update" );

		if( !OnUpdatePrivate( DatabaseInfosAsset->UserVersion, StoredUserVersion ) )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnUpdatePrivate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "update - private" );
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		bParentOnUpdateCalled = false;
		if( !OnUpdate( DatabaseInfosAsset->UserVersion, StoredUserVersion ) )
		{
			UE_LOG( LogSqlite, Error, TEXT("OnUpdate failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );

			Rollback( "update - public" );
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}

		if( !bParentOnUpdateCalled )
		{
			UE_LOG( LogSqlite, Warning, TEXT("Super::OnUpdate not called") );
		}

		bParentOnUpdateCalled = false;
		ESqliteReturnCode ResultCode = ESqliteReturnCode::Unset;
		OnUpdateEvent( DatabaseInfosAsset->UserVersion, StoredUserVersion, ResultCode );

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
			return ESqliteDatabaseOpenExecutionPins::OnFail;
		}
	
		UpdateUserVersion( DatabaseInfosAsset->SchemaName );

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

bool USqliteDatabase::IsInitialized()
{
	return bIsInitialized;
}

bool USqliteDatabase::IsOpen()
{
	return bIsOpen;
}

void USqliteDatabase::Finalize()
{
	// TODO: finalize statements

	if( IsOpen() )
	{
		Close();
	}
	else
	{
		UE_LOG( LogSqlite, Log, TEXT( "Database '%s' already closed." ),
			*DatabaseFilePath );
	}
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
	UE_LOG( LogSqlite, Log, TEXT("%s()"),
		*FString( __func__ ) );

	bool error = false;

	if( DatabaseInfosAsset->bCreateStoredStatementsTable && !CreateTable( TN_StoredStatements, SQL_CreateStoredStatements ) )
	{
		error |= true;
	}

	if( DatabaseInfosAsset->bCreatePropertiesTable && !CreateTable( TN_Properties, SQL_CreateProperties ) )
	{
		error |= true;
	}

	if( DatabaseInfosAsset->bCreateActorsStoreTable && !CreateTable( TN_ActorsStore, SQL_CreateActorsStore ) )
	{
		error |= true;
	}

	if( DatabaseInfosAsset->bCreateLogTable && !CreateTable( TN_Log, SQL_CreateLog ) )
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
	UE_LOG( LogSqlite, Log, TEXT("%s() [USqliteDatabase]"),
		*FString( __func__ ) );

	bParentOnCreateCalled = true;
	return true;
}

void USqliteDatabase::OnCreateEvent_Implementation( ESqliteReturnCode& Result )
{
	UE_LOG( LogSqlite, Log, TEXT("%s() [USqliteDatabase]"),
		*FString( __func__ ) );

	bParentOnCreateCalled = true;
	Result = ESqliteReturnCode::Success;
}

// ----------------------------------------------------------------------------

bool USqliteDatabase::OnUpdatePrivate( int NewDatabaseVersion, int OldDatabaseVersion )
{
	UE_LOG( LogSqlite, Log, TEXT("%s()"),
		*FString( __func__ ) );

	return true;
}

bool USqliteDatabase::OnUpdate( int NewDatabaseVersion, int OldDatabaseVersion )
{
	UE_LOG( LogSqlite, Log, TEXT("%s( %d, %d ) [USqliteDatabase]"),
		*FString( __func__ ),
		NewDatabaseVersion,
		OldDatabaseVersion );

	bParentOnUpdateCalled = true;
	return true;
}

void USqliteDatabase::OnUpdateEvent_Implementation( int NewDatabaseVersion, int OldDatabaseVersion, ESqliteReturnCode& Result )
{
	UE_LOG( LogSqlite, Log, TEXT("%s() [USqliteDatabase]"),
		*FString( __func__ ) );

	bParentOnUpdateCalled = true;
	Result = ESqliteReturnCode::Success;
}

// ============================================================================
// === 
// ============================================================================

void USqliteDatabase::Close()
{
	UE_LOG( LogSqlite, Log, TEXT("Closing database '%s'"),
		*DatabaseFilePath );

	if( DatabaseConnectionHandler != nullptr )
	{
		if( sqlite3_close_v2( DatabaseConnectionHandler ) != SQLITE_OK )
		{
			UE_LOG( LogSqlite, Error, TEXT("Close() failed: (%d) %s"),
				GetErrorCode(),
				*GetErrorMessage() );
		}

		bIsOpen = false;
		DatabaseConnectionHandler = nullptr;
	}
}

// ============================================================================
// === application_id & user_version ==========================================
// ============================================================================

void USqliteDatabase::GetApplicationId( ESqliteDatabaseSimpleExecutionPins& Branch, int& _ApplicationId )
{
	Branch = GetApplicationId( _ApplicationId )
		? ESqliteDatabaseSimpleExecutionPins::OnSuccess
		: ESqliteDatabaseSimpleExecutionPins::OnFail;
}

bool USqliteDatabase::GetApplicationId( int& _ApplicationId )
{
	bool bReturnValue;

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2( this->DatabaseConnectionHandler, "PRAGMA application_id", -1, &stmt, 0 );
	if( rc != SQLITE_OK )
	{
		bReturnValue = false;
	}
	else
	{
		rc = sqlite3_step( stmt );
		if( rc != SQLITE_ROW )
		{
			bReturnValue = false;
		}
		else
		{
			_ApplicationId = sqlite3_column_int( stmt, 0 );
			bReturnValue = true;
		}

		sqlite3_finalize( stmt );
	}

	return bReturnValue;
}

bool USqliteDatabase::UpdateApplicationId( FName Schema )
{
	FString sql = FString::Format( TEXT( "PRAGMA \"{0}\".application_id = {1}" ), { Schema.ToString(), DatabaseInfosAsset->ApplicationId } );
	char* errmsg;

	int rc = sqlite3_exec( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *sql ), 0, 0, &errmsg );
	if( rc != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Could not set appplication_id: (%d) %s"),
			rc,
			*FString( errmsg ) );
	}
	else
	{
		StoredApplicationId = DatabaseInfosAsset->ApplicationId;
	}

	if( errmsg != nullptr )
	{
		sqlite3_free( errmsg );
	}

	return rc == SQLITE_OK;
}

// ----------------------------------------------------------------------------

void USqliteDatabase::GetUserVersion( ESqliteDatabaseSimpleExecutionPins& Branch, int& _UserVersion )
{
	Branch = GetUserVersion( _UserVersion )
		? ESqliteDatabaseSimpleExecutionPins::OnSuccess
		: ESqliteDatabaseSimpleExecutionPins::OnFail;
}

bool USqliteDatabase::GetUserVersion( int& _UserVersion )
{
	bool bReturnValue;

	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2( this->DatabaseConnectionHandler, "PRAGMA user_version", -1, &stmt, 0 );
	if( rc != SQLITE_OK )
	{
		bReturnValue = false;
	}
	else
	{
		rc = sqlite3_step( stmt );
		if( rc != SQLITE_ROW )
		{
			bReturnValue = false;
		}
		else
		{
			_UserVersion = sqlite3_column_int( stmt, 0 );
			bReturnValue = true;
		}

		sqlite3_finalize( stmt );
	}

	return bReturnValue;
}

bool USqliteDatabase::UpdateUserVersion( FName Schema )
{
	FString sql = FString::Format( TEXT( "PRAGMA \"{0}\".user_version = {1}" ), { Schema.ToString(), DatabaseInfosAsset->UserVersion } );
	char* errmsg;

	int rc = sqlite3_exec( this->DatabaseConnectionHandler, TCHAR_TO_ANSI( *sql ), 0, 0, &errmsg );
	if( rc != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Could not set user_version: (%d) %s"),
			rc,
			*FString( errmsg ) );
	}
	else
	{
		StoredUserVersion = DatabaseInfosAsset->UserVersion;
	}

	if( errmsg != nullptr )
	{
		sqlite3_free( errmsg );
	}

	return rc == SQLITE_OK;
}

// ============================================================================
// === Transactions ===========================================================
// ============================================================================

int USqliteDatabase::BeginTransaction( const FString& Hint )
{
	char* errmsg = nullptr;

	int rc = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *SQL_BeginTransaction ), nullptr, nullptr, &errmsg );
	if( rc != SQLITE_OK )
	{
		if( !Hint.IsEmpty() )
		{
			UE_LOG( LogSqlite, Error, TEXT("[Hint: %s] Failed to BEGIN transaction: (%d) %s."),
				*Hint,
				rc,
				*FString( errmsg ) );
		}
		else
		{
			UE_LOG( LogSqlite, Error, TEXT("Failed to BEGIN transaction: (%d) %s."),
				rc,
				*FString( errmsg ) );
		}
	}

	if( errmsg != nullptr )
	{
		sqlite3_free( errmsg );
	}

	return rc;
}

int USqliteDatabase::Commit( const FString& Hint )
{
	char* errmsg = nullptr;

	int rc = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *SQL_Commit ), nullptr, nullptr, &errmsg );
	if( rc != SQLITE_OK )
	{
		if( !Hint.IsEmpty() )
		{
			UE_LOG( LogSqlite, Error, TEXT("[Hint: %s] Failed to COMMIT transaction: (%d) %s."),
				*Hint,
				rc,
				*FString( errmsg ) );
		}
		else
		{
			UE_LOG( LogSqlite, Error, TEXT("Failed to COMMIT transaction: (%d) %s."),
				rc,
				*FString( errmsg ) );
		}
	}

	if( errmsg != nullptr )
	{
		sqlite3_free( errmsg );
	}

	return rc;
}

int USqliteDatabase::Rollback( const FString& Hint )
{
	char* errmsg = nullptr;

	int rc = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *SQL_Rollback ), nullptr, nullptr, &errmsg );
	if( rc != SQLITE_OK )
	{
		if( !Hint.IsEmpty() )
		{
			UE_LOG( LogSqlite, Error, TEXT("[Hint: %s] Failed to ROLLBACK transaction: (%d) %s."),
				*Hint,
				rc,
				*FString( errmsg ) );
		}
		else
		{
			UE_LOG( LogSqlite, Error, TEXT("Failed to ROLLBACK transaction: (%d) %s."),
				rc,
				*FString( errmsg ) );
		}
	}

	if( errmsg != nullptr )
	{
		sqlite3_free( errmsg );
	}

	return rc;
}

// ============================================================================
// === Errors =================================================================
// ============================================================================

int USqliteDatabase::GetErrorCode()
{
	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	int rc = sqlite3_errcode( DatabaseConnectionHandler );

	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rc;
}

int USqliteDatabase::GetExtentedErrorCode()
{
	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	int rc = sqlite3_extended_errcode( DatabaseConnectionHandler );

	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rc;
}

FString USqliteDatabase::GetErrorMessage()
{
	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	FString rs = FString( sqlite3_errmsg( DatabaseConnectionHandler ) );

	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_leave( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	return rs;
}

int USqliteDatabase::GetErrorOffset()
{
	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
	{
		sqlite3_mutex_enter( sqlite3_db_mutex( DatabaseConnectionHandler ) );
	}

	int rc = sqlite3_error_offset( DatabaseConnectionHandler );

	if( DatabaseInfosAsset->ThreadingMode == ESqliteDatabaseThreadingMode::FULL_MUTEX )
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

int USqliteDatabase::GetLastErrorCodeCxx()
{
	return LastSqliteReturnCode;
}

USqliteStatement* USqliteDatabase::Prepare( FString sql )
{
	UE_LOG( LogSqlite, Log, TEXT("Preparing statement '%s'"), *sql );

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

	return Statement;
}

FString USqliteDatabase::GetDatabaseFileName()
{
	return DatabaseInfosAsset->DatabaseFileName;
}

FString USqliteDatabase::GetDatabaseFilePath()
{
	return DatabaseFilePath;
}

// ============================================================================
// = UTILITIES ================================================================
// ============================================================================

bool USqliteDatabase::CreateTable( FName TableName, FString Sql )
{
	char* errmsg = nullptr;

	int rc = sqlite3_exec( DatabaseConnectionHandler, TCHAR_TO_ANSI( *Sql ), 0, 0, &errmsg );
	if( rc != SQLITE_OK )
	{
		UE_LOG( LogSqlite, Error, TEXT("Could not create table '%s': (%d) %s"),
			*TableName.ToString(),
			rc,
			*FString( errmsg ) );
	}
	else
	{
		UE_LOG( LogSqlite, Log, TEXT("Created table '%s' succesfully"),
			*TableName.ToString() );
	}

	if( errmsg != nullptr )
	{
		sqlite3_free( errmsg );
	}

	return rc == SQLITE_OK;
}

// ============================================================================
// = CONSTANTS ================================================================
// ============================================================================

const FString USqliteDatabase::SQL_BeginTransaction = TEXT( "BEGIN TRANSACTION;" );
const FString USqliteDatabase::SQL_Commit = TEXT( "COMMIT;" );
const FString USqliteDatabase::SQL_Rollback = TEXT( "ROLLBACK;" );

// ----------------------------------------------------------------------------

const FName USqliteDatabase::PN_DatabaseName( "@DatabaseName" );
const FName USqliteDatabase::PN_SchemaName( "@SchemaName" );

const FString USqliteDatabase::SQL_AttachDatabase = TEXT( "ATTACH DATABASE @DatabaseName AS @SchemaName;" );
const FString USqliteDatabase::SQL_DetachDatabase = TEXT( "DETACH DATABASE @SchemName;" );

const FString USqliteDatabase::SQL_ListSchemas = TEXT(
	"SELECT name "
	"FROM pragma_database_list "
	"WHERE name NOT IN ('main', 'temp') "
	"ORDER BY name;"
);

const FString USqliteDatabase::SQL_SchemaExists = TEXT(
	"SELECT COUNT(*)"
	"FROM pragma_database_list "
	"WHERE name = @SchemaName"
);

// ----------------------------------------------------------------------------

const FName USqliteDatabase::TN_StoredStatements( "StoredStatements" );
const FString USqliteDatabase::SQL_CreateStoredStatements = TEXT(
	"CREATE TABLE IF NOT EXISTS StoredStatements "
	"("
	"\"Key\" TEXT PRIMARY KEY, "
	"\"Group\" TEXT NOT NULL, "
	"\"Schema\" TEXT NOT NULL, "
	"\"Query\" TEXT NOT NULL "
	");"
);

const FName USqliteDatabase::TN_Properties( "Properties" );
const FString USqliteDatabase::SQL_CreateProperties = TEXT(
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
const FString USqliteDatabase::SQL_CreateActorsStore = TEXT(
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
const FString USqliteDatabase::SQL_CreateLog = TEXT(
	""
);
