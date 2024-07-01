// Copyright Epic Games, Inc. All Rights Reserved.

#if SQLITE_OS_OTHER

#include "platform/malloc.h"
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

/** Register the malloc system */
void FSQLiteMallocFuncs::Register()
{
	static const sqlite3_mem_methods MallocFuncs = {
		&Malloc,
		&Free,
		&Realloc,
		&Size,
		&Roundup,
		&Init,
		&Shutdown,
		nullptr,
	};

	static const sqlite3_mem_methods WorkaroundMallocFuncs = {
		&WorkaroundMalloc,
		&WorkaroundFree,
		&WorkaroundRealloc,
		&WorkaroundSize,
		&WorkaroundRoundup,
		&Init,
		&Shutdown,
		nullptr,
	};

	// SQLite needs a working FMemory::GetAllocSize, so check if the
	// current allocator supports FMemory::GetAllocSize()

	void* TempMem = FMemory::Malloc( 16 );
	bool bSupportsGetAllocSize = FMemory::GetAllocSize( TempMem ) >= 16;
	FMemory::Free( TempMem );

	// Use the workaround functions if GetAllocSize is not supported.

	sqlite3_config( SQLITE_CONFIG_MALLOC, bSupportsGetAllocSize ? &MallocFuncs : &WorkaroundMallocFuncs );
}

// ============================================================================
// = 
// ============================================================================

/** Initialize the malloc system */
int FSQLiteMallocFuncs::Init(void*)
{
	return SQLITE_OK;
}

/** Shutdown the malloc system */
void FSQLiteMallocFuncs::Shutdown(void*)
{
}

// ============================================================================
// = 
// ============================================================================

/** Allocate memory */
void* FSQLiteMallocFuncs::Malloc(int InSizeBytes)
{
	return FMemory::Malloc(InSizeBytes, DEFAULT_ALIGNMENT);
}

/** Free memory returned by Alloc or Realloc */
void FSQLiteMallocFuncs::Free(void* InPtr)
{
	FMemory::Free(InPtr);
}

/** Reallocate memory returned by Alloc or Realloc */
void* FSQLiteMallocFuncs::Realloc(void* InPtr, int InSizeBytes)
{
	return FMemory::Realloc(InPtr, InSizeBytes, DEFAULT_ALIGNMENT);
}

/** Get the actual size of an allocation returned by Alloc or Realloc */
int FSQLiteMallocFuncs::Size(void* InPtr)
{
	return (int)FMemory::GetAllocSize(InPtr);
}

/** Roundup to the expected allocation size */
int FSQLiteMallocFuncs::Roundup(int InSizeBytes)
{
	return (int)FMemory::QuantizeSize(InSizeBytes, DEFAULT_ALIGNMENT);
}

// ============================================================================
// = 
// ============================================================================

/** Allocate memory */
void* FSQLiteMallocFuncs::WorkaroundMalloc(int InSizeBytes)
{
	void* Result = FMemory::Malloc(InSizeBytes + WorkaroundHeaderSize, DEFAULT_ALIGNMENT);
	if (Result)
	{
		*(int*)Result = InSizeBytes;
		Result = (char*)Result + WorkaroundHeaderSize;
	}
	return Result;
}

/** Free memory returned by Alloc or Realloc */
void FSQLiteMallocFuncs::WorkaroundFree(void* InPtr)
{
	if (InPtr)
	{
		FMemory::Free((char*)InPtr - WorkaroundHeaderSize);
	}
}

/** Reallocate memory returned by Alloc or Realloc */
void* FSQLiteMallocFuncs::WorkaroundRealloc(void* InPtr, int InSizeBytes)
{
	if (InPtr)
	{
		InPtr = (char*)InPtr - WorkaroundHeaderSize;
	}
	void* Result = FMemory::Realloc(InPtr, InSizeBytes + WorkaroundHeaderSize, DEFAULT_ALIGNMENT);
	if (Result)
	{
		*(int*)Result = InSizeBytes;
		Result = (char*)Result + WorkaroundHeaderSize;
	}
	return Result;
}

/** Get the actual size of an allocation returned by Alloc or Realloc */
int FSQLiteMallocFuncs::WorkaroundSize(void* InPtr)
{
	return InPtr ? *(int*)((char*)InPtr - WorkaroundHeaderSize) : 0;
}

/** Roundup to the expected allocation size */
int FSQLiteMallocFuncs::WorkaroundRoundup(int InSizeBytes)
{
	return (int)FMemory::QuantizeSize(InSizeBytes + WorkaroundHeaderSize, DEFAULT_ALIGNMENT) - WorkaroundHeaderSize;
}

#endif
