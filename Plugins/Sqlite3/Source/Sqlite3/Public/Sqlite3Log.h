// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"

#include "Sqlite3/Public/SqliteStatics.h"

DECLARE_LOG_CATEGORY_EXTERN( LogSqlite, Log, All );

#define LOG_SQLITE_WARNING( ErrorCode, Message ) \
	{UE_LOG( LogSqlite, Warning, TEXT("%hs = %hs [%d] : %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );}

#define LOG_SQLITE_WARNING_TAG( ErrorCode, Tag, Message ) \
	{UE_LOG( LogSqlite, Warning, TEXT("%hs = %hs [%d] : [%hs] %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );}

#define LOG_SQLITE_ERROR( ErrorCode, Message ) \
	{UE_LOG( LogSqlite, Error, TEXT("%hs = %hs [%d] : %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Message );}

#define LOG_SQLITE_ERROR_TAG( ErrorCode, Tag, Message ) \
	{UE_LOG( LogSqlite, Error, TEXT("%hs = %hs [%d] : [%hs] %hs"), __func__, USqliteStatics::GetSqlite3ErrorSymbol(ErrorCode), ErrorCode, Tag, Message );}
