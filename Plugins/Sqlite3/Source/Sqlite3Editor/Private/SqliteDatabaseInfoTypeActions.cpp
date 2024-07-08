// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfoTypeActions.h"
#include "ThumbnailRendering/SceneThumbnailInfo.h"

// ----------------------------------------------------------------------------

FSqliteDatabaseInfoTypeActions::FSqliteDatabaseInfoTypeActions( EAssetTypeCategories::Type InAssetCategory )
	: AssetCategory( InAssetCategory )
{
}

// ----------------------------------------------------------------------------

UClass* FSqliteDatabaseInfoTypeActions::GetSupportedClass() const
{
	return USqliteDatabaseInfo::StaticClass();
}

FText FSqliteDatabaseInfoTypeActions::GetName() const
{
	return INVTEXT( "Sqlite3 Database Infos" );
}

uint32 FSqliteDatabaseInfoTypeActions::GetCategories()
{
	return AssetCategory;
}

FColor FSqliteDatabaseInfoTypeActions::GetTypeColor() const
{
	return FColor::White;
}

FText FSqliteDatabaseInfoTypeActions::GetAssetDescription( const FAssetData& AssetData ) const
{
	return INVTEXT( "Sqlite3 Database connection descriptor" ); 
}

UThumbnailInfo* FSqliteDatabaseInfoTypeActions::GetThumbnailInfo( UObject* Asset ) const
{
	USqliteDatabaseInfo* DatabaseInfos = CastChecked<USqliteDatabaseInfo>(Asset);

	UThumbnailInfo* ThumbnailInfo = DatabaseInfos->ThumbnailInfo;
	if ( ThumbnailInfo == NULL )
	{
		ThumbnailInfo = NewObject<USceneThumbnailInfo>( DatabaseInfos, NAME_None, RF_Transactional );
		DatabaseInfos->ThumbnailInfo = ThumbnailInfo;
	}

	return ThumbnailInfo;
}

// ----------------------------------------------------------------------------

void FSqliteDatabaseInfoTypeActions::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
	FSimpleAssetEditor::CreateEditor( EToolkitMode::Standalone, EditWithinLevelEditor, InObjects );
}


