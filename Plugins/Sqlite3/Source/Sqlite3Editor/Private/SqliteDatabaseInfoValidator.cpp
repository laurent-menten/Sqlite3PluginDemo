// (c)2024+ Laurent Menten


#include "SqliteDatabaseInfoValidator.h"

#include "Sqlite3EditorLog.h"
#include "SqliteDatabase.h"
#include "SqliteDatabaseInfo.h"

#define SQLITE_OS_OTHER 1
#include "Sqlite3.h"
#include "Sqlite3Editor.h"
#include "../../Sqlite3/Private/platform/malloc.cpp"

#include "Misc/DataValidation.h"


USqliteDatabaseInfoValidator::USqliteDatabaseInfoValidator()
{
	LOG_SQLITEEDITOR_WARNING( 0, " --- " );

	bIsEnabled = true;

	FSqlite3EditorModule::Validator = this;
}

bool USqliteDatabaseInfoValidator::CanValidateAsset_Implementation( const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext ) const
{
	if( Cast<USqliteDatabaseInfo>( InObject ) )
	{
		return true;
	}

	return Super::CanValidateAsset_Implementation( InAssetData, InObject, InContext );
}

/*
 * TO add: validation of table.
 *
 * create a temporary database in memory database.
 * generate sql request and execute it, if it fails report failure.
 */

EDataValidationResult USqliteDatabaseInfoValidator::ValidateLoadedAsset_Implementation( const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context )
{
	USqliteDatabaseInfo* DatabaseInfos = Cast<USqliteDatabaseInfo>( InAsset );
	if( ! DatabaseInfos )
	{
		return EDataValidationResult::NotValidated;
	}

	// ---------------------------------------------------------------------------
	// Handler class check.
	// ---------------------------------------------------------------------------

	if( DatabaseInfos->DatabaseClass == nullptr )
	{
		Context.AddError( FText::FromString( "No managing class set" ) );
	}

	// ---------------------------------------------------------------------------
	// Invalid application_id. Zero is the default value for newly created database
	// and is used as a marker to trigger OnCreate event.
	// ---------------------------------------------------------------------------

	if( DatabaseInfos->ApplicationId == 0 ) 
	{
		Context.AddError( FText::FromString( "ApplicationId must be non-zero" ) );
	}

	// ---------------------------------------------------------------------------
	// Database name checks
	// ---------------------------------------------------------------------------

	if( DatabaseInfos->DatabaseFileName.IsEmpty() ) // Private temporary on-disk database
	{
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
			Context.AddError( FText::FromString( "Filename starts with colon, prefix filename with \"./\" to avoid error." ) );
		}
	}

	else // Normal names
	{
		if( !DatabaseInfos->bOpenAsURI && (DatabaseInfos->DatabaseFileName.Contains( "/" )
				|| DatabaseInfos->DatabaseFileName.Contains( "\\" )
				|| DatabaseInfos->DatabaseFileName.Contains( ":" ))
			)
		{
			Context.AddError( FText::FromString( "Database filename contains path separator but is not opened as an URI" ) );
		}
	}

	// ---------------------------------------------------------------------------
	// Check attachments.
	// ---------------------------------------------------------------------------

	TArray<FString> AttachmentNames;
	TArray<FString> AttachmentSchemas;

	for( const auto& Attachment : DatabaseInfos->Attachments )
	{
		if( Attachment.FileName.IsEmpty() )
		{
			Context.AddError( FText::FromString( TEXT( "Attachment filename cannot be empty, use ':memory:' or a valid name." ) ) );
		}
		else if( Attachment.FileName.Compare( ":memory:", ESearchCase::IgnoreCase ) != 0 )
		{
			if( AttachmentNames.Contains( Attachment.FileName ) )
			{
				FString message = FString::Printf( TEXT( "Database '%s' is already used." ), *Attachment.FileName );
				Context.AddError( FText::FromString( message ) );
			}
			else
			{
				AttachmentNames.Add( Attachment.FileName );
			}
		}

		if( Attachment.SchemaName.IsEmpty() )
		{
			Context.AddError( FText::FromString( TEXT( "Schema name cannot be empty." ) ) );
		}
		else if( AttachmentSchemas.Contains( Attachment.SchemaName ) )
		{
			FString message = FString::Printf( TEXT( "Schema '%s' is already used." ), *Attachment.SchemaName );
			Context.AddError( FText::FromString( message ) );
		}
		else
		{
			AttachmentSchemas.Add( Attachment.SchemaName );
		}
	}

	// ---------------------------------------------------------------------------
	// - Check generated table against sqlite ------------------------------------
	// ---------------------------------------------------------------------------

	if( ! DatabaseInfos->bCreateTableSqlCommandsGenerated )
	{
		// Needed if validation is manually requested.
		// Create table commands are generated automatically when asset is saved.
		
		GenerateCreateTableSqlCommands( DatabaseInfos );
	}

	// Create an in-memory database and create the tables.
	// TODO: add other entities as well when they are implemented.
	
	FSQLiteMallocFuncs::Register();
	sqlite3_initialize();
	
	sqlite3* CheckDatabase;
	int ErrorCode = sqlite3_open_v2( "check-database", &CheckDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_MEMORY, nullptr );
	if( ErrorCode != SQLITE_OK )
	{
		LOG_SQLITEEDITOR_ERROR( ErrorCode, "Open check-database" );

		Context.AddError( FText::FromString("Could not open check-database") );
	}
	else
	{
		for( auto& CreateTableSqlCommand : DatabaseInfos->GeneratedCreateTableSqlCommands )
		{
			char* ErrorMessage = nullptr;

			ErrorCode = sqlite3_exec( CheckDatabase, TCHAR_TO_ANSI(*CreateTableSqlCommand.Value), nullptr, nullptr, &ErrorMessage );
			if( ErrorCode != SQLITE_OK )
			{
				FString message = FString::Printf( TEXT( "For table [%s]\n\n%s\n\n%hs.\n" ),
					*CreateTableSqlCommand.Key,
					*CreateTableSqlCommand.Value,
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

	DatabaseInfos->bCreateTableSqlCommandsGenerated = false;
	
	// ---------------------------------------------------------------------------
	// - Exit --------------------------------------------------------------------
	// ---------------------------------------------------------------------------

	if( Context.GetNumErrors() != 0 )
	{
		DatabaseInfos->bIsValidated = false;

		AssetFails( InAsset, FText::FromString( "Failed..." ) );
		return EDataValidationResult::Invalid;
	}

	DatabaseInfos->bIsValidated = true;

	AssetPasses( InAsset );
	return EDataValidationResult::Valid;
}

void USqliteDatabaseInfoValidator::GenerateCreateTableSqlCommands( USqliteDatabaseInfo* DatabaseInfos )
{
	DatabaseInfos->GeneratedCreateTableSqlCommands.Empty();

	for( auto& CustomTable : DatabaseInfos->CustomTables )
	{
		FString sql = GenerateCreateTableSqlCommand( CustomTable );

		DatabaseInfos->GeneratedCreateTableSqlCommands.Add( CustomTable.TableName, sql );
	}

	DatabaseInfos->bCreateTableSqlCommandsGenerated = true;
}


FString USqliteDatabaseInfoValidator::GenerateCreateTableSqlCommand( const FDatabaseTable& CustomTable )
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

	if( ! CustomTable.SchemaName.IsEmpty() )
	{
		Sql += CustomTable.SchemaName + ".";
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
		
		Sql += "\t" + Column.ColumnName; // + " " + Column.ColumnType;

		bNeedComma = true;
	}

	// ------------------------------------------------------------------------
	// - Table constraints
	// ------------------------------------------------------------------------

	for( auto& Constraint : CustomTable.ExtraConstraints )
	{
		Sql += ",\n";
		Sql += Constraint;
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
