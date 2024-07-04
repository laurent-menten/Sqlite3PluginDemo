// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SqliteDatabaseInfo.generated.h"

class USqliteDatabase;

// ============================================================================
// === SqliteDatabaseInfos ====================================================
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteDatabaseOpenMode : uint8
{
	/**
	 * The database is opened for reading only. The database must already
	 * exist.
	 * (SQLITE_OPEN_READONLY)
	 */
	READ_ONLY			UMETA( DisplayName = "Read-only" ),

	/**
	 * The database is opened for reading and writing if possible, or reading
	 * only if the file is write protected by the operating system. In either
	 * case the database must already exist.
	 * (SQLITE_OPEN_READWRITE)
	 */
	READ_WRITE			UMETA( DisplayName = "Read-write" ),

	/**
	 * The database is opened for reading and writing, and is created if it
	 * does not already exist.
	 * (SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)
	 */
	READ_WRITE_CREATE	UMETA( DisplayName = "Read-write & create" ),	// default
};

UENUM( BlueprintType )
enum class ESqliteDatabaseThreadingMode : uint8
{
	/**
	 * The database connection will use the "multi-thread" threading mode. Each thread
	 * must be using a different database connection.
	 * (SQLITE_OPEN_NOMUTEX)
	 */
	NO_MUTEX		UMETA( DisplayName = "No mutex" ),

	/**
	 * The database connection will use the "serialized" threading mode. Multiple thread
	 * can safely attempt to use the same database connection.
	 * (SQLITE_OPEN_FULLMUTEX)
	 */
	FULL_MUTEX		UMETA( DisplayName = "Full mutex" ),

	UNSET			UMETA( DisplayName = "Default" ),
};

UENUM( BlueprintType )
enum class ESqliteDatabaseCacheMode : uint8
{
	/**
	 * The database is opened with shared cache enabled.
	 * (SQLITE_OPEN_SHAREDCACHE)
	 */
	SHARED_CACHE	UMETA( DisplayName = "Shared cache" ),

	/**
	 * The database is opened with shared cache disabled.
	 * (SQLITE_OPEN_PRIVATECACHE)
	 */
	PRIVATE_CACHE	UMETA( DisplayName = "Private cache" ),

	UNSET			UMETA( DisplayName = "Default" ),
};

// ============================================================================
// === 
// ============================================================================

/**
 *
 */
USTRUCT()
struct FDatabaseAttachmentInfo
{
	GENERATED_USTRUCT_BODY()

	/**
	 * The attachment database filename.
	 */
	UPROPERTY(EditAnywhere)
	FString FileName;

	/**
	 * The schema name for this attachment.
	 */
	UPROPERTY(EditAnywhere)
	FName SchemaName;

	/**
	 * The user_version for this attachment.
	 */
	UPROPERTY(EditAnywhere)
	int32 UserVersion = 1;
};

// ============================================================================
// === 
// ============================================================================

/**
 * 
 */
UCLASS( BlueprintType, Blueprintable, ClassGroup = Sqlite3, Category = "Sqlite3 Database" )
class SQLITE3_API USqliteDatabaseInfo : public UDataAsset
{
	GENERATED_BODY()

	friend class USqliteDatabaseInfoValidator;
	
public:
	USqliteDatabaseInfo( const FObjectInitializer& ObjectInitializer );

	virtual ~USqliteDatabaseInfo() override;

private:
	bool bIsValidated;
	
public:
	/**
	 * A C++ or blueprint class deriving from USqliteDatabase to be used to
	 * handle the database connection.
	 */
	UPROPERTY(EditAnywhere)
	TSubclassOf<USqliteDatabase> DatabaseClass;

	/**
	 * The database filename.
	 * • An empty string will open a private temporary on-disk database.
	 * • :memory: will open a private in-memory temporary database.
	 * • If OpenAsURI is set, filename is interpreted as a RFC3986 compliant URI.
	 */
	UPROPERTY(EditAnywhere)
	FString DatabaseFileName;

	/**
	 * How to open the database.
	 * (SQLITE_OPEN_READONLY, SQLITE_OPEN_READWRITE, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)
	 */
	UPROPERTY(EditAnywhere)
	ESqliteDatabaseOpenMode OpenMode = ESqliteDatabaseOpenMode::READ_WRITE_CREATE;

	// ---------------------------------------------------------------------------

	/**
	 * 
	 */
	UPROPERTY(EditAnywhere)
	TArray<FDatabaseAttachmentInfo> Attachments;

	// ---------------------------------------------------------------------------

	/**
	 * 
	 */
	UPROPERTY(EditAnywhere)
	TMap<FString,FString> StoredStatements;

	// ---------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Schema")
	FName SchemaName = FName( "main" );

	/**
	 * Create the StoredStatement table when creating the database.
	 */
	UPROPERTY(EditAnywhere, Category="Schema|Default tables")
	bool bCreateStoredStatementsTable = false;

	/**
	 * Create the Properties table when creating the database.
	 */
	UPROPERTY(EditAnywhere, Category="Schema|Default tables")
	bool bCreatePropertiesTable = false;

	/**
	 * Create the ActorsStore table when creating the database.
	 */
	UPROPERTY(EditAnywhere, Category="Schema|Default tables")
	bool bCreateActorsStoreTable = false;

	/**
	 * Create the Log table when creating the database.
	 */
	UPROPERTY(EditAnywhere, Category="Schema|Default tables")
	bool bCreateLogTable = false;

	// ---------------------------------------------------------------------------

	/**
	 * Override the default %PlatformUserDir%/%ProjectName%/ directory where
	 * the database and its attachments if any should be located.
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	FDirectoryPath DatabaseDirectoryOverride;

	/**
	 * The filename can be interpreted as a URI.
	 * (SQLITE_OPEN_URI)
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bOpenAsURI = false;

	/**
	 * The database will be opened as an in-memory database.
	 * (SQLITE_OPEN_MEMORY)
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bInMemory = false;

	/**
	 * Set the threading mode for multiple connections to a single database.
	 * (SQLITE_OPEN_NOMUTEX, SQLITE_OPEN_FULLMUTEX)
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	ESqliteDatabaseThreadingMode ThreadingMode = ESqliteDatabaseThreadingMode::UNSET;

	/**
	 * Set the caching mode.
	 * (SQLITE_OPEN_SHAREDCACHE, SQLITE_OPEN_PRIVATECACHE)
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	ESqliteDatabaseCacheMode CacheMode = ESqliteDatabaseCacheMode::UNSET;

	/**
	 * The database connection comes up in "extended result code mode".
	 * (SQLITE_OPEN_EXRESCODE)
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bUseExtendedResultCode = false;

	/**
	 * The database filename is not allowed to contain a symbolic link.
	 * (SQLITE_OPEN_NOFOLLOW)
	 */
	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bNoFollow = false;

	// ---------------------------------------------------------------------------

	/**
	 * The application_id value for the project. This value is set when the database is
	 * created and will cause the open operation to fail if it is changed.
	 */
	UPROPERTY(EditAnywhere, Category="Version")
	int32 ApplicationId = 1;

	/**
	 * The user_version value for the database. A change to this value will trigger
	 * an OnUpdate event on database open.
	 */
	UPROPERTY(EditAnywhere, Category = "Version")
	int32 UserVersion = 1;

	// ---------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDeleteFileBeforeOpen = false;

	// ===========================================================================
	// =
	// ===========================================================================

	/** Information for thumbnail rendering */
	UPROPERTY( Instanced )
	TObjectPtr<class UThumbnailInfo> ThumbnailInfo;
};
