// (c)2024+ Laurent Menten

#include "Sqlite3Editor.h"
#include "Sqlite3EditorLog.h"
#include "SqliteDatabaseInfoValidator.h"

#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Sqlite3.h"
#include "SqliteDatabaseInfoThumbnailRenderer.h"
#include "SqliteDatabaseInfoTypeActions.h"

DEFINE_LOG_CATEGORY( LogSqliteEditor );

#define LOCTEXT_NAMESPACE "FSqlite3EditorModule"

const FName FSqlite3EditorModule::AssetCategoryName = FName( TEXT( "Sqlite3Database" ) );
const FText FSqlite3EditorModule::AssetCatgegoryDisplayName = FText::FromString( TEXT( "Sqlite3 Database" ) );

// ----------------------------------------------------------------------------
// - 
// ----------------------------------------------------------------------------

class Yyyy : public ISqlite3Editor
{
public:
	virtual void Sqlite3EditorInterfaceTest( USqliteDatabaseInfo* SqliteDatabaseInfo ) override
	{
		LOG_SQLITEEDITOR_WARNING( 0, " - Y - " );

		USqliteDatabaseInfoValidator::GenerateCreateTableSqlCommands( SqliteDatabaseInfo );
	}
};

USqliteDatabaseInfoValidator* FSqlite3EditorModule::Validator = nullptr;

// ----------------------------------------------------------------------------
// - 
// ----------------------------------------------------------------------------







static Yyyy* YyyyInstance = new Yyyy();



void FSqlite3EditorModule::StartupModule()
{
	LOG_SQLITEEDITOR_WARNING( 0, " --- " );
	
	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	
	EAssetTypeCategories::Type AssetCategory = AssetTools.RegisterAdvancedAssetCategory( AssetCategoryName, AssetCatgegoryDisplayName );
	
	SqliteDatabaseInfoTypeActions = MakeShareable( new FSqliteDatabaseInfoTypeActions( AssetCategory ) );
	AssetTools.RegisterAssetTypeActions( SqliteDatabaseInfoTypeActions.ToSharedRef() );

	UThumbnailManager::Get().RegisterCustomRenderer( USqliteDatabaseInfo::StaticClass(), USqliteDatabaseInfoThumbnailRenderer::StaticClass() );

	FSqlite3Module::GetModule().RegisterSqlite3EditorInterface( YyyyInstance );
}

void FSqlite3EditorModule::ShutdownModule()
{
	FSqlite3Module::GetModule().RegisterSqlite3EditorInterface( nullptr );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FSqlite3EditorModule, Sqlite3Editor )
