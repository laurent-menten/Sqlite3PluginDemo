// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"

#include "Sqlite3/Public/SqliteStatics.h"

DECLARE_LOG_CATEGORY_EXTERN( LogSqlite, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( LogSqliteEditor, Log, All );

FORCEINLINE void LOG_SQLITE_WARNING( const char Func [], const int ErrorCode, const char* Message )
{
	UE_LOG( LogSqlite, Warning, TEXT("%hs = %hs [%d] : %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );
}

FORCEINLINE void LOG_SQLITE_WARNING( const char Func [], const int ErrorCode, const char* Tag, const char* Message )
{
	UE_LOG( LogSqlite, Warning, TEXT("%hs = %hs [%d] : [%hs] %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );
}

FORCEINLINE void LOG_SQLITE_ERROR( const char Func [], const int ErrorCode, const char* Message )
{
	UE_LOG( LogSqlite, Error, TEXT("%hs = %hs [%d] : %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );
}

FORCEINLINE void LOG_SQLITE_ERROR( const char Func [], const int ErrorCode, const char* Tag, const char* Message )
{
	UE_LOG( LogSqlite, Error, TEXT("%hs = %hs [%d] : [%hs] %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );
}

/*
#define LOG_FATAL(x, ...)		{UE_LOG( LogSqlite, Fatal, TEXT(x), __VA_ARGS__ )}
#define LOG_ERROR(x, ...)		{UE_LOG( LogSqlite, Error, TEXT(x), __VA_ARGS__ )}
#define LOG_WARNING(x, ...)		{UE_LOG( LogSqlite, Warning, TEXT(x), __VA_ARGS__ )}
#define LOG_DISPLAY(x, ...)		{UE_LOG( LogSqlite, Display, TEXT(x), __VA_ARGS__ )}
#define LOG(x, ...)				{UE_LOG( LogSqlite, Log, TEXT(x), __VA_ARGS__ )}
#define LOG_VERBOSE(x, ...)		{UE_LOG( LogSqlite, Verbose, TEXT(x), __VA_ARGS__ )}
#define LOG_VERYVERBOSE(x, ...)	{UE_LOG( LogSqlite, VeryVerbose, TEXT(x), __VA_ARGS__ )}


#define LOG_ENTER				LOG( "%08.8lX --> %s", this, *FString( __func__ ) );
#define LOG_EXIT				LOG( "%08.8lX <-- %s", this, *FString( __func__ ) );
*/
