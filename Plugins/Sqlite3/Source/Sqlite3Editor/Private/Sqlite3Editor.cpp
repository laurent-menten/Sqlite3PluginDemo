// (c)2024+ Laurent Menten

#include "Sqlite3Editor.h"
#include "Sqlite3EditorLog.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "SqliteDatabaseInfoThumbnailRenderer.h"
#include "SqliteDatabaseInfoTypeActions.h"

DEFINE_LOG_CATEGORY( LogSqliteEditor );

#define LOCTEXT_NAMESPACE "FSqlite3EditorModule"

const FName FSqlite3EditorModule::AssetCategoryName = FName( TEXT( "Sqlite3Database" ) );
const FText FSqlite3EditorModule::AssetCatgegoryDisplayName = FText::FromString( TEXT( "Sqlite3 Database" ) );

// This code will execute after your module is loaded into memory;
// the exact timing is specified in the .uplugin file per-module
void FSqlite3EditorModule::StartupModule()
{
//	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	
	EAssetTypeCategories::Type AssetCategory = AssetTools.RegisterAdvancedAssetCategory( AssetCategoryName, AssetCatgegoryDisplayName );
	
	SqliteDatabaseInfoTypeActions = MakeShareable( new FSqliteDatabaseInfoTypeActions( AssetCategory ) );
	AssetTools.RegisterAssetTypeActions( SqliteDatabaseInfoTypeActions.ToSharedRef() );

	UThumbnailManager::Get().RegisterCustomRenderer( USqliteDatabaseInfo::StaticClass(), USqliteDatabaseInfoThumbnailRenderer::StaticClass() );
}

// This function may be called during shutdown to clean up your module.
// For modules that support dynamic reloading, we call this function before
// unloading the module.	
void FSqlite3EditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FSqlite3EditorModule, Sqlite3Editor )
