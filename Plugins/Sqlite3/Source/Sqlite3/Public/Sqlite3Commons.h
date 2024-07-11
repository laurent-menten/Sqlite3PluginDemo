// (c)2024+ Laurent Menten

#pragma once

#include "Sqlite3Commons.generated.h"

// ----------------------------------------------------------------------------
// -
// ----------------------------------------------------------------------------

UINTERFACE()
class USqlite3Editor : public UInterface
{
	GENERATED_BODY()
};

// ----------------------------------------------------------------------------

class USqliteDatabaseInfo;
	
class ISqlite3Editor
{
	GENERATED_BODY()

public:
	virtual void Sqlite3EditorInterfaceTest( USqliteDatabaseInfo* SqliteDatabaseInfo ) {};
};
