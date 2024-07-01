// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SqliteDatabase.h"
#include "SqliteStatics.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:

public:
	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * Create a USqliteDatabase object.
	 *
	 * @param DatabaseInfos - Asset of type USqliteDatabaseInfos describing the database connection
	 * @param Branch (out) - Upon return, will determine the execution pin
	 * @param ReturnCode (out) - The return code explaining the failure
	 * @param DatabaseHandle (out) - A pointer to the USqliteDatabase on success or null
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Database", meta = (ExpandEnumAsExecs = "Branch", DisplayName = "Create Sqlite Database Object from DatabaseInfos asset") )
	static void CreateSqliteDatabaseObject_Asset(
		const USqliteDatabaseInfo* DatabaseInfos,
		ESqliteDatabaseSimpleExecutionPins& Branch,
		ESqliteErrorCode& ReturnCode,
		UPARAM(DisplayName = "Database") USqliteDatabase* & DatabaseHandle
	);

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 *
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Version|Library" )
	static FString LibVersion();

	/**
	 *
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Version|Library" )
	static int LibVersionNumber();

	/**
	 *
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Version|Library" )
	static FString LibSourceId();

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * 0: No mutex code compiled in.
	 * 1: Mutex code compiled in, serialized mode by default.
	 * 2: Mutex code compiled in, multi-thread mode by default.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Config" )
	static int IsThreadSafe();

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * Get the corresponding ESqlite[Extended]ErrorCode enum value for a native error code.
	 * 
	 * @param ErrorCode - A native Sqlite error code
	 * @return The corresponding value from the ESqlite[Extended]ErrorCode enum
	 */
	static ESqliteErrorCode MapNativeErrorCode( int ErrorCode );
	static ESqliteExtendedErrorCode MapNativeExtendedErrorCode( int ErrorCode );

	/**
	 * Get the corresponding native error code from a ESqlite[Extended]ErrorCode enum value.
	 * 
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @return The corresponding native Sqlite error code
	 */
	static int UnmapNativeErrorCode( ESqliteErrorCode ErrorCode );

	static int UnmapNativeExtendedErrorCode( ESqliteExtendedErrorCode ErrorCode );

	/**
	 * Get the English-language text that describes the error.
	 * Note: only returns primary result.
	 *
	 * @param ErrorCode - A native Sqlite error code
	 * @return The text that describes the error
	 */
	static FString NativeErrorString(int ErrorCode);

	/**
	 * Get the English-language text that describes the error.
	 * Note: only returns primary result.
	 *
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @return The text that describes the error
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Utils" )
	static FString ErrorString( ESqliteErrorCode ErrorCode );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Utils" )
	static FString ExtendedErrorString( ESqliteExtendedErrorCode ErrorCode );

	/**
	 * Checks if the native error code is the same as a ESqliteExtended]ErrorCode.
	 * 
	 * @param NativeErrorCode - A native Sqlite error code
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @return true if valid
	 */

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Utils" )
	static bool CheckSqliteErrorCode( int NativeErrorCode, ESqliteErrorCode ErrorCode );

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Utils" )
	static bool CheckSqliteExtendedErrorCode( int NativeErrorCode, ESqliteExtendedErrorCode ErrorCode );

};
