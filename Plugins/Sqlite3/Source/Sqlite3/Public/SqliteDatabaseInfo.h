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
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString FileName;

	/**
	 * The schema name for this attachment.
	 */
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FName SchemaName;

	/**
	 * The user_version for this attachment.
	 */
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	int32 UserVersion = 1;
};

// ============================================================================
// === 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteDatabaseColumnType : uint8
{
	Null,
	Integer, 
	Real,
	Text,
	Blob,

	Unspecified UMETA( DisplayName = "--" )
};

USTRUCT()
struct FDatabaseColumn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString ColumnName;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	ESqliteDatabaseColumnType ColumnType = ESqliteDatabaseColumnType::Unspecified;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bPrimaryKey = false;
	
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bUnique = false;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bNotNull = false;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString ExtraConstraint;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString DefaultValue;
};

// ----------------------------------------------------------------------------

USTRUCT()
struct FDatabaseTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString SchemaName;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString TableName;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bTemporary = false;
	
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bIfNotExists = false;
	
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bWithoutRowId = false;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bStrict = false;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	TArray<FDatabaseColumn> Columns;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	TArray<FString> ExtraConstraints;	
};

// ============================================================================
// === 
// ============================================================================

/**
 * 
 */
UCLASS( BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew, ClassGroup = Sqlite3, Category = "Sqlite3 Database" )
class SQLITE3_API USqliteDatabaseInfo : public UDataAsset
{
	GENERATED_BODY()

	friend class USqliteDatabaseInfoValidator;
	friend class USqliteDatabase;
	friend class USqliteStatics;
	
public:
	USqliteDatabaseInfo( const FObjectInitializer& ObjectInitializer );

	virtual ~USqliteDatabaseInfo() override;

private:
	/**
	 * (Internal)
	 * The USqliteDatabase instance associated with this USqliteDatabaseInfo.
	 */
	UPROPERTY()
	USqliteDatabase* DatabaseInstance = nullptr;

	/**
	 * (Internal)
	 * How many times this USqliteDatabaseInfo was used to create a USqliteDatabase object.
	 */
	UPROPERTY()
	int DatabaseOpenCount = 0;

	/**
	 * (Internal)
	 * Was this asset successfully generated
	 */
	UPROPERTY()
	bool bIsValidated;
	
public:
	UFUNCTION( BlueprintCallable, CallInEditor, Category="Tools" )
	void Test();

	// ---------------------------------------------------------------------------

	/**
	 * A C++ or blueprint class deriving from USqliteDatabase to be used to
	 * handle the database connection.
	 */
	UPROPERTY(EditAnywhere, Category="Database")
	TSubclassOf<USqliteDatabase> DatabaseClass;

	/**
	 * The database filename.
	 * • An empty string will open a private temporary on-disk database.
	 * • :memory: will open a private in-memory temporary database.
	 * • If OpenAsURI is set, filename is interpreted as a RFC3986 compliant URI.
	 */
	UPROPERTY(EditAnywhere, Category="Database")
	FString DatabaseFileName;

	// ---------------------------------------------------------------------------

	/**
	 * The application_id value for the project. This value is set when the database is
	 * created and will cause the open operation to fail if it is changed.
	 */
	UPROPERTY(EditAnywhere, Category="Database")
	int32 ApplicationId = 1;

	/**
	 * The user_version value for the database. A change to this value will trigger
	 * an OnUpdate event on database open.
	 */
	UPROPERTY(EditAnywhere, Category = "Database")
	int32 UserVersion = 1;

	// ---------------------------------------------------------------------------

	/**
	 * How to open the database.
	 * (SQLITE_OPEN_READONLY, SQLITE_OPEN_READWRITE, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)
	 */
	UPROPERTY(EditAnywhere, Category="Database|Advanced")
	ESqliteDatabaseOpenMode OpenMode = ESqliteDatabaseOpenMode::READ_WRITE_CREATE;
	
	/**
	 * The Schema name for main database.
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	FName SchemaName = FName( "" );
	
	/**
	 * Override the default %PlatformUserDir%/%ProjectName%/ directory where
	 * the database and its attachments if any should be located.
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	FDirectoryPath DatabaseDirectoryOverride;

	/**
	 * The filename can be interpreted as a URI.
	 * (SQLITE_OPEN_URI)
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	bool bOpenAsURI = false;

	/**
	 * The database will be opened as an in-memory database.
	 * (SQLITE_OPEN_MEMORY)
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	bool bInMemory = false;

	/**
	 * Set the threading mode for multiple connections to a single database.
	 * (SQLITE_OPEN_NOMUTEX, SQLITE_OPEN_FULLMUTEX)
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	ESqliteDatabaseThreadingMode ThreadingMode = ESqliteDatabaseThreadingMode::UNSET;

	/**
	 * Set the caching mode.
	 * (SQLITE_OPEN_SHAREDCACHE, SQLITE_OPEN_PRIVATECACHE)
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	ESqliteDatabaseCacheMode CacheMode = ESqliteDatabaseCacheMode::UNSET;

	/**
	 * The database connection comes up in "extended result code mode".
	 * (SQLITE_OPEN_EXRESCODE)
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	bool bUseExtendedResultCode = false;

	/**
	 * The database filename is not allowed to contain a symbolic link.
	 * (SQLITE_OPEN_NOFOLLOW)
	 */
	UPROPERTY(EditAnywhere, Category = "Database|Advanced")
	bool bNoFollow = false;

	// ---------------------------------------------------------------------------

	/**
	 * 
	 */
	UPROPERTY(EditAnywhere, Category="Database|Attachmements")
	TArray<FDatabaseAttachmentInfo> Attachments;

	// ---------------------------------------------------------------------------

	/**
	 * 
	 */
//	UPROPERTY(EditAnywhere, Category="Database")
//	TMap<FString,FString> StoredStatements;

	// ---------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Schema|Custom tables")
	TArray<FDatabaseTable> CustomTables;

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

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDeleteFileBeforeOpen = false;

	/**
	 * (Internal)
	 * The sql requests that were generated from custom table descriptions in the asset.
	 */
	//	UPROPERTY( Instanced )
	UPROPERTY(EditAnywhere, Category="Debug" )
	//	UPROPERTY(BlueprintReadOnly, Category = "Debug", meta=(AllowPrivateAccess = "true"))
	TMap<FString,FString> GeneratedTableSqlCommands;
	

	// ===========================================================================
	// =
	// ===========================================================================

	/** Information for thumbnail rendering */
	UPROPERTY( Instanced )
	TObjectPtr<class UThumbnailInfo> ThumbnailInfo;

	// ===========================================================================
	// =
	// ===========================================================================

public:
};
