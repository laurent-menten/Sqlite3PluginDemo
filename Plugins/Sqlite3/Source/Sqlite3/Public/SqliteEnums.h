// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "sqlite/Sqlite3Include.h"
#include "UObject/NoExportTypes.h"

// ============================================================================
// === 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteDatabaseEqualityExecutionPins : uint8
{
	Equal,
	NotEqual
};

UENUM( BlueprintType )
enum class ESqliteDatabaseSimpleExecutionPins : uint8
{
	OnSuccess,
	OnFail
};

// ----------------------------------------------------------------------------

/**
 * Return value for blueprint events
 */
UENUM( BlueprintType )
enum class ESqliteReturnCode : uint8
{
	Unset		UMETA(Hidden),
	Success,
	Failure
};

// ----------------------------------------------------------------------------

UENUM( BlueprintType )
enum class ESqliteDatabaseOpenExecutionPins : uint8
{
    OnSuccess,		// Database successfully opened
	OnCreate,		// Database was created (application_id was 0)
	OnUpdate,		// Database need update (user_version mismatch)
	OnFail			// Failed to open database
};

// ============================================================================
// === 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteDatabaseOpenReturnCodes : uint8
{
	Ok,
	InvalidApplicationId,
	EmptyDatabaseFileName,
	InvalidDatabaseFileName,
	ApplicationIdMismatch,
	OpenFailed
};

// ============================================================================
// === 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteType : uint8
{
	Integer,
	Float,
	Blob,
	Null,
	Text,

	None
};

// ============================================================================
// === 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteStatementStatus : uint8
{
	FullScanStep	= SQLITE_STMTSTATUS_FULLSCAN_STEP,
	Sort			= SQLITE_STMTSTATUS_SORT,
	AutoIndex		= SQLITE_STMTSTATUS_AUTOINDEX,
	VMStep			= SQLITE_STMTSTATUS_VM_STEP,
	Reprepare		= SQLITE_STMTSTATUS_REPREPARE,
	Run				= SQLITE_STMTSTATUS_RUN,
	FilterMiss		= SQLITE_STMTSTATUS_FILTER_MISS,
	FilterHit		= SQLITE_STMTSTATUS_FILTER_HIT,
	MemUsed			= SQLITE_STMTSTATUS_MEMUSED,

	None			= 0 UMETA( Hidden ),
};

// ----------------------------------------------------------------------------

UENUM( BlueprintType )
enum class ESqliteDatabaseStatus : uint8
{
	LookasideUsed		= SQLITE_DBSTATUS_LOOKASIDE_USED,
	LookasideHit		= SQLITE_DBSTATUS_LOOKASIDE_HIT,
	LookasideMissSize	= SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE,
	LookasideMissFull	= SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL,
	CacheUsed			= SQLITE_DBSTATUS_CACHE_USED,
	CacheUsedShared		= SQLITE_DBSTATUS_CACHE_USED_SHARED,
	CacheHit			= SQLITE_DBSTATUS_CACHE_HIT,
	CacheMiss			= SQLITE_DBSTATUS_CACHE_MISS,
	CacheWrite			= SQLITE_DBSTATUS_CACHE_WRITE,
	CacheSpill			= SQLITE_DBSTATUS_CACHE_SPILL,
	SchemaUsed			= SQLITE_DBSTATUS_SCHEMA_USED,
	StatementUsed		= SQLITE_DBSTATUS_STMT_USED,

	None				= 0 UMETA(Hidden),
};

// ============================================================================
// === 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteErrorCode : uint8
{
	Ok,							/* No error */

	Row,						/* sqlite3_step() has another row ready */
	Done,						/* sqlite3_step() has finished executing */

	Error,						/* Generic error */
	Internal,					/* Internal logic error in  */
	Perm,						/* Access permission denied */
	Abort,						/* Callback routine requested an abort */
	Busy,						/* The database file is locked */
	Locked,						/* A table in the database is locked */
	NoMem,						/* A malloc() failed */
	ReadOnly,					/* Attempt to write a read-only database */
	Interrupt,					/* Operation terminated by 3_interrupt()*/
	IOErr,						/* Some kind of disk I/O error occurred */
	Corrupt,					/* The database disk image is malformed */
	NotFound,					/* Unknown opcode in 3_file_control() */
	Full,						/* Insertion failed because database is full */
	CantOpen,					/* Unable to open the database file */
	Protocol,					/* Database lock protocol error */
	Empty,						/* Internal use only */
	Schema,						/* The database schema changed */
	TooBig,						/* String or BLOB exceeds size limit */
	Constaint,					/* Abort due to constraint violation */
	Mismatch,					/* Data type mismatch */
	Misuse,						/* Library used incorrectly */
	NoLFS,						/* Uses OS features not supported on host */
	Auth,						/* Authorization denied */
	Format,						/* Not used */
	Range,						/* 2nd parameter to 3_bind out of range */
	NotADB,						/* File opened that is not a database file */
	Notice,						/* Notifications from sqlite3_log() */
	Warning,					/* Warnings from 3_log() */
};

UENUM( BlueprintType )
enum class ESqliteExtendedErrorCode : uint8
{
	Ok,							/* No error */

	Row,						/* sqlite3_step() has another row ready */
	Done,						/* sqlite3_step() has finished executing */

	Error,						/* Generic error */
	Internal,					/* Internal logic error in  */
	Perm,						/* Access permission denied */
	Abort,						/* Callback routine requested an abort */
	Busy,						/* The database file is locked */
	Locked,						/* A table in the database is locked */
	NoMem,						/* A malloc() failed */
	ReadOnly,					/* Attempt to write a read-only database */
	Interrupt,					/* Operation terminated by 3_interrupt()*/
	IOErr,						/* Some kind of disk I/O error occurred */
	Corrupt,					/* The database disk image is malformed */
	NotFound,					/* Unknown opcode in 3_file_control() */
	Full,						/* Insertion failed because database is full */
	CantOpen,					/* Unable to open the database file */
	Protocol,					/* Database lock protocol error */
	Empty,						/* Internal use only */
	Schema,						/* The database schema changed */
	TooBig,						/* String or BLOB exceeds size limit */
	Constaint,					/* Abort due to constraint violation */
	Mismatch,					/* Data type mismatch */
	Misuse,						/* Library used incorrectly */
	NoLFS,						/* Uses OS features not supported on host */
	Auth,						/* Authorization denied */
	Format,						/* Not used */
	Range,						/* 2nd parameter to 3_bind out of range */
	NotADB,						/* File opened that is not a database file */
	Notice,						/* Notifications from sqlite3_log() */
	Warning,					/* Warnings from 3_log() */

	// Extended error codes

	OkLoadPermanently,			/* Extension will not unload at db close*/

	ErrorMissingCollSeq,		/**/
	ErrorRetry,					/**/
	ErrorSnapshot,				/**/

	AbortRollback,				/**/

	BusyRecovery,				/**/
	BusySnapshot,				/**/
	BusyTimeOut,				/**/

	LockedSharedCache,			/**/
	LockedVTab,					/**/

	ReadOnlyRecovery,			/**/
	ReadOnlyCantLock,			/**/
	ReadOnlyRollback,			/**/
	ReadOnlyDBMoved,			/**/
	ReadOnlyCantInit,			/**/
	ReadOnlyDirectory,			/**/

	IOErrRead,					/**/
	IOErrShortRead,				/**/
	IOErrWrite,					/**/
	IOErrFSync,					/**/
	IOErrDirFSync,				/**/
	IOErrTruncate,				/**/
	IOErrFStat,					/**/
	IOErrUnlock,				/**/
	IOErrRDLock,				/**/
	IOErrDelete,				/**/
	IOErrBlocked,				/**/
	IOErrNoMem,					/**/
	IOErrAccess,				/**/
	IOErrCheckReservedLock,		/**/
	IOErrLock,					/**/
	IOErrClose,					/**/
	IOErrDirClose,				/**/
	IOErrShmOpen,				/**/
	IOErrShmSize,				/**/
	IOErrShmLock,				/**/
	IOErrShmMap,				/**/
	IOErrSeek,					/**/
	IOErrDeleteNoEnt,			/**/
	IOErrMMap,					/**/
	IOErrGetTempPath,			/**/
	IOErrConvPath,				/**/
	IOErrVNode,					/**/
	IOErrAuth,					/**/
	IOErrBeginAtomic,			/**/
	IOErrCommitAtomic,			/**/
	IOErrRollbackAtomic,		/**/
	IOErrData,					/**/
	IOErrCorruptFS,				/**/

	CorruptVTab,				/**/
	CorruptSequence,			/**/
	CorruptIndex,				/**/
	
	CantOpenNoTempDir,			/**/
	CantOpenIsDir,				/**/
	CantOpenFullPath,			/**/
	CantOpen_ConvPath,			/**/
	CantOpenDirtyWal,			/* Not Used */
	CantOpenSymLink,			/**/

	ConstraintCheck,			/**/
	ConstraintCommitHook,		/**/
	ConstraintForeignKey,		/**/
	ConstraintFunction,			/**/
	ConstraintNotNull,			/**/
	ConstraintPrimaryKey,		/**/
	ConstraintTrigger,			/**/
	ConstraintUnique,			/**/
	ConstraintVTab,				/**/
	ConstraintRowId,			/**/
	ConstraintPinned,			/**/
	ConstraintDataType,			/**/

	AuthUser,					/**/

	NoticeRecoverWal,			/**/
	NoticeRecoverRollback,		/**/
	NoticeRBU,					/**/

	WarningAutoIndex			/**/
};
