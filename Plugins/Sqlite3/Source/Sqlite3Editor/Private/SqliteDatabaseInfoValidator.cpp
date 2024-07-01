// (c)2024+ Laurent Menten


#include "SqliteDatabaseInfoValidator.h"
#include "SqliteDatabase.h"
#include "SqliteDatabaseInfo.h"

#include "Misc/DataValidation.h"


USqliteDatabaseInfoValidator::USqliteDatabaseInfoValidator()
{
	bIsEnabled = true;
}

bool USqliteDatabaseInfoValidator::CanValidateAsset_Implementation( const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext ) const
{
	if( Cast<USqliteDatabaseInfo>( InObject ) )
	{
		return true;
	}

	return Super::CanValidateAsset_Implementation( InAssetData, InObject, InContext );
}

EDataValidationResult USqliteDatabaseInfoValidator::ValidateLoadedAsset_Implementation( const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context )
{
	USqliteDatabaseInfo* DatabaseInfos = Cast<USqliteDatabaseInfo>( InAsset );
	if( ! DatabaseInfos )
	{
		return EDataValidationResult::NotValidated;
	}

//	Context.AddWarning( FText::FromString( "It Works !" ) );
	
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
	TArray<FName> AttachmentSchemas;

	for( const auto& Attachment : DatabaseInfos->Attachments )
	{
		if( Attachment.FileName.IsEmpty() )
		{
			Context.AddError( FText::FromString( TEXT( "Filename cannot be empty, user ':memory:' or a valid name." ) ) );
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

		if( Attachment.SchemaName.ToString().IsEmpty() )
		{
			Context.AddError( FText::FromString( TEXT( "Schema name cannot be empty." ) ) );
		}
		else if( AttachmentSchemas.Contains( Attachment.SchemaName ) )
		{
			FString message = FString::Printf( TEXT( "Schema '%s' is already used." ), *Attachment.SchemaName.ToString() );
			Context.AddError( FText::FromString( message ) );
		}
		else
		{
			AttachmentSchemas.Add( Attachment.SchemaName );
		}
	}

	// ---------------------------------------------------------------------------
	// 
	// ---------------------------------------------------------------------------

	if( Context.GetNumErrors() == 0 )
	{
		AssetPasses( InAsset );
		return EDataValidationResult::Valid;
	}
	else
	{
		AssetFails( InAsset, FText::FromString( "failed..." ) );
		return EDataValidationResult::Invalid;
	}
}
