// (c)2024+ Laurent Menten

#include "SqliteData.h"

FString USqliteData::ToString() const
{
	return FString::Printf( TEXT( "??? base class ???" ) );
}

FString USqliteData::ToString_Blueprint() const
{
	return this->ToString();
}
