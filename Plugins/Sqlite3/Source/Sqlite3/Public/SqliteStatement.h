// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "sqlite/Sqlite3Include.h"

#include "SqliteEnums.h" 
#include "SqliteData.h" 
#include "SqliteStatement.generated.h"

// not yet implements:

// sqlite3_sql, sqlite3_normalized_sql, sqlite3_expanded_sql
// sqlite3_stmt_scanstatus, sqlite3_stmt_scanstatus_v2, sqlite3_stmt_scanstatus_reset

//	int BindText(int ColumnIndex, const char* Value, int Length);
//	int BindText16( int ColumnIndex, const void* Data, int DataSize );
//	int BindText64( int ColumnIndex, const char* Value, int64 Length);

//	int BindBlob( int ColumnIndex, const void* Data, int DataSize );
//	int BindBlob64( int ColumnIndex, const void* Data, int64 DataSize );
	
//	int BindPointer( int ColumnIndex );
//	int BindValue( int ColumnIndex );

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

public:
	/**
	 * Get the database object associated with this statement.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	USqliteDatabase* GetDatabase() const;

	// ---------------------------------------------------------------------------
	// - Statement work ----------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 * Evaluate a prepared statement.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, BlueprintPure=false, Category = "Sqlite3|Statement" )
	int Step() const;

	/**
	 * Destroy a prepared statement object.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int Finalize();

	// ---------------------------------------------------------------------------
	// - Parameters binding ------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 * Reset all bindings on a prepared statement.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int ClearBindings();

	/**
	 * Reset a prepared statement object back to its initial state and ready to be re-executed.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	int Reset() const;

	// ------------------------------------------------------------------------
	
	/**
	 * Get the number of parameters in a prepared statement.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings" )
	int GetBindParameterCount() const;

	/**
	 * Get the index of an SQL parameter given its name.
	 * 
	 * @param Branch 
	 * @param ColumnName 
	 * @return 0 if not found.
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int GetBindParameterIndex( ESqliteDatabaseSimpleExecutionPins& Branch, FString ColumnName );

	/**
	 * Get the name of an SQL parameter given its index.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	FString GetBindParameterName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex );

	// ------------------------------------------------------------------------

	/**
	 * Binding double values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param Value 
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindDouble( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, double Value, USqliteStatement*& Statement );

	/**
	 * Binding int values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param Value 
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindInteger( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int Value, USqliteStatement*& Statement );

	/**
	 * Binding int64 values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param Value 
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindInteger64( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int64 Value, USqliteStatement*& Statement );

	/**
	 * Binding text (FString) values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param Value 
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindText( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, FString Value, USqliteStatement*& Statement );

	/**
	 * Binding NULL values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindNull( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, USqliteStatement*& Statement );

	/**
	 * Binding zero filled blob values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param DataSize 
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindZeroBlob( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int DataSize, USqliteStatement*& Statement );

	/**
	 * Binding zero filled blob values To Prepared Statements.
	 * 
	 * @param Branch 
	 * @param ColumnIndex (stating at 1)
	 * @param DataSize 
	 * @param Statement 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Bindings", meta = (ExpandEnumAsExecs = "Branch") )
	int BindZeroBlob64( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex, int64 DataSize, USqliteStatement*& Statement );

	// ---------------------------------------------------------------------------
	// - Result columns ----------------------------------------------------------
	// ---------------------------------------------------------------------------

	/**
	 * Get the number of columns in a result set regardless of its state.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns" )
	int GetColumnCount() const;

	/**
	 * Get the number of columns actually present in a result set.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns" )
	int GetDataCount() const;

	/**
	 * Get the name of a column in a result set.
	 * 
	 * @param Branch 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns", meta = (ExpandEnumAsExecs = "Branch") )
	FString GetColumnName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex ) const;

	/**
	 * Get the database name that is the origin of a column in a result set.
	 * 
	 * @param Branch 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns", meta = (ExpandEnumAsExecs = "Branch") )
	FString GetColumnDatabaseName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex ) const;

	/**
	 * Get the table name that is the origin of a column in a result set.
	 * 
	 * @param Branch 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns", meta = (ExpandEnumAsExecs = "Branch") )
	FString GetColumnTableName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex ) const;

	/**
	 * Get the column name that is the origin of a column in a result set.
	 * 
	 * @param Branch 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Resul Sett|Columns", meta = (ExpandEnumAsExecs = "Branch") )
	FString GetColumnOriginName( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex ) const;

	/**
	 * Get the type of a column in a result set.
	 * 
	 * @param Branch 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns", meta = (ExpandEnumAsExecs = "Branch") )
	ESqliteType GetColumnType( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex ) const;

	/**
	 * Get the declared type of a column in a result set.
	 * 
	 * @param Branch 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set|Columns", meta = (ExpandEnumAsExecs = "Branch") )
	FString GetColumnDeclaredType( ESqliteDatabaseSimpleExecutionPins& Branch, int ColumnIndex ) const;

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set" )
	int GetColumnAsInteger( int ColumnIndex ) const;

	/**
	 * 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set" )
	int64 GetColumnAsInteger64( int ColumnIndex ) const;

	/**
	 * 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set" )
	double GetColumnAsDouble( int ColumnIndex ) const;

	/**
	 * 
	 * @param ColumnIndex 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result Set" )
	FString GetColumnAsString( int ColumnIndex ) const;

	const void* GetColumnAsBlob( int ColumnIndex ) const;
	
	const unsigned char* GetColumnAsText( int ColumnIndex ) const;

	int GetColumnBytes( int ColumnIndex ) const;

	// ---------------------------------------------------------------------------

	/**
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement|Result|Columns Set" )
	TArray<USqliteData*> GetResultSet() const;

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * Check if a prepared statement has been reset. 
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	bool IsBusy() const;

	/**
	 * Query the EXPLAIN setting for a prepared statement.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	bool IsExplain() const;

	/**
	 * Check if a prepared statement writes to the database.
	 * 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Statement" )
	bool IsReadOnly() const;

	// ---------------------------------------------------------------------------
	// - 
	// ---------------------------------------------------------------------------

	/**
	 * Get the performance status of a prepared statement.
	 * 
	 * @param Counter 
	 * @param ResetFlag 
	 * @return 
	 */
	UFUNCTION( BlueprintCallable, Category = "Sqlite3|Performances" )
	int GetStatementStatus( ESqliteStatementStatus Counter, bool ResetFlag  ) const;
};
