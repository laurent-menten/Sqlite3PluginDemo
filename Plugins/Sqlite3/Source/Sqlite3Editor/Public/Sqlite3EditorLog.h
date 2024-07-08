// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"

#include "Sqlite3/Public/SqliteStatics.h"

DECLARE_LOG_CATEGORY_EXTERN( LogSqliteEditor, Log, All );

FORCEINLINE void LOG_SQLITEEDITOR_WARNING( const char Func [], const int ErrorCode, const char* Message )
{
	UE_LOG( LogSqliteEditor, Warning, TEXT("%hs = %hs [%d] : %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );
}

FORCEINLINE void LOG_SQLITEEDITOR_WARNING( const char Func [], const int ErrorCode, const char* Tag, const char* Message )
{
	UE_LOG( LogSqliteEditor, Warning, TEXT("%hs = %hs [%d] : [%hs] %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );
}

FORCEINLINE void LOG_SQLITEEDITOR_ERROR( const char Func [], const int ErrorCode, const char* Message )
{
	UE_LOG( LogSqliteEditor, Error, TEXT("%hs = %hs [%d] : %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );
}

FORCEINLINE void LOG_SQLITEEDITOR_ERROR( const char Func [], const int ErrorCode, const char* Tag, const char* Message )
{
	UE_LOG( LogSqliteEditor, Error, TEXT("%hs = %hs [%d] : [%hs] %hs"), Func, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );
}
