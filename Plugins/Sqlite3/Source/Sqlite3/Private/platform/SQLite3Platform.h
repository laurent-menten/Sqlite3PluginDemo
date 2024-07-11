// Copyright Epic Games, Inc. All Rights Reserved.

#if SQLITE_OS_OTHER

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

extern "C"
{
	/** Perform additional initialization during sqlite3_initialize */
	SQLITE_API int sqlite3_os_init();

	/** Perform additional shutdown during sqlite3_shutdown */
	SQLITE_API int sqlite3_os_end();

}	// extern "C"

/** Perform additional configuration before calling sqlite3_initialize - called from FSQLiteCore::StartupModule (not a real SQLite API function) */

int sqlite3_ue_config();

#endif
