// (c)2024+ Laurent Menten

#include "SqliteInteger.h"

FString USqliteInteger::ToString() const
{
	return FString::Printf( TEXT( "%ld" ), Value );
}
