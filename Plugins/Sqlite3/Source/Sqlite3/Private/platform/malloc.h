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

// ============================================================================
// = Malloc functions used by SQLite (see sqlite3_mem_methods) ================
// ============================================================================

struct FSQLiteMallocFuncs
{
public:
	/** Register the malloc system */
	static void Register();

private:

	// ------------------------------------------------------------------------
	// 
	// ------------------------------------------------------------------------
	
	/** Initialize the malloc system */
	static int Init( void* );

	/** Shutdown the malloc system */
	static void Shutdown( void* );

	// ------------------------------------------------------------------------
	// 
	// ------------------------------------------------------------------------

	/** Allocate memory */
	static void* Malloc( int InSizeBytes );

	/** Free memory returned by Alloc or Realloc */
	static void Free( void* InPtr );

	/** Reallocate memory returned by Alloc or Realloc */
	static void* Realloc( void* InPtr, int InSizeBytes );

	/** Get the actual size of an allocation returned by Alloc or Realloc */
	static int Size( void* InPtr );

	/** Roundup to the expected allocation size */
	static int Roundup( int InSizeBytes );

	// ------------------------------------------------------------------------
	// Workaround memory allocation functions which store the size in the 16
	// bytes preceding the allocation.
	// This assumes that the default alignment is no more than 16 bytes, so we
	// can just subtract 16 from any non-null pointer to get to the size.
	// ------------------------------------------------------------------------

	static constexpr int WorkaroundHeaderSize = 16;

	/** Allocate memory */
	static void* WorkaroundMalloc( int InSizeBytes );

	/** Free memory returned by Alloc or Realloc */
	static void WorkaroundFree( void* InPtr );

	/** Reallocate memory returned by Alloc or Realloc */
	static void* WorkaroundRealloc( void* InPtr, int InSizeBytes );

	/** Get the actual size of an allocation returned by Alloc or Realloc */
	static int WorkaroundSize( void* InPtr );

	/** Roundup to the expected allocation size */
	static int WorkaroundRoundup( int InSizeBytes );
};

#endif
