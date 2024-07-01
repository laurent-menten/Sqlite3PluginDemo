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
/** Unreal implementation of an SQLite file (zeroed on init)                 */
/* ========================================================================= */

struct FSQLiteFile
{
	const sqlite3_io_methods* IOMethods;
	IFileHandle* FileHandle;
	FString Filename;
	int LockMode;
	bool bDeleteOnClose;
	bool bIsReadOnly;

	static FCriticalSection CurrentlyOpenAsReadOnlySection;
	static TSet<FString> CurrentlyOpenAsReadOnly;
	static bool AllowOpenAsReadOnly(const TCHAR* InFilename);
	static void CloseAsReadOnly(const TCHAR* InFilename);
};

/* ========================================================================= *
 * File functions used by SQLite (see sqlite3_io_methods and sqlite3_vfs)
 * @note We have to make some concessions for things not exposed in the Unreal HAL that will affect multi-process concurrency (single-process access is not affected):
 *   - We do not provide an implementation for shared memory (mmap) as not all platforms implement it (see MapNamedSharedMemoryRegion and UnmapNamedSharedMemoryRegion)
 *   - We do not provide an implementation for granular file locks as our HAL doesn't expose the concept; instead we always take a writable handle for all file opens to prevent concurrent writes
 * ========================================================================= */
struct FSQLiteFileFuncs
{
public:
	/** Register the file system */
	static void Register();

private:
	/** Attempt to open a file */
	static int Open( sqlite3_vfs* InVFS, const char* InFilename, sqlite3_file* InFile, int InFlags, int* OutFlagsPtr );

	/** Close a file previously opened by Open */
	static int Close( sqlite3_file* InFile );

	/** Read from a file previously opened by Open */
	static int Read( sqlite3_file* InFile, void* OutBuffer, int InReadAmountBytes, sqlite3_int64 InReadOffsetBytes );

	/** Write to a file previously opened by Open */
	static int Write( sqlite3_file* InFile, const void* InBuffer, int InWriteAmountBytes, sqlite3_int64 InWriteOffsetBytes );

	/** Truncate a file previously opened by Open */
	static int Truncate( sqlite3_file* InFile, sqlite3_int64 InSizeBytes );

	/** Synchronize a file previously opened by Open */
	static int Sync( sqlite3_file* InFile, int InFlags );

	/** Get the size of a file previously opened by Open */
	static int FileSize( sqlite3_file* InFile, sqlite3_int64* OutSizePtr );

	/** Lock a file previously opened by Open */
	static int Lock( sqlite3_file* InFile, int InLockMode );

	/** Unlock a file previously opened by Open */
	static int Unlock( sqlite3_file* InFile, int InLockMode );

	/** Check for a lock on a file previously opened by Open */
	static int CheckReservedLock( sqlite3_file* InFile, int* OutIsLocked );

	/** Perform additional control operations on a file previously opened by Open */
	static int FileControl( sqlite3_file* InFile, int InOp, void* InOutOpData );

	/** Get the underlying sector size of a file previously opened by Open */
	static int SectorSize( sqlite3_file* InFile );

	/** Get the device characteristics of a file previously opened by Open */
	static int DeviceCharacteristics( sqlite3_file* InFile );

	/** Attempt to delete the named file */
	static int Delete( sqlite3_vfs* InVFS, const char* InFilename, int InSyncDir );

	/** Check whether the given file or directory exists on disk */
	static int Access( sqlite3_vfs* InVFS, const char* InFilename, int InAccessMode, int* OutResultPtr );

	/** Convert the given filename to be absolute */
	static int FullPathname( sqlite3_vfs* InVFS, const char* InFilename, int InOutputBufferSizeBytes, char* InOutputBuffer );

	/** Attempt to get the requested number of random bytes into the output buffer, returning the number of random bytes actually filled */
	static int Randomness( sqlite3_vfs* InVFS, int InNumRandomBytesRequested, char* InOutputBuffer );

	/** Attempt to sleep the calling thread for at least the requested number of microseconds, returning the number of microseconds actually slept */
	static int Sleep( sqlite3_vfs* InVFS, int InNumMicrosecondsRequested );

	/** Get the current time as a Julian Day value */
	static int CurrentTime( sqlite3_vfs* InVFS, double* OutTimePtr );

	/** Get the current time as a Julian Day value, multiplied the number of milliseconds in a 24-hour day (86400000) */
	static int CurrentTimeInt64( sqlite3_vfs* InVFS, sqlite3_int64* OutTimePtr );

	/** Get the last OS error */
	static int GetLastError( sqlite3_vfs* InVFS, int InOutputBufferSizeBytes, char* InOutputBuffer );
};

#endif
