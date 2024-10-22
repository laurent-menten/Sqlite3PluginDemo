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
// === Table definition ======================================================= 
// ============================================================================

UENUM( BlueprintType )
enum class ESqliteDatabaseColumnAffinity : uint8
{
	TYPE_INTEGER	UMETA( DisplayName = "Integer" ), 
	TYPE_REAL		UMETA( DisplayName = "Real" ),
	TYPE_NUMERIC	UMETA( DisplayName = "Numeric" ),
	TYPE_TEXT		UMETA( DisplayName = "Text" ),
	TYPE_BLOB		UMETA( DisplayName = "Blob" ),

	UNSET			UMETA( DisplayName = "None" )
};

USTRUCT()
struct FDatabaseColumn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString ColumnName;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	ESqliteDatabaseColumnAffinity ColumnAffinity = ESqliteDatabaseColumnAffinity::UNSET;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bPrimaryKey = false;
	
	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bUnique = false;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	bool bNotNull = false;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString CheckConstraint;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString ExtraColumnConstraint;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	FString DefaultValue;
};

// ----------------------------------------------------------------------------

USTRUCT()
struct FDatabaseTable
{
	GENERATED_BODY()

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

	UPROPERTY(EditAnywhere, Category="Sqlite3", meta = (TitleProperty = "ColumnName") )
	TArray<FDatabaseColumn> Columns;

	UPROPERTY(EditAnywhere, Category="Sqlite3")
	TArray<FString> ExtraTableConstraints;	
};

// ============================================================================
// === 
// ============================================================================

/**
 * DefaultToInstanced, 
 */
UCLASS( BlueprintType, Blueprintable, EditInlineNew, ClassGroup = Sqlite3, Category = "Sqlite3 Database" )
class SQLITE3_API USqliteDatabaseInfo : public UDataAsset
{
	GENERATED_BODY()

	friend class USqliteDatabaseInfoValidator;
	friend class FSqliteDatabaseInfoTypeActions;
	friend class ISqlite3Editor;

	friend class USqliteDatabase;
	friend class USqliteStatics;
	
public:
	USqliteDatabaseInfo( const FObjectInitializer& ObjectInitializer );

	virtual ~USqliteDatabaseInfo() override;

	virtual void PreSave( FObjectPreSaveContext SaveContext ) override;

private:
	// ------------------------------------------------------------------------
	// - Runtime utils --------------------------------------------------------
	// ------------------------------------------------------------------------

	/**
	 * (Internal - Runtime)
	 * The USqliteDatabase instance associated with this USqliteDatabaseInfo.
	 */
	UPROPERTY()
	USqliteDatabase* DatabaseInstance = nullptr;

	/**
	 * (Internal - Runtime)
	 * How many times this USqliteDatabaseInfo was used to create a USqliteDatabase object.
	 */
	UPROPERTY()
	int DatabaseOpenCount = 0;

	// ------------------------------------------------------------------------
	// - Editor utils ---------------------------------------------------------
	// ------------------------------------------------------------------------
	
	/**
	 * (Internal - Editor)
	 * Was this asset successfully generated
	 */
	UPROPERTY()
	bool bCreateDatabaseSqlCommandsGenerated = false;

	/**
	 * (Internal - Editor)
	 * Information for thumbnail rendering
	 * */
	UPROPERTY( Instanced )
	TObjectPtr<class UThumbnailInfo> ThumbnailInfo;

	// ========================================================================
	// = Data =================================================================
	// ========================================================================
	
public:
	/**
	 * A C++ or blueprint class deriving from USqliteDatabase to be used to
	 * handle the database connection.
	 */
	UPROPERTY( EditAnywhere, Category="Database" )
	TSubclassOf<USqliteDatabase> DatabaseClass; // = USqliteDatabase::StaticClass();

	/**
	 * The database filename.
	 * • An empty string will open a private temporary on-disk database.
	 * • :memory: will open a private temporary in-memory database.
	 * • If OpenAsURI (advanced option) is set, filename will be interpreted as a RFC3986 compliant URI.
	 */
	UPROPERTY( EditAnywhere, Category="Database" )
	FString DatabaseFileName = FString( "" );

	// ---------------------------------------------------------------------------

	/**
	 * The application_id value for the project. This value is set when the database is
	 * created and will cause the open operation to fail if it is changed.
	 * A value of zero is forbidden (sqlite default value that will trigger OnCreate event).
	 */
	UPROPERTY( EditAnywhere, Category="Database" )
	int32 ApplicationId = 1;

	/**
	 * The user_version value for the database. A change to this value will trigger
	 * an OnUpdate event on database open.
	 */
	UPROPERTY( EditAnywhere, Category = "Database" )
	int32 UserVersion = 1;

	// ---------------------------------------------------------------------------

	/**
	 * How to open the database.
	 * (SQLITE_OPEN_READONLY, SQLITE_OPEN_READWRITE, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)
	 */
	UPROPERTY( EditAnywhere, Category="Database|Advanced" )
	ESqliteDatabaseOpenMode OpenMode = ESqliteDatabaseOpenMode::READ_WRITE_CREATE;
	
	/**
	 * Override the default %PlatformUserDir%/%ProjectName%/ directory where
	 * the database and its attachments if any should be located.
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	FDirectoryPath DatabaseDirectoryOverride;

	/**
	 * The filename can be interpreted as a URI.
	 * (SQLITE_OPEN_URI)
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	bool bOpenAsURI = false;

	/**
	 * The database will be opened as an in-memory database.
	 * (SQLITE_OPEN_MEMORY)
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	bool bInMemory = false;

	/**
	 * Set the threading mode for multiple connections to a single database.
	 * (SQLITE_OPEN_NOMUTEX, SQLITE_OPEN_FULLMUTEX)
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	ESqliteDatabaseThreadingMode ThreadingMode = ESqliteDatabaseThreadingMode::UNSET;

	/**
	 * Set the caching mode.
	 * (SQLITE_OPEN_SHAREDCACHE, SQLITE_OPEN_PRIVATECACHE)
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	ESqliteDatabaseCacheMode CacheMode = ESqliteDatabaseCacheMode::UNSET;

	/**
	 * The database connection comes up in "extended result code mode".
	 * (SQLITE_OPEN_EXRESCODE)
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	bool bUseExtendedResultCode = false;

	/**
	 * The database filename is not allowed to contain a symbolic link.
	 * (SQLITE_OPEN_NOFOLLOW)
	 */
	UPROPERTY( EditAnywhere, Category = "Database|Advanced" )
	bool bNoFollow = false;

	// ---------------------------------------------------------------------------

	/**
	 * Create the Properties table when creating the database.
	 */
	UPROPERTY( EditAnywhere, Category="Schema" )
	bool bCreatePropertiesTable = true;
	
	/**
	 * Tables to create in database.
	 */
	UPROPERTY( EditAnywhere, Category = "Schema", meta = (TitleProperty = "TableName") )
	TArray<FDatabaseTable> CustomTables;

	/**
	 * Extra SQL commands. 
	 */
	UPROPERTY( EditAnywhere, Category = "Schema" )
	TArray<FString> ExtraSqlCommands;

	// ===========================================================================
	// = Debug ===================================================================
	// ===========================================================================

	/**
	 * Delete database file if it exists before trying to open it.
	 */
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDeleteFileBeforeOpen = false;

	/**
	 * (Internal - Visible but Read-only)
	 * The sql requests that were generated from custom table descriptions in the asset.
	 * Updated during PreSave because asset validation is done after save.
	 */
	UPROPERTY( VisibleDefaultsOnly, Category="Debug" )
	TMap<FString,FString> GeneratedDatabaseSqlCommands;
};
