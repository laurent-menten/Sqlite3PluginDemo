// (c)2024+ Laurent Menten

#include "SqliteStatics.h"
#include "Sqlite3Log.h"
#include "Sqlite3Subsystem.h"

#include "CoreMinimal.h"

// ============================================================================
// === 
// ============================================================================

FString USqliteStatics::LibVersion()
{
	return FString( sqlite3_libversion() );
}

int USqliteStatics::LibVersionNumber()
{
	return sqlite3_libversion_number();
}

FString USqliteStatics::LibSourceId()
{
	return FString( sqlite3_sourceid() );
}

// ============================================================================
// === 
// ============================================================================

int USqliteStatics::IsThreadSafe()
{
	return sqlite3_threadsafe();
}

// ============================================================================
// === 
// ============================================================================

const char* USqliteStatics::GetSqlite3ErrorSymbol( int ErrorCode )
{
		switch( ErrorCode )
	{
		case SQLITE_OK:							return "SQLITE_OK";
		case SQLITE_OK_LOAD_PERMANENTLY:		return "SQLITE_OK_LOAD_PERMANENTLY";

		case SQLITE_ERROR:						return "SQLITE_ERROR";
		case SQLITE_ERROR_MISSING_COLLSEQ:		return "SQLITE_ERROR_MISSING_COLLSEQ";
		case SQLITE_ERROR_RETRY:				return "SQLITE_ERROR_RETRY";
		case SQLITE_ERROR_SNAPSHOT:				return "SQLITE_ERROR_SNAPSHOT";

		case SQLITE_INTERNAL:					return "SQLITE_INTERNAL";

		case SQLITE_PERM:						return "SQLITE_PERM";

		case SQLITE_ABORT:						return "SQLITE_ABORT";
		case SQLITE_ABORT_ROLLBACK:				return "SQLITE_ABORT_ROLLBACK";

		case SQLITE_BUSY:						return "SQLITE_BUSY";
		case SQLITE_BUSY_RECOVERY:				return "SQLITE_BUSY_RECOVERY";
		case SQLITE_BUSY_SNAPSHOT:				return "SQLITE_BUSY_SNAPSHOT";
		case SQLITE_BUSY_TIMEOUT:				return "SQLITE_BUSY_TIMEOUT";

		case SQLITE_LOCKED:						return "SQLITE_LOCKED";
		case SQLITE_LOCKED_SHAREDCACHE:			return "SQLITE_LOCKED_SHAREDCACHE";
		case SQLITE_LOCKED_VTAB:				return "SQLITE_LOCKED_VTAB";

		case SQLITE_NOMEM:						return "SQLITE_NOMEM";

		case SQLITE_READONLY:					return "SQLITE_READONLY";
		case SQLITE_READONLY_RECOVERY:			return "SQLITE_READONLY_RECOVERY";
		case SQLITE_READONLY_CANTLOCK:			return "SQLITE_READONLY_CANTLOCK";
		case SQLITE_READONLY_ROLLBACK:			return "SQLITE_READONLY_ROLLBACK";
		case SQLITE_READONLY_DBMOVED:			return "SQLITE_READONLY_DBMOVED";
		case SQLITE_READONLY_CANTINIT:			return "SQLITE_READONLY_CANTINIT";
		case SQLITE_READONLY_DIRECTORY:			return "SQLITE_READONLY_DIRECTORY";

		case SQLITE_INTERRUPT:					return "SQLITE_INTERRUPT";

		case SQLITE_IOERR:						return "SQLITE_IOERR";
		case SQLITE_IOERR_READ:					return "SQLITE_IOERR_READ";
		case SQLITE_IOERR_SHORT_READ:			return "SQLITE_IOERR_SHORT_READ";
		case SQLITE_IOERR_WRITE:				return "SQLITE_IOERR_WRITE";
		case SQLITE_IOERR_FSYNC:				return "SQLITE_IOERR_FSYNC";
		case SQLITE_IOERR_DIR_FSYNC:			return "SQLITE_IOERR_DIR_FSYNC";
		case SQLITE_IOERR_TRUNCATE:				return "SQLITE_IOERR_TRUNCATE";
		case SQLITE_IOERR_FSTAT:				return "SQLITE_IOERR_FSTAT";
		case SQLITE_IOERR_UNLOCK:				return "SQLITE_IOERR_UNLOCK";
		case SQLITE_IOERR_RDLOCK:				return "SQLITE_IOERR_RDLOCK";
		case SQLITE_IOERR_DELETE:				return "SQLITE_IOERR_DELETE";
		case SQLITE_IOERR_BLOCKED:				return "SQLITE_IOERR_BLOCKED";
		case SQLITE_IOERR_NOMEM:				return "SQLITE_IOERR_NOMEM";
		case SQLITE_IOERR_ACCESS:				return "SQLITE_IOERR_ACCESS";
		case SQLITE_IOERR_CHECKRESERVEDLOCK:	return "SQLITE_IOERR_CHECKRESERVEDLOCK";
		case SQLITE_IOERR_LOCK:					return "SQLITE_IOERR_LOCK";
		case SQLITE_IOERR_CLOSE:				return "SQLITE_IOERR_CLOSE";
		case SQLITE_IOERR_DIR_CLOSE:			return "SQLITE_IOERR_DIR_CLOSE";
		case SQLITE_IOERR_SHMOPEN:				return "SQLITE_IOERR_SHMOPEN";
		case SQLITE_IOERR_SHMSIZE:				return "SQLITE_IOERR_SHMSIZE";
		case SQLITE_IOERR_SHMLOCK:				return "SQLITE_IOERR_SHMLOCK";
		case SQLITE_IOERR_SHMMAP:				return "SQLITE_IOERR_SHMMAP";
		case SQLITE_IOERR_SEEK:					return "SQLITE_IOERR_SEEK";
		case SQLITE_IOERR_DELETE_NOENT:			return "SQLITE_IOERR_DELETE_NOENT";
		case SQLITE_IOERR_MMAP:					return "SQLITE_IOERR_MMAP";
		case SQLITE_IOERR_GETTEMPPATH:			return "SQLITE_IOERR_GETTEMPPATH";
		case SQLITE_IOERR_CONVPATH:				return "SQLITE_IOERR_CONVPATH";
		case SQLITE_IOERR_VNODE:				return "SQLITE_IOERR_VNODE";
		case SQLITE_IOERR_AUTH:					return "SQLITE_IOERR_AUTH";
		case SQLITE_IOERR_BEGIN_ATOMIC:			return "SQLITE_IOERR_BEGIN_ATOMIC";
		case SQLITE_IOERR_COMMIT_ATOMIC:		return "SQLITE_IOERR_COMMIT_ATOMIC";
		case SQLITE_IOERR_ROLLBACK_ATOMIC:		return "SQLITE_IOERR_ROLLBACK_ATOMIC";
		case SQLITE_IOERR_DATA:					return "SQLITE_IOERR_DATA";
		case SQLITE_IOERR_CORRUPTFS:			return "SQLITE_IOERR_CORRUPTFS";

		case SQLITE_CORRUPT:					return "SQLITE_CORRUPT";
		case SQLITE_CORRUPT_VTAB:				return "SQLITE_CORRUPT_VTAB";
		case SQLITE_CORRUPT_SEQUENCE:			return "SQLITE_CORRUPT_SEQUENCE";
		case SQLITE_CORRUPT_INDEX:				return "SQLITE_CORRUPT_INDEX";

		case SQLITE_NOTFOUND:					return "SQLITE_NOTFOUND";

		case SQLITE_FULL:						return "SQLITE_FULL";

		case SQLITE_CANTOPEN:					return "SQLITE_CANTOPEN";
		case SQLITE_CANTOPEN_NOTEMPDIR:			return "SQLITE_CANTOPEN_NOTEMPDIR";
		case SQLITE_CANTOPEN_ISDIR:				return "SQLITE_CANTOPEN_ISDIR";
		case SQLITE_CANTOPEN_FULLPATH:			return "SQLITE_CANTOPEN_FULLPATH";
		case SQLITE_CANTOPEN_CONVPATH:			return "SQLITE_CANTOPEN_CONVPATH";
		case SQLITE_CANTOPEN_DIRTYWAL:			return "SQLITE_CANTOPEN_DIRTYWAL";
		case SQLITE_CANTOPEN_SYMLINK:			return "SQLITE_CANTOPEN_SYMLINK";

		case SQLITE_PROTOCOL:					return "SQLITE_PROTOCOL";

		case SQLITE_EMPTY:						return "SQLITE_EMPTY";

		case SQLITE_SCHEMA:						return "SQLITE_SCHEMA";

		case SQLITE_TOOBIG:						return "SQLITE_TOOBIG";

		case SQLITE_CONSTRAINT:					return "SQLITE_CONSTRAINT";
		case SQLITE_CONSTRAINT_CHECK:			return "SQLITE_CONSTRAINT_CHECK";
		case SQLITE_CONSTRAINT_COMMITHOOK:		return "SQLITE_CONSTRAINT_COMMITHOOK";
		case SQLITE_CONSTRAINT_FOREIGNKEY:		return "SQLITE_CONSTRAINT_FOREIGNKEY";
		case SQLITE_CONSTRAINT_FUNCTION:		return "SQLITE_CONSTRAINT_FUNCTION";
		case SQLITE_CONSTRAINT_NOTNULL:			return "SQLITE_CONSTRAINT_NOTNULL";
		case SQLITE_CONSTRAINT_PRIMARYKEY:		return "SQLITE_CONSTRAINT_PRIMARYKEY";
		case SQLITE_CONSTRAINT_TRIGGER:			return "SQLITE_CONSTRAINT_TRIGGER";
		case SQLITE_CONSTRAINT_UNIQUE:			return "SQLITE_CONSTRAINT_UNIQUE";
		case SQLITE_CONSTRAINT_VTAB:			return "SQLITE_CONSTRAINT_VTAB";
		case SQLITE_CONSTRAINT_ROWID:			return "SQLITE_CONSTRAINT_ROWID";
		case SQLITE_CONSTRAINT_PINNED:			return "SQLITE_CONSTRAINT_PINNED";
		case SQLITE_CONSTRAINT_DATATYPE:		return "SQLITE_CONSTRAINT_DATATYPE";

		case SQLITE_MISMATCH:					return "SQLITE_MISMATCH";

		case SQLITE_MISUSE:						return "SQLITE_MISUSE";

		case SQLITE_NOLFS:						return "SQLITE_NOLFS";

		case SQLITE_AUTH:						return "SQLITE_AUTH";
		case SQLITE_AUTH_USER:					return "SQLITE_AUTH_USER";

		case SQLITE_FORMAT:						return "SQLITE_FORMAT";

		case SQLITE_RANGE:						return "SQLITE_RANGE";

		case SQLITE_NOTADB:						return "SQLITE_NOTADB";

		case SQLITE_NOTICE:						return "SQLITE_NOTICE";
		case SQLITE_NOTICE_RECOVER_WAL:			return "SQLITE_NOTICE_RECOVER_WAL";
		case SQLITE_NOTICE_RECOVER_ROLLBACK:	return "SQLITE_NOTICE_RECOVER_ROLLBACK";
		case SQLITE_NOTICE_RBU:					return "SQLITE_NOTICE_RBU";

		case SQLITE_WARNING:					return "SQLITE_WARNING";
		case SQLITE_WARNING_AUTOINDEX:			return "SQLITE_WARNING_AUTOINDEX";

		case SQLITE_ROW:						return "SQLITE_ROW";

		case SQLITE_DONE:						return "SQLITE_DONE";

		default:								return "???";
	}
}

// ============================================================================
// === 
// ============================================================================

ESqliteErrorCode USqliteStatics::MapNativeErrorCode( int ErrorCode )
{
	switch( ErrorCode )
	{
		case SQLITE_OK:							return ESqliteErrorCode::Ok;
		case SQLITE_ERROR:						return ESqliteErrorCode::Error;
		case SQLITE_INTERNAL:					return ESqliteErrorCode::Internal;
		case SQLITE_PERM:						return ESqliteErrorCode::Perm;
		case SQLITE_ABORT:						return ESqliteErrorCode::Abort;
		case SQLITE_BUSY:						return ESqliteErrorCode::Busy;
		case SQLITE_LOCKED:						return ESqliteErrorCode::Locked;
		case SQLITE_NOMEM:						return ESqliteErrorCode::NoMem;
		case SQLITE_READONLY:					return ESqliteErrorCode::ReadOnly;
		case SQLITE_INTERRUPT:					return ESqliteErrorCode::Interrupt;
		case SQLITE_IOERR:						return ESqliteErrorCode::IOErr;
		case SQLITE_CORRUPT:					return ESqliteErrorCode::Corrupt;
		case SQLITE_NOTFOUND:					return ESqliteErrorCode::NotFound;
		case SQLITE_FULL:						return ESqliteErrorCode::Full;
		case SQLITE_CANTOPEN:					return ESqliteErrorCode::CantOpen;
		case SQLITE_PROTOCOL:					return ESqliteErrorCode::Protocol;
		case SQLITE_EMPTY:						return ESqliteErrorCode::Empty;
		case SQLITE_SCHEMA:						return ESqliteErrorCode::Schema;
		case SQLITE_TOOBIG:						return ESqliteErrorCode::TooBig;
		case SQLITE_CONSTRAINT:					return ESqliteErrorCode::Constaint;
		case SQLITE_MISMATCH:					return ESqliteErrorCode::Mismatch;
		case SQLITE_MISUSE:						return ESqliteErrorCode::Misuse;
		case SQLITE_NOLFS:						return ESqliteErrorCode::NoLFS;
		case SQLITE_AUTH:						return ESqliteErrorCode::Auth;
		case SQLITE_FORMAT:						return ESqliteErrorCode::Format;
		case SQLITE_RANGE:						return ESqliteErrorCode::Range;
		case SQLITE_NOTADB:						return ESqliteErrorCode::NotADB;
		case SQLITE_NOTICE:						return ESqliteErrorCode::Notice;
		case SQLITE_WARNING:					return ESqliteErrorCode::Warning;
		case SQLITE_ROW:						return ESqliteErrorCode::Row;
		case SQLITE_DONE:						return ESqliteErrorCode::Done;

		default:
			UE_LOG( LogSqlite, Error, TEXT( "Unexpected Sqlite native error code: %d" ), ErrorCode );
			return ESqliteErrorCode::Error;
	}
}

int USqliteStatics::UnmapNativeErrorCode( ESqliteErrorCode ErrorCode )
{
	switch( ErrorCode )
	{
		case ESqliteErrorCode::Ok:						return SQLITE_OK;
		case ESqliteErrorCode::Error:					return SQLITE_ERROR;
		case ESqliteErrorCode::Internal:				return SQLITE_INTERNAL;
		case ESqliteErrorCode::Perm:					return SQLITE_PERM;
		case ESqliteErrorCode::Abort:					return SQLITE_ABORT;
		case ESqliteErrorCode::Busy:					return SQLITE_BUSY;
		case ESqliteErrorCode::Locked:					return SQLITE_LOCKED;
		case ESqliteErrorCode::NoMem:					return SQLITE_NOMEM;
		case ESqliteErrorCode::ReadOnly:				return SQLITE_READONLY;
		case ESqliteErrorCode::Interrupt:				return SQLITE_INTERRUPT;
		case ESqliteErrorCode::IOErr:					return SQLITE_IOERR;
		case ESqliteErrorCode::Corrupt:					return SQLITE_CORRUPT;
		case ESqliteErrorCode::NotFound:				return SQLITE_NOTFOUND;
		case ESqliteErrorCode::Full:					return SQLITE_FULL;
		case ESqliteErrorCode::CantOpen:				return SQLITE_CANTOPEN;
		case ESqliteErrorCode::Protocol:				return SQLITE_PROTOCOL;
		case ESqliteErrorCode::Empty:					return SQLITE_EMPTY;
		case ESqliteErrorCode::Schema:					return SQLITE_SCHEMA;
		case ESqliteErrorCode::TooBig:					return SQLITE_TOOBIG;
		case ESqliteErrorCode::Constaint:				return SQLITE_CONSTRAINT;
		case ESqliteErrorCode::Mismatch:				return SQLITE_MISMATCH;
		case ESqliteErrorCode::Misuse:					return SQLITE_MISUSE;
		case ESqliteErrorCode::NoLFS:					return SQLITE_NOLFS;
		case ESqliteErrorCode::Auth:					return SQLITE_AUTH;
		case ESqliteErrorCode::Format:					return SQLITE_FORMAT;
		case ESqliteErrorCode::Range:					return SQLITE_RANGE;
		case ESqliteErrorCode::NotADB:					return SQLITE_NOTADB;
		case ESqliteErrorCode::Notice:					return SQLITE_NOTICE;
		case ESqliteErrorCode::Warning:					return SQLITE_WARNING;
		case ESqliteErrorCode::Row:						return SQLITE_ROW;
		case ESqliteErrorCode::Done:					return SQLITE_DONE;

		default:
			UE_LOG( LogSqlite, Error, TEXT( "Unexpected ESqliteErrorCode value: '%s'" ), *UEnum::GetValueAsString( ErrorCode ) );
			return SQLITE_ERROR;
	}
}

bool USqliteStatics::CheckSqliteErrorCode( int NativeErrorCode, ESqliteErrorCode ErrorCode )
{
	return NativeErrorCode == UnmapNativeErrorCode( ErrorCode );
}

void USqliteStatics::BranchIfSqliteErrorCode( int NativeErrorCode, ESqliteErrorCode ErrorCode, ESqliteDatabaseEqualityExecutionPins& Branch )
{
	const bool rc =  CheckSqliteErrorCode( NativeErrorCode, ErrorCode );
	Branch = rc ? ESqliteDatabaseEqualityExecutionPins::Equal : ESqliteDatabaseEqualityExecutionPins::NotEqual;
}

// ----------------------------------------------------------------------------

ESqliteExtendedErrorCode USqliteStatics::MapNativeExtendedErrorCode( int ErrorCode )
{
	switch( ErrorCode )
	{
		case SQLITE_OK:							return ESqliteExtendedErrorCode::Ok;
		case SQLITE_OK_LOAD_PERMANENTLY:		return ESqliteExtendedErrorCode::OkLoadPermanently;

		case SQLITE_ERROR:						return ESqliteExtendedErrorCode::Error;
		case SQLITE_ERROR_MISSING_COLLSEQ:		return ESqliteExtendedErrorCode::ErrorMissingCollSeq;
		case SQLITE_ERROR_RETRY:				return ESqliteExtendedErrorCode::ErrorRetry;
		case SQLITE_ERROR_SNAPSHOT:				return ESqliteExtendedErrorCode::ErrorSnapshot;

		case SQLITE_INTERNAL:					return ESqliteExtendedErrorCode::Internal;

		case SQLITE_PERM:						return ESqliteExtendedErrorCode::Perm;

		case SQLITE_ABORT:						return ESqliteExtendedErrorCode::Abort;
		case SQLITE_ABORT_ROLLBACK:				return ESqliteExtendedErrorCode::AbortRollback;

		case SQLITE_BUSY:						return ESqliteExtendedErrorCode::Busy;
		case SQLITE_BUSY_RECOVERY:				return ESqliteExtendedErrorCode::BusyRecovery;
		case SQLITE_BUSY_SNAPSHOT:				return ESqliteExtendedErrorCode::BusySnapshot;
		case SQLITE_BUSY_TIMEOUT:				return ESqliteExtendedErrorCode::BusyTimeOut;

		case SQLITE_LOCKED:						return ESqliteExtendedErrorCode::Locked;
		case SQLITE_LOCKED_SHAREDCACHE:			return ESqliteExtendedErrorCode::LockedSharedCache;
		case SQLITE_LOCKED_VTAB:				return ESqliteExtendedErrorCode::LockedVTab;

		case SQLITE_NOMEM:						return ESqliteExtendedErrorCode::NoMem;

		case SQLITE_READONLY:					return ESqliteExtendedErrorCode::ReadOnly;
		case SQLITE_READONLY_RECOVERY:			return ESqliteExtendedErrorCode::ReadOnlyRecovery;
		case SQLITE_READONLY_CANTLOCK:			return ESqliteExtendedErrorCode::ReadOnlyCantLock;
		case SQLITE_READONLY_ROLLBACK:			return ESqliteExtendedErrorCode::ReadOnlyRollback;
		case SQLITE_READONLY_DBMOVED:			return ESqliteExtendedErrorCode::ReadOnlyDBMoved;
		case SQLITE_READONLY_CANTINIT:			return ESqliteExtendedErrorCode::ReadOnlyCantInit;
		case SQLITE_READONLY_DIRECTORY:			return ESqliteExtendedErrorCode::ReadOnlyDirectory;

		case SQLITE_INTERRUPT:					return ESqliteExtendedErrorCode::Interrupt;

		case SQLITE_IOERR:						return ESqliteExtendedErrorCode::IOErr;
		case SQLITE_IOERR_READ:					return ESqliteExtendedErrorCode::IOErrRead;
		case SQLITE_IOERR_SHORT_READ:			return ESqliteExtendedErrorCode::IOErrShortRead;
		case SQLITE_IOERR_WRITE:				return ESqliteExtendedErrorCode::IOErrWrite;
		case SQLITE_IOERR_FSYNC:				return ESqliteExtendedErrorCode::IOErrFSync;
		case SQLITE_IOERR_DIR_FSYNC:			return ESqliteExtendedErrorCode::IOErrDirFSync;
		case SQLITE_IOERR_TRUNCATE:				return ESqliteExtendedErrorCode::IOErrTruncate;
		case SQLITE_IOERR_FSTAT:				return ESqliteExtendedErrorCode::IOErrFStat;
		case SQLITE_IOERR_UNLOCK:				return ESqliteExtendedErrorCode::IOErrUnlock;
		case SQLITE_IOERR_RDLOCK:				return ESqliteExtendedErrorCode::IOErrRDLock;
		case SQLITE_IOERR_DELETE:				return ESqliteExtendedErrorCode::IOErrDelete;
		case SQLITE_IOERR_BLOCKED:				return ESqliteExtendedErrorCode::IOErrBlocked;
		case SQLITE_IOERR_NOMEM:				return ESqliteExtendedErrorCode::IOErrNoMem;
		case SQLITE_IOERR_ACCESS:				return ESqliteExtendedErrorCode::IOErrAccess;
		case SQLITE_IOERR_CHECKRESERVEDLOCK:	return ESqliteExtendedErrorCode::IOErrCheckReservedLock;
		case SQLITE_IOERR_LOCK:					return ESqliteExtendedErrorCode::IOErrLock;
		case SQLITE_IOERR_CLOSE:				return ESqliteExtendedErrorCode::IOErrClose;
		case SQLITE_IOERR_DIR_CLOSE:			return ESqliteExtendedErrorCode::IOErrDirClose;
		case SQLITE_IOERR_SHMOPEN:				return ESqliteExtendedErrorCode::IOErrShmOpen;
		case SQLITE_IOERR_SHMSIZE:				return ESqliteExtendedErrorCode::IOErrShmSize;
		case SQLITE_IOERR_SHMLOCK:				return ESqliteExtendedErrorCode::IOErrShmLock;
		case SQLITE_IOERR_SHMMAP:				return ESqliteExtendedErrorCode::IOErrShmMap;
		case SQLITE_IOERR_SEEK:					return ESqliteExtendedErrorCode::IOErrSeek;
		case SQLITE_IOERR_DELETE_NOENT:			return ESqliteExtendedErrorCode::IOErrDeleteNoEnt;
		case SQLITE_IOERR_MMAP:					return ESqliteExtendedErrorCode::IOErrMMap;
		case SQLITE_IOERR_GETTEMPPATH:			return ESqliteExtendedErrorCode::IOErrGetTempPath;
		case SQLITE_IOERR_CONVPATH:				return ESqliteExtendedErrorCode::IOErrConvPath;
		case SQLITE_IOERR_VNODE:				return ESqliteExtendedErrorCode::IOErrVNode;
		case SQLITE_IOERR_AUTH:					return ESqliteExtendedErrorCode::IOErrAuth;
		case SQLITE_IOERR_BEGIN_ATOMIC:			return ESqliteExtendedErrorCode::IOErrBeginAtomic;
		case SQLITE_IOERR_COMMIT_ATOMIC:		return ESqliteExtendedErrorCode::IOErrCommitAtomic;
		case SQLITE_IOERR_ROLLBACK_ATOMIC:		return ESqliteExtendedErrorCode::IOErrRollbackAtomic;
		case SQLITE_IOERR_DATA:					return ESqliteExtendedErrorCode::IOErrData;
		case SQLITE_IOERR_CORRUPTFS:			return ESqliteExtendedErrorCode::IOErrCorruptFS;

		case SQLITE_CORRUPT:					return ESqliteExtendedErrorCode::Corrupt;
		case SQLITE_CORRUPT_VTAB:				return ESqliteExtendedErrorCode::CorruptVTab;
		case SQLITE_CORRUPT_SEQUENCE:			return ESqliteExtendedErrorCode::CorruptSequence;
		case SQLITE_CORRUPT_INDEX:				return ESqliteExtendedErrorCode::CorruptIndex;

		case SQLITE_NOTFOUND:					return ESqliteExtendedErrorCode::NotFound;

		case SQLITE_FULL:						return ESqliteExtendedErrorCode::Full;

		case SQLITE_CANTOPEN:					return ESqliteExtendedErrorCode::CantOpen;
		case SQLITE_CANTOPEN_NOTEMPDIR:			return ESqliteExtendedErrorCode::CantOpenNoTempDir;
		case SQLITE_CANTOPEN_ISDIR:				return ESqliteExtendedErrorCode::CantOpenIsDir;
		case SQLITE_CANTOPEN_FULLPATH:			return ESqliteExtendedErrorCode::CantOpenFullPath;
		case SQLITE_CANTOPEN_CONVPATH:			return ESqliteExtendedErrorCode::CantOpen_ConvPath;
		case SQLITE_CANTOPEN_DIRTYWAL:			return ESqliteExtendedErrorCode::CantOpenDirtyWal;
		case SQLITE_CANTOPEN_SYMLINK:			return ESqliteExtendedErrorCode::CantOpenSymLink;

		case SQLITE_PROTOCOL:					return ESqliteExtendedErrorCode::Protocol;

		case SQLITE_EMPTY:						return ESqliteExtendedErrorCode::Empty;

		case SQLITE_SCHEMA:						return ESqliteExtendedErrorCode::Schema;

		case SQLITE_TOOBIG:						return ESqliteExtendedErrorCode::TooBig;

		case SQLITE_CONSTRAINT:					return ESqliteExtendedErrorCode::Constaint;
		case SQLITE_CONSTRAINT_CHECK:			return ESqliteExtendedErrorCode::ConstraintCheck;
		case SQLITE_CONSTRAINT_COMMITHOOK:		return ESqliteExtendedErrorCode::ConstraintCommitHook;
		case SQLITE_CONSTRAINT_FOREIGNKEY:		return ESqliteExtendedErrorCode::ConstraintForeignKey;
		case SQLITE_CONSTRAINT_FUNCTION:		return ESqliteExtendedErrorCode::ConstraintFunction;
		case SQLITE_CONSTRAINT_NOTNULL:			return ESqliteExtendedErrorCode::ConstraintNotNull;
		case SQLITE_CONSTRAINT_PRIMARYKEY:		return ESqliteExtendedErrorCode::ConstraintPrimaryKey;
		case SQLITE_CONSTRAINT_TRIGGER:			return ESqliteExtendedErrorCode::ConstraintTrigger;
		case SQLITE_CONSTRAINT_UNIQUE:			return ESqliteExtendedErrorCode::ConstraintUnique;
		case SQLITE_CONSTRAINT_VTAB:			return ESqliteExtendedErrorCode::ConstraintVTab;
		case SQLITE_CONSTRAINT_ROWID:			return ESqliteExtendedErrorCode::ConstraintRowId;
		case SQLITE_CONSTRAINT_PINNED:			return ESqliteExtendedErrorCode::ConstraintPinned;
		case SQLITE_CONSTRAINT_DATATYPE:		return ESqliteExtendedErrorCode::ConstraintDataType;

		case SQLITE_MISMATCH:					return ESqliteExtendedErrorCode::Mismatch;

		case SQLITE_MISUSE:						return ESqliteExtendedErrorCode::Misuse;

		case SQLITE_NOLFS:						return ESqliteExtendedErrorCode::NoLFS;

		case SQLITE_AUTH:						return ESqliteExtendedErrorCode::Auth;
		case SQLITE_AUTH_USER:					return ESqliteExtendedErrorCode::AuthUser;

		case SQLITE_FORMAT:						return ESqliteExtendedErrorCode::Format;

		case SQLITE_RANGE:						return ESqliteExtendedErrorCode::Range;

		case SQLITE_NOTADB:						return ESqliteExtendedErrorCode::NotADB;

		case SQLITE_NOTICE:						return ESqliteExtendedErrorCode::Notice;
		case SQLITE_NOTICE_RECOVER_WAL:			return ESqliteExtendedErrorCode::NoticeRecoverWal;
		case SQLITE_NOTICE_RECOVER_ROLLBACK:	return ESqliteExtendedErrorCode::NoticeRecoverRollback;
		case SQLITE_NOTICE_RBU:					return ESqliteExtendedErrorCode::NoticeRBU;

		case SQLITE_WARNING:					return ESqliteExtendedErrorCode::Warning;
		case SQLITE_WARNING_AUTOINDEX:			return ESqliteExtendedErrorCode::WarningAutoIndex;

		case SQLITE_ROW:						return ESqliteExtendedErrorCode::Row;

		case SQLITE_DONE:						return ESqliteExtendedErrorCode::Done;

		default:
			UE_LOG( LogSqlite, Error, TEXT( "Unexpected Sqlite native error code: %d" ), ErrorCode );
			return ESqliteExtendedErrorCode::Error;
	}

}

int USqliteStatics::UnmapNativeExtendedErrorCode( ESqliteExtendedErrorCode ErrorCode )
{
	switch( ErrorCode )
	{
		case ESqliteExtendedErrorCode::Ok:						return SQLITE_OK;
		case ESqliteExtendedErrorCode::OkLoadPermanently:		return SQLITE_OK_LOAD_PERMANENTLY;

		case ESqliteExtendedErrorCode::ErrorMissingCollSeq:		return SQLITE_ERROR_MISSING_COLLSEQ;
		case ESqliteExtendedErrorCode::ErrorRetry:				return SQLITE_ERROR_RETRY;
		case ESqliteExtendedErrorCode::ErrorSnapshot:			return SQLITE_ERROR_SNAPSHOT;

		case ESqliteExtendedErrorCode::Internal:				return SQLITE_INTERNAL;

		case ESqliteExtendedErrorCode::Perm:					return SQLITE_PERM;

		case ESqliteExtendedErrorCode::Abort:					return SQLITE_ABORT;
		case ESqliteExtendedErrorCode::AbortRollback:			return SQLITE_ABORT_ROLLBACK;

		case ESqliteExtendedErrorCode::Busy:					return SQLITE_BUSY;
		case ESqliteExtendedErrorCode::BusyRecovery:			return SQLITE_BUSY_RECOVERY;
		case ESqliteExtendedErrorCode::BusySnapshot:			return SQLITE_BUSY_SNAPSHOT;
		case ESqliteExtendedErrorCode::BusyTimeOut:				return SQLITE_BUSY_TIMEOUT;

		case ESqliteExtendedErrorCode::Locked:					return SQLITE_LOCKED;
		case ESqliteExtendedErrorCode::LockedSharedCache:		return SQLITE_LOCKED_SHAREDCACHE;
		case ESqliteExtendedErrorCode::LockedVTab:				return SQLITE_LOCKED_VTAB;

		case ESqliteExtendedErrorCode::NoMem:					return SQLITE_NOMEM;

		case ESqliteExtendedErrorCode::ReadOnly:				return SQLITE_READONLY;
		case ESqliteExtendedErrorCode::ReadOnlyRecovery:		return SQLITE_READONLY_RECOVERY;
		case ESqliteExtendedErrorCode::ReadOnlyCantLock:		return SQLITE_READONLY_CANTLOCK;
		case ESqliteExtendedErrorCode::ReadOnlyRollback:		return SQLITE_READONLY_ROLLBACK;
		case ESqliteExtendedErrorCode::ReadOnlyDBMoved:			return SQLITE_READONLY_DBMOVED;
		case ESqliteExtendedErrorCode::ReadOnlyCantInit:		return SQLITE_READONLY_CANTINIT;
		case ESqliteExtendedErrorCode::ReadOnlyDirectory:		return SQLITE_READONLY_DIRECTORY;

		case ESqliteExtendedErrorCode::Interrupt:				return SQLITE_INTERRUPT;

		case ESqliteExtendedErrorCode::IOErr:					return SQLITE_IOERR;
		case ESqliteExtendedErrorCode::IOErrRead:				return SQLITE_IOERR_READ;
		case ESqliteExtendedErrorCode::IOErrShortRead:			return SQLITE_IOERR_SHORT_READ;
		case ESqliteExtendedErrorCode::IOErrWrite:				return SQLITE_IOERR_WRITE;
		case ESqliteExtendedErrorCode::IOErrFSync:				return SQLITE_IOERR_FSYNC;
		case ESqliteExtendedErrorCode::IOErrDirFSync:			return SQLITE_IOERR_DIR_FSYNC;
		case ESqliteExtendedErrorCode::IOErrTruncate:			return SQLITE_IOERR_TRUNCATE;
		case ESqliteExtendedErrorCode::IOErrFStat:				return SQLITE_IOERR_FSTAT;
		case ESqliteExtendedErrorCode::IOErrUnlock:				return SQLITE_IOERR_UNLOCK;
		case ESqliteExtendedErrorCode::IOErrRDLock:				return SQLITE_IOERR_RDLOCK;
		case ESqliteExtendedErrorCode::IOErrDelete:				return SQLITE_IOERR_DELETE;
		case ESqliteExtendedErrorCode::IOErrBlocked:			return SQLITE_IOERR_BLOCKED;
		case ESqliteExtendedErrorCode::IOErrNoMem:				return SQLITE_IOERR_NOMEM;
		case ESqliteExtendedErrorCode::IOErrAccess:				return SQLITE_IOERR_ACCESS;
		case ESqliteExtendedErrorCode::IOErrCheckReservedLock:	return SQLITE_IOERR_CHECKRESERVEDLOCK;
		case ESqliteExtendedErrorCode::IOErrLock:				return SQLITE_IOERR_LOCK;
		case ESqliteExtendedErrorCode::IOErrClose:				return SQLITE_IOERR_CLOSE;
		case ESqliteExtendedErrorCode::IOErrDirClose:			return SQLITE_IOERR_DIR_CLOSE;
		case ESqliteExtendedErrorCode::IOErrShmOpen:			return SQLITE_IOERR_SHMOPEN;
		case ESqliteExtendedErrorCode::IOErrShmSize:			return SQLITE_IOERR_SHMSIZE;
		case ESqliteExtendedErrorCode::IOErrShmLock:			return SQLITE_IOERR_SHMLOCK;
		case ESqliteExtendedErrorCode::IOErrShmMap:				return SQLITE_IOERR_SHMMAP;
		case ESqliteExtendedErrorCode::IOErrSeek:				return SQLITE_IOERR_SEEK;
		case ESqliteExtendedErrorCode::IOErrDeleteNoEnt:		return SQLITE_IOERR_DELETE_NOENT;
		case ESqliteExtendedErrorCode::IOErrMMap:				return SQLITE_IOERR_MMAP;
		case ESqliteExtendedErrorCode::IOErrGetTempPath:		return SQLITE_IOERR_GETTEMPPATH;
		case ESqliteExtendedErrorCode::IOErrConvPath:			return SQLITE_IOERR_CONVPATH;
		case ESqliteExtendedErrorCode::IOErrVNode:				return SQLITE_IOERR_VNODE;
		case ESqliteExtendedErrorCode::IOErrAuth:				return SQLITE_IOERR_AUTH;
		case ESqliteExtendedErrorCode::IOErrBeginAtomic:		return SQLITE_IOERR_BEGIN_ATOMIC;
		case ESqliteExtendedErrorCode::IOErrCommitAtomic:		return SQLITE_IOERR_COMMIT_ATOMIC;
		case ESqliteExtendedErrorCode::IOErrRollbackAtomic:		return SQLITE_IOERR_ROLLBACK_ATOMIC;
		case ESqliteExtendedErrorCode::IOErrData:				return SQLITE_IOERR_DATA;
		case ESqliteExtendedErrorCode::IOErrCorruptFS:			return SQLITE_IOERR_CORRUPTFS;

		case ESqliteExtendedErrorCode::Corrupt:					return SQLITE_CORRUPT;
		case ESqliteExtendedErrorCode::CorruptVTab:				return SQLITE_CORRUPT_VTAB;
		case ESqliteExtendedErrorCode::CorruptSequence:			return SQLITE_CORRUPT_SEQUENCE;
		case ESqliteExtendedErrorCode::CorruptIndex:			return SQLITE_CORRUPT_INDEX;

		case ESqliteExtendedErrorCode::NotFound:				return SQLITE_NOTFOUND;

		case ESqliteExtendedErrorCode::Full:					return SQLITE_FULL;

		case ESqliteExtendedErrorCode::CantOpen:				return SQLITE_CANTOPEN;
		case ESqliteExtendedErrorCode::CantOpenNoTempDir:		return SQLITE_CANTOPEN_NOTEMPDIR;
		case ESqliteExtendedErrorCode::CantOpenIsDir:			return SQLITE_CANTOPEN_ISDIR;
		case ESqliteExtendedErrorCode::CantOpenFullPath:		return SQLITE_CANTOPEN_FULLPATH;
		case ESqliteExtendedErrorCode::CantOpen_ConvPath:		return SQLITE_CANTOPEN_CONVPATH;
		case ESqliteExtendedErrorCode::CantOpenDirtyWal:		return SQLITE_CANTOPEN_DIRTYWAL;
		case ESqliteExtendedErrorCode::CantOpenSymLink:			return SQLITE_CANTOPEN_SYMLINK;

		case ESqliteExtendedErrorCode::Protocol:				return SQLITE_PROTOCOL;

		case ESqliteExtendedErrorCode::Empty:					return SQLITE_EMPTY;

		case ESqliteExtendedErrorCode::Schema:					return SQLITE_SCHEMA;

		case ESqliteExtendedErrorCode::TooBig:					return SQLITE_TOOBIG;

		case ESqliteExtendedErrorCode::Constaint:				return SQLITE_CONSTRAINT;
		case ESqliteExtendedErrorCode::ConstraintCheck:			return SQLITE_CONSTRAINT_CHECK;
		case ESqliteExtendedErrorCode::ConstraintCommitHook:	return SQLITE_CONSTRAINT_COMMITHOOK;
		case ESqliteExtendedErrorCode::ConstraintForeignKey:	return SQLITE_CONSTRAINT_FOREIGNKEY;
		case ESqliteExtendedErrorCode::ConstraintFunction:		return SQLITE_CONSTRAINT_FUNCTION;
		case ESqliteExtendedErrorCode::ConstraintNotNull:		return SQLITE_CONSTRAINT_NOTNULL;
		case ESqliteExtendedErrorCode::ConstraintPrimaryKey:	return SQLITE_CONSTRAINT_PRIMARYKEY;
		case ESqliteExtendedErrorCode::ConstraintTrigger:		return SQLITE_CONSTRAINT_TRIGGER;
		case ESqliteExtendedErrorCode::ConstraintUnique:		return SQLITE_CONSTRAINT_UNIQUE;
		case ESqliteExtendedErrorCode::ConstraintVTab:			return SQLITE_CONSTRAINT_VTAB;
		case ESqliteExtendedErrorCode::ConstraintRowId:			return SQLITE_CONSTRAINT_ROWID;
		case ESqliteExtendedErrorCode::ConstraintPinned:		return SQLITE_CONSTRAINT_PINNED;
		case ESqliteExtendedErrorCode::ConstraintDataType:		return SQLITE_CONSTRAINT_DATATYPE;

		case ESqliteExtendedErrorCode::Mismatch:				return SQLITE_MISMATCH;

		case ESqliteExtendedErrorCode::Misuse:					return SQLITE_MISUSE;

		case ESqliteExtendedErrorCode::NoLFS:					return SQLITE_NOLFS;

		case ESqliteExtendedErrorCode::Auth:					return SQLITE_AUTH;
		case ESqliteExtendedErrorCode::AuthUser:				return SQLITE_AUTH_USER;

		case ESqliteExtendedErrorCode::Format:					return SQLITE_FORMAT;

		case ESqliteExtendedErrorCode::Range:					return SQLITE_RANGE;

		case ESqliteExtendedErrorCode::NotADB:					return SQLITE_NOTADB;

		case ESqliteExtendedErrorCode::Notice:					return SQLITE_NOTICE;
		case ESqliteExtendedErrorCode::NoticeRecoverWal:		return SQLITE_NOTICE_RECOVER_WAL;
		case ESqliteExtendedErrorCode::NoticeRecoverRollback:	return SQLITE_NOTICE_RECOVER_ROLLBACK;
		case ESqliteExtendedErrorCode::NoticeRBU:				return SQLITE_NOTICE_RBU;

		case ESqliteExtendedErrorCode::Warning:					return SQLITE_WARNING;
		case ESqliteExtendedErrorCode::WarningAutoIndex:		return SQLITE_WARNING_AUTOINDEX;

		case ESqliteExtendedErrorCode::Row:						return SQLITE_ROW;

		case ESqliteExtendedErrorCode::Done:					return SQLITE_DONE;

		default:
			UE_LOG( LogSqlite, Error, TEXT( "Unexpected ESqliteExtendedErrorCode value: '%s'" ), *UEnum::GetValueAsString( ErrorCode ) );
			return SQLITE_ERROR;
	}
}

bool USqliteStatics::CheckSqliteExtendedErrorCode( int NativeErrorCode, ESqliteExtendedErrorCode ErrorCode )
{
	return NativeErrorCode == UnmapNativeExtendedErrorCode( ErrorCode );
}

void USqliteStatics::BranchIfSqliteExtendedErrorCode( int NativeErrorCode, ESqliteExtendedErrorCode ErrorCode, ESqliteDatabaseEqualityExecutionPins& Branch )
{
	const bool rc =  CheckSqliteExtendedErrorCode( NativeErrorCode, ErrorCode );
	Branch = rc ? ESqliteDatabaseEqualityExecutionPins::Equal : ESqliteDatabaseEqualityExecutionPins::NotEqual;
}

// ============================================================================
// === 
// ============================================================================

FString USqliteStatics::NativeErrorString( int ErrorCode )
{
	return FString( sqlite3_errstr( ErrorCode ) );
}

FString USqliteStatics::ErrorString( ESqliteErrorCode ErrorCode )
{
	return FString( sqlite3_errstr( USqliteStatics::UnmapNativeErrorCode( ErrorCode ) ) );
}

FString USqliteStatics::ExtendedErrorString( ESqliteExtendedErrorCode ErrorCode )
{
	return FString( sqlite3_errstr( USqliteStatics::UnmapNativeExtendedErrorCode( ErrorCode ) ) );
}
