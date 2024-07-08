// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfo.h"
#include "Sqlite3Log.h" 

// ============================================================================
// =
// ============================================================================

USqliteDatabaseInfo::USqliteDatabaseInfo( const FObjectInitializer& ObjectInitializer )
	: UDataAsset( ObjectInitializer )
{
	UE_LOG( LogSqlite, Log, TEXT("%08.8p %s(%s %08.8p)"),
		this,
		*FString( __func__ ),
		*GetNameSafe(ObjectInitializer.GetArchetype()),
		ObjectInitializer.GetArchetype() );
}

USqliteDatabaseInfo::~USqliteDatabaseInfo()
{
	UE_LOG( LogSqlite, Log, TEXT("%08.8p %s()"),
		this,
		*FString( __func__ ) );
}

void USqliteDatabaseInfo::Test()
{
	UE_LOG( LogSqlite, Warning, TEXT("Test button clicked...") );
}
