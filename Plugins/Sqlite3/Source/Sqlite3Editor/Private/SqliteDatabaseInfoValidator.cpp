// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfoValidator.h"
#include "SqliteDatabaseInfo.h"
#include "SqliteDatabase.h"

#include "Sqlite3Editor.h"
#include "Sqlite3EditorLog.h"

#include "Misc/DataValidation.h"

#define SQLITE_OS_OTHER 1
#include "../../Sqlite3/Private/platform/malloc.cpp"

// ============================================================================
// = 
// ============================================================================

USqliteDatabaseInfoValidator::USqliteDatabaseInfoValidator()
{
	bIsEnabled = true;

	FSqlite3EditorModule::Validator = this;
}

// ============================================================================
// = 
// ============================================================================

bool USqliteDatabaseInfoValidator::CanValidateAsset_Implementation( const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext ) const
{
	if( InObject && InObject->GetClass() == USqliteDatabaseInfo::StaticClass() )
	{
		return true;
	}

	return Super::CanValidateAsset_Implementation( InAssetData, InObject, InContext );
}

// ============================================================================
// = 
// ============================================================================

EDataValidationResult USqliteDatabaseInfoValidator::ValidateLoadedAsset_Implementation( const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context )
{
	bool bSkipSqliteCheck = false;
	bool bBadData = false;

	// ---------------------------------------------------------------------------
	// 
	// ---------------------------------------------------------------------------

	USqliteDatabaseInfo* DatabaseInfos = Cast<USqliteDatabaseInfo>( InAsset );
	if( ! DatabaseInfos )
	{
		return EDataValidationResult::NotValidated;
	}

	// ---------------------------------------------------------------------------
	// Handler class check -------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( DatabaseInfos->DatabaseClass == nullptr )
	{
		Context.AddError( FText::FromString( "No managing class set." ) );
	}

	// ---------------------------------------------------------------------------
	// Zero is the default application_id value for newly created database and is 
	// used as a marker to trigger OnCreate event.
	// ---------------------------------------------------------------------------

	if( DatabaseInfos->ApplicationId == 0 ) 
	{
		Context.AddError( FText::FromString( "ApplicationId must be a non-zero value." ) );
	}

	// ---------------------------------------------------------------------------
	// Database name checks ------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( DatabaseInfos->DatabaseFileName.IsEmpty() ) // Private, temporary on-disk database
	{
		Context.AddWarning( FText::FromString( "Private, temporary on-disk databases are automatically deleted when connection is closed." ) );

		if( DatabaseInfos->bInMemory && DatabaseInfos->CacheMode != ESqliteDatabaseCacheMode::SHARED_CACHE )
		{
			Context.AddError( FText::FromString( "In memory database with shared cache enabled should have a name." ) );
		}
	}

	else if( DatabaseInfos->DatabaseFileName.StartsWith( FString(":") ) ) // Special  names
	{
		if( DatabaseInfos->DatabaseFileName.Compare( FString( ":memory:" ), ESearchCase::IgnoreCase ) ) // Private temporary in-memory database
		{
			// OK
		}
		else // Unsupported special filename
		{
			Context.AddError( FText::FromString( "Filename starts with a colon, please prefix it with \"./\" to avoid ambiguity." ) );
		}
	}

	else // Normal names
	{
		if( ! DatabaseInfos->bOpenAsURI && (DatabaseInfos->DatabaseFileName.Contains( "/" )
				|| DatabaseInfos->DatabaseFileName.Contains( "\\" )
				|| DatabaseInfos->DatabaseFileName.Contains( ":" ))
			)
		{
			Context.AddError( FText::FromString( "Database filename contains path separator but is not opened as an URI" ) );
		}
	}

	// ---------------------------------------------------------------------------
	// - Check custom tables -----------------------------------------------------
	// ---------------------------------------------------------------------------

	TArray<FString> CustomTableNames;

	int i = -1;
	for( FDatabaseTable& CustomTable : DatabaseInfos->CustomTables )
	{
		++i;

		// --------------------------------------------------------------------

		if( CustomTable.TableName.IsEmpty() )
		{
			FString message = FString::Printf( TEXT("Custom tables entry # %d: table name cannot be empty."), i );
			Context.AddError( FText::FromString( message ) );

			bSkipSqliteCheck = true;
			continue;
		}

		// --------------------------------------------------------------------
		
		if( CustomTableNames.ContainsByPredicate(
				[CustomTable]( const FString& Str )
				{
					return Str.Equals( CustomTable.TableName, ESearchCase::IgnoreCase );
				} )
			)
		{
			FString message = FString::Printf( TEXT("Table [%s] was already defined."), *CustomTable.TableName );
			Context.AddError( FText::FromString( message ) );

			bSkipSqliteCheck = true;
			continue;
		}

		// --------------------------------------------------------------------
		
		int j = -1;
		bBadData = false;
		for( FString Command : CustomTable.ExtraTableConstraints )
		{
			++j;

			if( Command.IsEmpty() )
			{
				FString message = FString::Printf( TEXT("Extra constraint entry # %d cannot be empty."), j );
				Context.AddError( FText::FromString( message ) );

				bBadData = true;
			}
		}

		if( bBadData )
		{
			bSkipSqliteCheck = true;
			continue;
		}

		// --------------------------------------------------------------------
		
		CustomTableNames.Add( CustomTable.TableName );
	}
	
	// ---------------------------------------------------------------------------
	// - Check generated SQL commands against sqlite -----------------------------
	// ---------------------------------------------------------------------------

	if( bSkipSqliteCheck )
	{
		Context.AddWarning( FText::FromString( "Due to previous error(s), check of generated SQL commands was skipped." ) );
	}
	else
	{
		SqliteCheck( DatabaseInfos, Context );
	}

	// ---------------------------------------------------------------------------
	// - Exit --------------------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( Context.GetNumErrors() != 0 )
	{
		AssetFails( InAsset, FText::FromString( "Failed..." ) );
		return EDataValidationResult::Invalid;
	}

	AssetPasses( InAsset );
	return EDataValidationResult::Valid;
}

// ============================================================================
// = 
// ============================================================================

void USqliteDatabaseInfoValidator::SqliteCheck( USqliteDatabaseInfo* DatabaseInfos, FDataValidationContext& Context )
{
	// Needed if validation is manually requested.
	// Create table commands are generated automatically when asset is saved.
		
	if( ! DatabaseInfos->bCreateDatabaseSqlCommandsGenerated )
	{
		GenerateCreateDatabaseSqlCommands( DatabaseInfos );
	}

	DatabaseInfos->bCreateDatabaseSqlCommandsGenerated = false;

	// ------------------------------------------------------------------------

	// Create an in-memory database and create the tables.
	// TODO: add other entities as well when they are implemented.
	
	FSQLiteMallocFuncs::Register();

	sqlite3_initialize();

	sqlite3* CheckDatabase;
	int ErrorCode = sqlite3_open_v2( "validation-database", &CheckDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY, nullptr );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITEEDITOR_ERROR( ErrorCode, "Failed to Open validation database." );

		Context.AddError( FText::FromString( "Failed to Open validation database." ) );
	}
	else
	{
		char* ErrorMessage = nullptr;
		for( auto& SqlCommand : DatabaseInfos->GeneratedDatabaseSqlCommands )
		{
			ErrorCode = sqlite3_exec( CheckDatabase, TCHAR_TO_ANSI(*SqlCommand.Value), nullptr, nullptr, &ErrorMessage );
			if( ErrorCode != SQLITE_OK )
			{
				FString message = FString::Printf( TEXT( "In SQL request [%s]\n\n%s\n\n%hs.\n" ),
					*SqlCommand.Key,
					*SqlCommand.Value,
					ErrorMessage );
				Context.AddError( FText::FromString( message ) );
			}

			if( ErrorMessage != nullptr )
			{
				sqlite3_free( ErrorMessage );
			}
		}

		sqlite3_close_v2( CheckDatabase );
	}

	sqlite3_shutdown();
}

// ============================================================================
// = 
// ============================================================================

void USqliteDatabaseInfoValidator::GenerateCreateDatabaseSqlCommands( USqliteDatabaseInfo* DatabaseInfos )
{
	DatabaseInfos->GeneratedDatabaseSqlCommands.Empty();

	for( auto& CustomTable : DatabaseInfos->CustomTables )
	{
		if( CustomTable.TableName.IsEmpty() )
		{
			continue;
		}

		FString CommandName = FString::Printf( TEXT("TABLE-%s"), *CustomTable.TableName );

		FString SqlCommand = GenerateCreateTableSqlCommand( CustomTable );

		DatabaseInfos->GeneratedDatabaseSqlCommands.Add( CommandName, SqlCommand );
	}

	int i = 0;
	for( auto& CustomCommand : DatabaseInfos->ExtraSqlCommands )
	{
		FString CommandName = FString::Printf( TEXT("SQL-%03.3d"), i++ );

		DatabaseInfos->GeneratedDatabaseSqlCommands.Add( CommandName, CustomCommand );
	}
	
	DatabaseInfos->bCreateDatabaseSqlCommandsGenerated = true;
}


FString USqliteDatabaseInfoValidator::GenerateCreateTableSqlCommand( const FDatabaseTable& CustomTable, const FString& SchemaName )
{
	bool bNeedComma = false;
	FString Sql = FString( "CREATE" ); 

	if( CustomTable.bTemporary )
	{
		Sql += " TEMPORARY";
	}

	Sql += " TABLE ";

	if( CustomTable.bIfNotExists )
	{
		Sql += "IF NOT EXISTS ";		
	}

	if( ! SchemaName.IsEmpty() )
	{
		Sql += SchemaName + ".";
	}

	Sql += CustomTable.TableName;
	Sql +=  + "\n(\n";

	// ------------------------------------------------------------------------
	// - Table columns
	// ------------------------------------------------------------------------

	bNeedComma = false;

	for( auto& Column : CustomTable.Columns)
	{
		if( bNeedComma )
		{
			Sql += ",\n";
		}
		
		Sql += "\t" + Column.ColumnName; // + " " + Column.ColumnAffinity;

		switch( Column.ColumnAffinity )
		{
			case ESqliteDatabaseColumnAffinity::TYPE_INTEGER:
				Sql += " INTEGER";
				break;

			case ESqliteDatabaseColumnAffinity::TYPE_REAL:
				Sql += " REAL";
				break;

			case ESqliteDatabaseColumnAffinity::TYPE_NUMERIC:
				Sql += " NUMERIC";
			break;

			case ESqliteDatabaseColumnAffinity::TYPE_TEXT:
				Sql += " TEXT";
				break;

			case ESqliteDatabaseColumnAffinity::TYPE_BLOB:
				Sql += " BLOB";
				break;
			
			case ESqliteDatabaseColumnAffinity::UNSET:
				break;
		}

		if( Column.bPrimaryKey )
		{
			Sql += " PRIMARY KEY";
		}

		if( Column.bUnique )
		{
			Sql += " UNIQUE";
		}

		if( Column.bPrimaryKey )
		{
			Sql += " NOT NULL";
		}

		if( ! Column.CheckConstraint.IsEmpty() )
		{
			Sql += " CHECK( " + Column.CheckConstraint + " )";
		}

		if( ! Column.DefaultValue.IsEmpty() )
		{
			Sql += " DEFAULT( " + Column.DefaultValue + " )";
		}

		if( ! Column.ExtraColumnConstraint.IsEmpty() )
		{
			Sql += Column.ExtraColumnConstraint;
		}
		
		bNeedComma = true;
	}

	// ------------------------------------------------------------------------
	// - Table constraints
	// ------------------------------------------------------------------------

	if( CustomTable.ExtraTableConstraints.Num() > 0 )
	{
		Sql += ",\n\n";
		bNeedComma = false;

		for( auto& Constraint : CustomTable.ExtraTableConstraints )
		{
			if( bNeedComma )
			{
				Sql += ",\n";
			}
			
			Sql += "\t" + Constraint;

			bNeedComma = true;
		}

	}
	
	Sql += "\n)";

	// ------------------------------------------------------------------------
	// - Table options --------------------------------------------------------
	// ------------------------------------------------------------------------
	
	bNeedComma = false;

	if( CustomTable.bWithoutRowId )
	{
		Sql += " WITHOUT ROWID";
		bNeedComma = true;
	}

	if( bNeedComma )
	{
		Sql += ",";
	}
	
	if( CustomTable.bStrict )
	{
		Sql += " STRICT";
		bNeedComma = true;
	}
	
	return Sql;
}
