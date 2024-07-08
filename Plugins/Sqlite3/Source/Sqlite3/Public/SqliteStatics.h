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
	// - Library version ---------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 *
	 */
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Library|Version" )
	static FString LibVersion();

	/**
	 *
	 */
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Library|Version" )
	static int LibVersionNumber();

	/**
	 *
	 */
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Library|Version" )
	static FString LibSourceId();

	// ---------------------------------------------------------------------------
	// - Config ------------------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 * 0: No mutex code compiled in.
	 * 1: Mutex code compiled in, serialized mode by default.
	 * 2: Mutex code compiled in, multi-thread mode by default.
	 */
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Library|Config" )
	static int IsThreadSafe();

	// ---------------------------------------------------------------------------
	// - Error codes -------------------------------------------------------------
	// ---------------------------------------------------------------------------

	static const char* GetSqlite3ErrorSymbol( int ErrorCode );
	
	/**
	 * Get the corresponding ESqlite[Extended]ErrorCode enum value for a native error code.
	 * 
	 * @param ErrorCode - A native Sqlite error code
	 * @return The corresponding value from the ESqlite[Extended]ErrorCode enum
	 */
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static ESqliteErrorCode MapNativeErrorCode( int ErrorCode );

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static ESqliteExtendedErrorCode MapNativeExtendedErrorCode( int ErrorCode );

	/**
	 * Get the corresponding native error code from a ESqlite[Extended]ErrorCode enum value.
	 * 
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @return The corresponding native Sqlite error code
	 */
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static int UnmapNativeErrorCode( ESqliteErrorCode ErrorCode );

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static int UnmapNativeExtendedErrorCode( ESqliteExtendedErrorCode ErrorCode );

	/**
	 * Get the English-language text that describes the error.
	 * Note: only returns primary result.
	 *
	 * @param ErrorCode - A native Sqlite error code
	 * @return The text that describes the error
	 */

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static FString NativeErrorString( int ErrorCode );

	/**
	 * Get the English-language text that describes the error.
	 * Note: only returns primary result.
	 *
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @return The text that describes the error
	 */

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static FString ErrorString( ESqliteErrorCode ErrorCode );

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static FString ExtendedErrorString( ESqliteExtendedErrorCode ErrorCode );

	/**
	 * Checks if the native error code is the same as a ESqlite[Extended]ErrorCode.
	 * 
	 * @param NativeErrorCode - A native Sqlite error code
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @return true if same
	 */

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static bool CheckSqliteErrorCode( int NativeErrorCode, ESqliteErrorCode ErrorCode );
	
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = "Sqlite3|Errors" )
	static bool CheckSqliteExtendedErrorCode( int NativeErrorCode, ESqliteExtendedErrorCode ErrorCode );

	/**
	 * Banched if the native error code is the same as a ESqlite[Extended]ErrorCode.
	 * 
	 * @param NativeErrorCode - A native Sqlite error code
	 * @param ErrorCode - An ESqlite[Extended]ErrorCode enum value
	 * @param Branch - An ESqliteDatabaseEqualityExecutionPins enum value
	 */
	
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Errors", meta = (ExpandEnumAsExecs = "Branch") )
	static void BranchIfSqliteErrorCode( int NativeErrorCode, ESqliteErrorCode ErrorCode, ESqliteDatabaseEqualityExecutionPins& Branch );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Errors", meta = (ExpandEnumAsExecs = "Branch") )
	static void BranchIfSqliteExtendedErrorCode( int NativeErrorCode, ESqliteExtendedErrorCode ErrorCode, ESqliteDatabaseEqualityExecutionPins& Branch );
};
