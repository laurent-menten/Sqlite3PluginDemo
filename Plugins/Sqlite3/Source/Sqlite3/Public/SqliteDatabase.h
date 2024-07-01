// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"

#include "sqlite/Sqlite3Include.h"

#include "SqliteDatabaseInfo.h"
#include "SqliteEnums.h" 
#include "SqliteStatement.h"
#include "SqliteDatabase.generated.h"

USTRUCT()
struct FSqliteDatabaseAttachment
{
	GENERATED_BODY()

	FString AttachmentFileName;
	FName AttachmentSchema;
};

/**
 * 
 */
UCLASS( Blueprintable, Abstract, Config=Engine )
class SQLITE3_API USqliteDatabase : public UObject
{
	GENERATED_BODY()

	friend class USqlite3Subsystem;
	friend class USqliteStatement;
	friend class USqliteStatics;

private:
	// ---------------------------------------------------------------------------
	// -
	// ---------------------------------------------------------------------------

	/**
	 * The DatabaseInfo asset.
	 */
	UPROPERTY()
	TObjectPtr<USqliteDatabaseInfo> DatabaseInfoAsset;

	/**
	 * The actual path of the database.
	 */
	FString DatabaseFilePath;

	/**
	 * The computed open flags.
	 */
	int OpenFlags;

	/**
	 * The SchemaName/DatabaseFilePath mapping for the attached databases.
	 */
	TMap<FName, FString> Attachments;

	// ---------------------------------------------------------------------------
	// - State -------------------------------------------------------------------
	// ---------------------------------------------------------------------------

	sqlite3* DatabaseConnectionHandler = nullptr;

	UPROPERTY()
	TArray<USqliteStatement*> ActiveStatements;

	void StatementFinalized( USqliteStatement* Statement );
	
	bool bIsInitialized = false;
	bool bIsOpen = false;

	int LastSqliteReturnCode = SQLITE_OK;

	// ---------------------------------------------------------------------------

	int StoredApplicationId;
	int StoredUserVersion;

	// ---------------------------------------------------------------------------

	static const FString Sql_BeginTransaction;
	static const FString Sql_Commit;
	static const FString Sql_Rollback;

	static const FName PN_DatabaseName;
	static const FName PN_SchemaName;

	static const FString Sql_AttachDatabase;
	static const FString Sql_DetachDatabase;

	static const FString Sql_ListSchemas;
	static const FString Sql_SchemaExists;

	static const FName TN_StoredStatements;
	static const FString Sql_CreateStoredStatements;

	static const FName TN_Properties;
	static const FString Sql_CreateProperties;

	static const FName TN_ActorsStore;
	static const FString Sql_CreateActorsStore;

	static const FName TN_Log;
	static const FString Sql_CreateLog;

	// ---------------------------------------------------------------------------

	/**
	 * Note: DatabaseInfo asset has been validated by editor.
	 */
	void Initialize( const USqliteDatabaseInfo* DatabaseInfo );

	/**
	 *
	 */
	ESqliteDatabaseOpenExecutionPins DoOpenSqliteDatabase();

	/**
	 * Create a table.
	 */
	bool CreateTable( FName TableName, FString Sql ) const;

	// ===========================================================================
	// = 
	// ===========================================================================

	/**
	 *
	 */
	bool bParentOnCreateCalled;

	/**
	 * Creates the table for the query manager facility and other internally used
	 * tables.
	 */
	bool OnCreatePrivate();

	// ---------------------------------------------------------------------------

	/**
	 *
	 */
	bool bParentOnUpdateCalled;

	/**
	 * Updates the table for the query manager facility and other internally used
	 * tables.
	 */
	bool OnUpdatePrivate( int NewDatabaseVersion, int OldDatabaseVersion );

	// ---------------------------------------------------------------------------

	/**
	 * Set the application_id PRAGMA to our custom value. A value different of
	 * zero indicates that the database has been initialized and the internally 
	 * used tables have been created.
	 * 
	 * This method is called by the Open() method logic when every of the
	 * OnCreatePrivate, OnCreate method and possibly the OnCreateEvent have
	 * been processed.
	 */
	bool UpdateApplicationId( FName Schema = FName("main") );

	/**
	 * Set the user_version PRAGMA to our current version of the database
	 * schema. This is used to determine if a call to OnUpdate() and the
	 * OnUpdate BluePrint event is required.
	 * 
	 * This method is called by the Open() method logic when every of the
	 * OnCreatePrivate, OnCreate method and possibly the OnCreateEvent have
	 * been processed.
	 * 
	 * It is also call when the OnUpdatePrivate, OnUpdate method and possibly
	 * the OnUpdateEvent have been processed.

	 */
	bool UpdateUserVersion( FName Schema = FName( "main" ) );

	/*
	 * Called by subsystem to terminate undergoing actions and close database 
	 */
	void Finalize();

	// ---------------------------------------------------------------------------

	static unsigned int AutovacuumCallbackGlue( void*, const char*, unsigned int, unsigned int, unsigned int );
	static void PreupdateHookGlue( void*, sqlite3*, int, const char*, const char*, int64, int64 );
	static void UpdateHookGlue( void*, int, char const*, char const*, int64 );
	static int CommitHookGlue( void* );
	static void RollbackHookGlue( void* );

protected:
	virtual unsigned int AutovacuumCallback( const FName&, unsigned int, unsigned int, unsigned int );
	virtual void PreupdateHook( int Operation, const FString& DatabaseName, const FName& TableName, int64 RowId1, int64 RowId2 );
	virtual void UpdateHook( int Operation, const FString& DatabaseName, const FName& TableName, int64 RowId );
	virtual int CommitHook();
	virtual void RollbackHook();

	/**
	 * This method is typically used by C++ derived class to implement the
	 * database schema creation logic. Overriding method should call its
	 * superclass version of the method.
	 * 
	 * It is also called before the blueprint OnCreate event.
	 */
	virtual bool OnCreate();

	/**
	 * 
	 */
	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Sqlite3|Database" )
	void OnCreateEvent( ESqliteReturnCode& Result );

	virtual void OnCreateEvent_Implementation( ESqliteReturnCode& Result ) final;

	/**
	 * This method is typically used by C++ derived class to implement the
	 * database schema update logic. Overriding method should call its
	 * superclass version of the method.
	 *
	 * It is also called before the blueprint OnUpdate event.
	 */
	virtual bool OnUpdate( int NewDatabaseVersion, int OldDatabaseVersion );

	/**
	 *
	 */
	UFUNCTION( BlueprintNativeEvent, BlueprintCallable, Category = "Sqlite3|Database")
	void OnUpdateEvent( int NewDatabaseVersion, int OldDatabaseVersion, ESqliteReturnCode& Result );

	virtual void OnUpdateEvent_Implementation( int NewDatabaseVersion, int OldDatabaseVersion, ESqliteReturnCode& Result ) final;

public:

#pragma region *** Livecycle
	// ===========================================================================
	// = Database lifecycle ======================================================
	// ===========================================================================

	/**
	 * Open the database and create/update it if necessary.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database", meta = (ExpandEnumAsExecs = "Branch") )
	void Open( ESqliteDatabaseOpenExecutionPins& Branch, ESqliteErrorCode& ErrorCode );

	/**
	 * Close the database and release any left resources.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	void Close();

#pragma endregion

#pragma region *** Versioning
	// ===========================================================================
	// = application_id & user_version ===========================================
	// ===========================================================================

	/**
	 * (Blueprint version)
	 * Read the application_id from the database.
	 *
	 * @param Branch - Upon return, will determine the execution pin
	 * @param OutApplicationId - Where to store the application_id from the database
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Version", meta = (ExpandEnumAsExecs = "Branch"))
	void GetApplicationId( ESqliteDatabaseSimpleExecutionPins& Branch, int& OutApplicationId );

	/**
	 * (C++ version)
	 * Read the application_id from the database.
	 *
	 * @param OutApplicationId - Where to store the application_id from the database
	 * @return True if operation succeeded
	 */
	bool GetApplicationId( int& OutApplicationId ) const;

	/**
	 * (Blueprint version)
	 * Read the user_version from the database.
	 * 
	 * @param Branch - Upon return, will determine the execution pin
	 * @param OutUserVersion - Where to store the user_version from the database
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Version", meta = (ExpandEnumAsExecs = "Branch") )
	void GetUserVersion( ESqliteDatabaseSimpleExecutionPins& Branch, int& OutUserVersion );

	/**
	 * (C++ version)
	 * Read the user_version from the database.
	 *
	 * @param OutUserVersion - Where to store the user_version from the database
	 * @return True if operation succeeded
	 */
	bool GetUserVersion( int& OutUserVersion ) const;

#pragma endregion

#pragma region *** Transactions
	// ===========================================================================
	// = Transactions ============================================================
	// ===========================================================================

	/**
	 * Begins and SQL transaction.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Transaction" )
		virtual int BeginTransaction( const FString& Hint = "" ) final;

	/**
	 * Commits and SQL transaction.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Transaction" )
	virtual int Commit( const FString& Hint = "" ) final;

	/**
	 * Rollbacks and SQL transaction.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Transaction" )
	virtual int Rollback( const FString& Hint = "" ) final;

#pragma endregion

#pragma region *** Errors
	// ===========================================================================
	// = Errors ==================================================================
	// ===========================================================================

	/**
	 * Get the last error code or extended error code (if enabled) for the connection.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	int GetErrorCode();

	/**
	 * Get the last extended error code for the connection.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	int GetExtentedErrorCode();

	/**
	 * Get the string associated with the last error code for the connection.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	FString GetErrorMessage();

	/**
	 * Get the offset of the token referenced by the last error or -1.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	int GetErrorOffset();

	/**
	 * Get the string associated with the given error code.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	FString GetErrorString( int ErrorCode );

#pragma endregion

#pragma region *** Hooks
	// ===========================================================================
	// = Callbacks & hooks =======================================================
	// ===========================================================================

	void EnableAutovacuumCallback( bool enabled );
	void EnablePreupdateHook( bool enabled );
	void EnableUpdateHook( bool enabled );
	void EnableCommitHook( bool enabled );
	void EnableRollbackHook( bool enabled );

#pragma endregion

	// ===========================================================================
	// = 
	// ===========================================================================

	bool IsInitialized();

	bool IsOpen();

	FString GetDatabaseFileName() const;

	FString GetDatabaseFilePath();

	/**
	 * (Blueprint version)
	 * Get the last error code from Sqlite.
	 * 
	 * @return The error code
	 *
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database" )
	ESqliteErrorCode GetLastErrorCode();

	int GetLastErrorCodeCxx() const;

	// ===========================================================================
	// = 
	// ===========================================================================

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	USqliteStatement* Prepare( FString sql );
};

