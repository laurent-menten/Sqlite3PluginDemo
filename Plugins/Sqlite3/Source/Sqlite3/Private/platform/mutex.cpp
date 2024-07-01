// Copyright Epic Games, Inc. All Rights Reserved.

#if SQLITE_OS_OTHER

#include "platform/mutex.h"
#include "Sqlite3Log.h"

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

FSQLiteMutex::FSQLiteMutex( int InSQLiteMutexId )
	: SQLiteMutexId( InSQLiteMutexId )
{
}

/* ========================================================================= */
/** Mutex functions used by SQLite (see sqlite3_mutex_methods)               */
/* ========================================================================= */

FSQLiteMutex* FSQLiteMutexFuncs::SQLiteStaticMutexArray[FSQLiteMutexFuncs::SQLiteStaticMutexArrayCount] = { 0 };

/** Register the mutex system */
void FSQLiteMutexFuncs::Register()
{
	static const sqlite3_mutex_methods MutexFuncs = {
		&Init,
		&End,
		&Alloc,
		&Free,
		&Enter,
		&Try,
		&Leave,
		&Held,
		&Notheld
	};

	sqlite3_config(SQLITE_CONFIG_MUTEX, &MutexFuncs);
}

/** Initialize the mutex system */
int FSQLiteMutexFuncs::Init()
{
	for (int32 SQLiteStaticMutexIndex = 0; SQLiteStaticMutexIndex < SQLiteStaticMutexArrayCount; ++SQLiteStaticMutexIndex)
	{
		check(!SQLiteStaticMutexArray[SQLiteStaticMutexIndex]);
		SQLiteStaticMutexArray[SQLiteStaticMutexIndex] = new FSQLiteMutex(SQLiteStaticMutexIndex + 2);
	}
		return SQLITE_OK;
}

/** Shutdown the mutex system */
int FSQLiteMutexFuncs::End()
{
	for (int32 SQLiteStaticMutexIndex = 0; SQLiteStaticMutexIndex < SQLiteStaticMutexArrayCount; ++SQLiteStaticMutexIndex)
	{
		check(SQLiteStaticMutexArray[SQLiteStaticMutexIndex]);
		delete SQLiteStaticMutexArray[SQLiteStaticMutexIndex];
		SQLiteStaticMutexArray[SQLiteStaticMutexIndex] = nullptr;
	}

	return SQLITE_OK;
}

/** Allocate a mutex */
sqlite3_mutex* FSQLiteMutexFuncs::Alloc( int InSQLiteMutexId )
{
	if (InSQLiteMutexId == SQLITE_MUTEX_FAST || InSQLiteMutexId == SQLITE_MUTEX_RECURSIVE)
	{
		return (sqlite3_mutex*)new FSQLiteMutex(InSQLiteMutexId);
	}
	else
	{
		const int32 SQLiteStaticMutexIndex = InSQLiteMutexId - 2;
		check(SQLiteStaticMutexIndex >= 0 && SQLiteStaticMutexIndex < SQLiteStaticMutexArrayCount);

		FSQLiteMutex* SQLiteStaticMutex = SQLiteStaticMutexArray[SQLiteStaticMutexIndex];
		checkSlow(SQLiteStaticMutex->SQLiteMutexId == InSQLiteMutexId);

		return (sqlite3_mutex*)SQLiteStaticMutex;
	}
}

/** Free a mutex returned by Alloc */
void FSQLiteMutexFuncs::Free(sqlite3_mutex* InMutex)
{
	FSQLiteMutex* Mutex = (FSQLiteMutex*)InMutex;
	check(Mutex);

	if (Mutex->SQLiteMutexId == SQLITE_MUTEX_FAST || Mutex->SQLiteMutexId == SQLITE_MUTEX_RECURSIVE)
	{
		delete Mutex;
	}
}

/** Lock a mutex returned by Alloc */
void FSQLiteMutexFuncs::Enter(sqlite3_mutex* InMutex)
{
	FSQLiteMutex* Mutex = (FSQLiteMutex*)InMutex;
	check(Mutex);

	Mutex->CriticalSection.Lock();
#ifdef SQLITE_DEBUG
	Mutex->OwnerThreadId = FPlatformTLS::GetCurrentThreadId();
#endif
}

/** TryLock a mutex returned by Alloc */
int FSQLiteMutexFuncs::Try(sqlite3_mutex* InMutex)
{
	FSQLiteMutex* Mutex = (FSQLiteMutex*)InMutex;
	check(Mutex);

	if (Mutex->CriticalSection.TryLock())
	{
#ifdef SQLITE_DEBUG
		Mutex->OwnerThreadId = FPlatformTLS::GetCurrentThreadId();
#endif
		return SQLITE_OK;
	}
	return SQLITE_BUSY;
}

/** Unlock a mutex returned by Alloc */
void FSQLiteMutexFuncs::Leave( sqlite3_mutex* InMutex )
{
	FSQLiteMutex* Mutex = (FSQLiteMutex*)InMutex;
	check(Mutex);

	Mutex->CriticalSection.Unlock();
#ifdef SQLITE_DEBUG
	Mutex->OwnerThreadId = (uint32)INDEX_NONE;
#endif
}

/** Test whether a mutex returned by Alloc is held by the current thread (debug only) */
int FSQLiteMutexFuncs::Held(sqlite3_mutex* InMutex)
{
	FSQLiteMutex* Mutex = (FSQLiteMutex*)InMutex;
	check(Mutex);

#ifdef SQLITE_DEBUG
	return Mutex->OwnerThreadId == FPlatformTLS::GetCurrentThreadId();
#else
	return true;
#endif
}

/** Test whether a mutex returned by Alloc is not held by the current thread (debug only) */
int FSQLiteMutexFuncs::Notheld(sqlite3_mutex* InMutex)
{
	FSQLiteMutex* Mutex = (FSQLiteMutex*)InMutex;
	check(Mutex);

#ifdef SQLITE_DEBUG
	return Mutex->OwnerThreadId != FPlatformTLS::GetCurrentThreadId();
#else
	return true;
#endif
}

#endif
