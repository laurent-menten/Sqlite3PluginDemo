// (c)2024+ Laurent Menten

#include "Sqlite3.h"
#include "Sqlite3Log.h"

DEFINE_LOG_CATEGORY( LogSqlite );

#define LOCTEXT_NAMESPACE "FSqlite3Module"

void FSqlite3Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSqlite3Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FSqlite3Module, Sqlite3 )