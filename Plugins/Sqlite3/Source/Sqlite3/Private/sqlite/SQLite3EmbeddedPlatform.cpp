// Copyright Epic Games, Inc. All Rights Reserved.

#if SQLITE_OS_OTHER

#include "sqlite/SQLite3EmbeddedPlatform.h"

#include "CoreTypes.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"
#include "Math/RandomStream.h"
#include "HAL/PlatformProcess.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformFile.h"
#include "Templates/Atomic.h"

THIRD_PARTY_INCLUDES_START
#include "sqlite/sqlite3.h-inline"
THIRD_PARTY_INCLUDES_END

#include "platform/file.h"
#include "platform/malloc.h"
#include "platform/mutex.h"

// ============================================================================
// = Perform additional initialization during sqlite3_initialize
// ============================================================================

/*extern "C"*/ SQLITE_API int sqlite3_os_init()
{
	FSQLiteFileFuncs::Register();
	return SQLITE_OK;
}

// ============================================================================
// = Perform additional shutdown during sqlite3_shutdown
// ============================================================================

/*extern "C"*/ SQLITE_API int sqlite3_os_end()
{
	return SQLITE_OK;
}

// ============================================================================
// = Perform additional configuration before calling sqlite3_initialize.
// = Called from USqlite3Subsystem::InitializeSqlite
// ============================================================================

int sqlite3_ue_config()
{
	FSQLiteMallocFuncs::Register();
	FSQLiteMutexFuncs::Register();
	
	return SQLITE_OK;
}

#endif
