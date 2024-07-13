// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfo.h"

#include "Sqlite3.h"
#include "Sqlite3Log.h" 
#include "UObject/ObjectSaveContext.h"

// ============================================================================
// =
// ============================================================================

USqliteDatabaseInfo::USqliteDatabaseInfo( const FObjectInitializer& ObjectInitializer )
	: UDataAsset( ObjectInitializer )
{
	UE_LOG( LogSqlite, Log, TEXT("%08.8p %s( %s %08.8p %hs)"),
		this,
		*FString( __func__ ),
		*GetNameSafe(ObjectInitializer.GetArchetype()),
		ObjectInitializer.GetArchetype(),
		this == GetClass()->GetDefaultObject() ? "[DEFAULT]" : "");

	DatabaseClass = USqliteDatabase::StaticClass();
}

USqliteDatabaseInfo::~USqliteDatabaseInfo()
{
	UE_LOG( LogSqlite, Log, TEXT("%08.8p %s()"),
		this,
		*FString( __func__ ) );
}

// ============================================================================
// =
// ============================================================================

void USqliteDatabaseInfo::PreSave( FObjectPreSaveContext SaveContext )
{
	if( const auto SqliteEditor = FSqlite3Module::GetModule().GetEditor() )
	{
		SqliteEditor->PerformAssetPreValidation( this );
	}

	Super::PreSave(SaveContext);
}
