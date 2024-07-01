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


/* ========================================================================= */
/** Unreal implementation of an SQLite mutex                                 */
/* ========================================================================= */

struct FSQLiteMutex
{
	FSQLiteMutex( int InSQLiteMutexId );

	FCriticalSection CriticalSection;
	int SQLiteMutexId;
#ifdef SQLITE_DEBUG
	TAtomic<uint32> OwnerThreadId{ (uint32)INDEX_NONE };
#endif
};

/* ========================================================================= */
/** Mutex functions used by SQLite (see sqlite3_mutex_methods)               */
/* ========================================================================= */

struct FSQLiteMutexFuncs
{
public:
	/** Register the mutex system */
	static void Register();

private:
	/** Array of static mutexes used by SQLite */
	static const int32 SQLiteStaticMutexArrayCount = 12;
	static FSQLiteMutex* SQLiteStaticMutexArray[SQLiteStaticMutexArrayCount];

	/** Initialize the mutex system */
	static int Init();

	/** Shutdown the mutex system */
	static int End();

	/** Allocate a mutex */
	static sqlite3_mutex* Alloc( int InSQLiteMutexId );

	/** Free a mutex returned by Alloc */
	static void Free( sqlite3_mutex* InMutex );

	/** Lock a mutex returned by Alloc */
	static void Enter( sqlite3_mutex* InMutex );

	/** TryLock a mutex returned by Alloc */
	static int Try( sqlite3_mutex* InMutex );

	/** Unlock a mutex returned by Alloc */
	static void Leave( sqlite3_mutex* InMutex );

	/** Test whether a mutex returned by Alloc is held by the current thread (debug only) */
	static int Held( sqlite3_mutex* InMutex );

	/** Test whether a mutex returned by Alloc is not held by the current thread (debug only) */
	static int Notheld( sqlite3_mutex* InMutex );
};

#endif
