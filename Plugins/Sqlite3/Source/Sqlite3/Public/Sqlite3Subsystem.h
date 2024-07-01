// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Sqlite3Subsystem.generated.h"

class USqliteStatics;
class USqliteDatabase;

/**
 * 
 */
UCLASS()
class SQLITE3_API USqlite3Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class USqliteStatics;

private:
	/**
	 * The path of the default directory where we put our databases.
	 * %AppData% / %ProjectName% /
	 */
	FString DefaultDatabaseDirectory;

	bool bIsSqliteInitialized = false;

	int SqliteInitializationStatus;

	/**
	 * A list of all the databases that were opened.
	 */
	TArray<USqliteDatabase*> Databases;

	// ---------------------------------------------------------------------------

	bool InitializeSqlite();

	void DeinitializeSqlite();

	// ---------------------------------------------------------------------------

	void RegisterDatabase( USqliteDatabase* Database );

public:

	// ---------------------------------------------------------------------------
	// - UnrealEngine runtime ----------------------------------------------------
	// ---------------------------------------------------------------------------
	
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;

	virtual void Deinitialize() override;

	// ---------------------------------------------------------------------------

	static USqlite3Subsystem* GetInstance();

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	bool IsSqliteInitialized();

	int getSqliteInitializationStatus();

	FString GetDefaultDatabaseDirectory();
};
