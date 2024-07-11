// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"

#include "Sqlite3/Public/SqliteStatics.h"

DECLARE_LOG_CATEGORY_EXTERN( LogSqliteEditor, Log, All );

#define LOG_SQLITEEDITOR_WARNING( ErrorCode, Message ) \
	{UE_LOG( LogSqliteEditor, Warning, TEXT("%hs = %hs [%d] : %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );}

#define LOG_SQLITEEDITOR_WARNING_TAG( ErrorCode, Tag, Message ) \
	{UE_LOG( LogSqliteEditor, Warning, TEXT("%hs = %hs [%d] : [%hs] %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );}

#define LOG_SQLITEEDITOR_ERROR( ErrorCode, Message ) \
	{UE_LOG( LogSqliteEditor, Error, TEXT("%hs = %hs [%d] : %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );}

#define LOG_SQLITEEDITOR_ERROR_TAG( ErrorCode, Tag, Message ) \
	{UE_LOG( LogSqliteEditor, Error, TEXT("%hs = %hs [%d] : [%hs] %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );}
