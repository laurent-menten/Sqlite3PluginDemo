// (c)2024+ Laurent Menten

#include "Sqlite3.h"
#include "Sqlite3Log.h"

DEFINE_LOG_CATEGORY( LogSqlite );

#define LOCTEXT_NAMESPACE "FSqlite3Module"

FSqlite3Module::FSqlite3Module()
{
}

void FSqlite3Module::StartupModule()
{
	LOG_SQLITE_WARNING( 0, " --- " );
}

void FSqlite3Module::ShutdownModule()
{
}

// ---

void FSqlite3Module::RegisterSqlite3EditorInterface( ISqlite3Editor* InSqlite3Editor )
{
	LOG_SQLITE_WARNING( 0, " - X - " );

	Sqlite3Editor = InSqlite3Editor;
}

ISqlite3Editor* FSqlite3Module::GetEditor() const
{
	return Sqlite3Editor;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FSqlite3Module, Sqlite3 )