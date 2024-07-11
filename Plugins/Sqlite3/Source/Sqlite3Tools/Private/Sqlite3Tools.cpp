// (c)2024+ Laurent Menten

#include "Sqlite3Tools.h"
#include "Sqlite3ToolsLog.h"

DEFINE_LOG_CATEGORY( LogSqliteTools );

#define LOCTEXT_NAMESPACE "FSqlite3ToolsModule"

void FSqlite3ToolsModule::StartupModule()
{
    LOG_SQLITETOOLS_WARNING( 0, " --- " );
}

void FSqlite3ToolsModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSqlite3ToolsModule, Sqlite3Tools)