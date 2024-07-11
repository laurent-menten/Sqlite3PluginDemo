// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"

#include "Sqlite3/Public/SqliteStatics.h"

DECLARE_LOG_CATEGORY_EXTERN( LogSqliteTools, Log, All );

#define LOG_SQLITETOOLS_WARNING( ErrorCode, Message ) \
{UE_LOG( LogSqliteTools, Warning, TEXT("%hs = %hs [%d] : %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );}

#define LOG_SQLITETOOLS_WARNING_TAG( ErrorCode, Tag, Message ) \
{UE_LOG( LogSqliteTools, Warning, TEXT("%hs = %hs [%d] : [%hs] %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );}

#define LOG_SQLITETOOLS_ERROR( ErrorCode, Message ) \
{UE_LOG( LogSqliteTools, Error, TEXT("%hs = %hs [%d] : %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );}

#define LOG_SQLITETOOLS_ERROR_TAG( ErrorCode, Tag, Message ) \
{UE_LOG( LogSqliteTools, Error, TEXT("%hs = %hs [%d] : [%hs] %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );}
