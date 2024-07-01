// (c)2024+ Laurent Menten

#include "SqliteBlob.h"

FString USqliteBlob::ToString() const
{
	return FString::Printf( TEXT( "blob[%d]" ), Size );
}
