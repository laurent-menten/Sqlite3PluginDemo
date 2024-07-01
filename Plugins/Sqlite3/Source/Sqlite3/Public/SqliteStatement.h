// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"

#include "sqlite/Sqlite3Include.h"

#include "SqliteEnums.h" 
#include "SqliteData.h" 
#include "SqliteStatement.generated.h"

// not implements
// sqlite3_sql, sqlite3_normalized_sql, sqlite3_expanded_sql
// sqlite3_stmt_scanstatus, sqlite3_stmt_scanstatus_v2, sqlite3_stmt_scanstatus_reset

/**
 * 
 */
UCLASS( Blueprintable )
class SQLITE3_API USqliteStatement : public UObject
{
	GENERATED_BODY()

	friend class USqliteDatabase;

private:
	UPROPERTY( BlueprintReadOnly, VisibleAnywhere, Category = "Sqlite3", meta = (AllowPrivateAccess = "true") )
	TObjectPtr<USqliteDatabase> Database;

	sqlite3_stmt* StatementHandler;

protected:

public:
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	USqliteDatabase* GetDatabase();

	// ---------------------------------------------------------------------------
	// - Statement work ----------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 * Evaluate a prepared statement.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int Step();

	/**
	 * Destroy a prepared statement object.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int Finalize();

	/**
	 * Reset all bindings on a prepared statement.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int ClearBindings();

	/**
	 * Reset a prepared statement object.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int Reset();

	// ---------------------------------------------------------------------------
	// - Parameters binding ------------------------------------------------------
	// ---------------------------------------------------------------------------

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings" )
	int GetBindParameterCount();

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch" ))
	int GetBindParameterIndex( ESqliteDatabaseSimpleExecutionPins& Branch, FString ColumnName );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch" ))
	FString GetBindParameterName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

//	int BindBlob( int ColumnIndex, const void* Data, int DataSize );
//	int BindBlob64( int ColumnIndex, const void* Data, int64 DataSize );

	/**
	 * Binding double values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindDouble(ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, double Value, USqliteStatement*& Statement);

	/**
	 * Binding int values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindInteger(ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int Value, USqliteStatement*& Statement);

	/**
	 * Binding int64 values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindInteger64(ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int64 Value, USqliteStatement*& Statement);

	/**
	 * Binding text (FString) values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindText( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, FString Value, USqliteStatement*& Statement );

//	int BindText(int ColumnIndex, const char* Value, int Length);
//	int BindText16( int ColumnIndex, const void* Data, int DataSize );
//	int BindText64( int ColumnIndex, const char* Value, int64 Length);

	/**
	 * Binding NULL values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindNull(ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, USqliteStatement*& Statement);

	/**
	 * Binding zero filled blob values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindZeroBlob(ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int DataSize, USqliteStatement*& Statement);

	/**
	 * Binding zero filled blob values To Prepared Statements
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch"))
	int BindZeroBlob64(ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int64 DataSize, USqliteStatement*& Statement);

//	int BindPointer( int ColumnIndex );
//	int BindValue( int ColumnIndex );

	// ---------------------------------------------------------------------------
	// - Result columns ----------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 * Get the number of columns in a result set regardless of its state.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns" )
	int GetColumnCount();

	/**
	 * Get the number of columns actually present in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns" )
	int GetDataCount();

	/**
	 * Get the name of a column in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns", meta = (ExpandEnumAsExecs = "Branch" ))
	FString GetColumnName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	/**
	 * Get the database name that is the origin of a column in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns", meta = (ExpandEnumAsExecs = "Branch" ))
	FString GetColumnDatabaseName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	/**
	 * Get the table name that is the origin of a column in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns", meta = (ExpandEnumAsExecs = "Branch" ))
	FString GetColumnTableName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	/**
	 * Get the column name that is the origin of a column in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns", meta = (ExpandEnumAsExecs = "Branch" ))
	FString GetColumnOriginName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	/**
	 * Get the type of a column in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns", meta = (ExpandEnumAsExecs = "Branch" ))
	ESqliteType GetColumnType( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	/**
	 * Get the declared type of a column in a result set.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns", meta = (ExpandEnumAsExecs = "Branch" ))
	FString GetColumnDeclaredType( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result" )
	int GetColumnAsInteger( int ColumnIndex );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result" )
	int64 GetColumnAsInteger64( int ColumnIndex );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result" )
	double GetColumnAsDouble( int ColumnIndex );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result" )
	FString GetColumnAsString( int ColumnIndex );

	// ---------------------------------------------------------------------------

	const void* GetColumnAsBlob( int ColumnIndex );
	const unsigned char* GetColumnAsText( int ColumnIndex );
	int GetColumnBytes( int ColumnIndex );

	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns" )
	TArray<USqliteData*> GetResultSet();

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * Check if a prepared statement has been reset. 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	bool IsBusy();

	/**
	 * Query the EXPLAIN setting for a prepared statement.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	bool IsExplain();

	/**
	 * Check if a prepared statement writes to the database.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	bool IsReadOnly();

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * Get the performance status of a prepared statement.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Performances" )
	int GetStatementStatus( ESqliteStatementStatus Counter, bool ResetFlag  );
};
