// (c)2024+ Laurent Menten

#include "SqliteText.h"

FString USqliteText::ToString() const
{
	return FString( (const char*) Value );
}
