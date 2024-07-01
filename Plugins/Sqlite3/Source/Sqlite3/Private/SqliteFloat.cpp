// (c)2024+ Laurent Menten

#include "SqliteFloat.h"

FString USqliteFloat::ToString() const
{
	return FString::Printf( TEXT( "%lf" ), Value );
}
