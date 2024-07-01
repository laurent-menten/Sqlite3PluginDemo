// (c)2024+ Laurent Menten

#include "SqliteStatement.h"
#include "SqliteStatics.h"
#include "SqliteInteger.h"
#include "SqliteFloat.h"
#include "SqliteText.h"
#include "SqliteBlob.h"
#include "SqliteNull.h"
#include "Sqlite3Log.h"

// ---------------------------------------------------------------------------
// - 
// ---------------------------------------------------------------------------

// Blueprint
USqliteDatabase* USqliteStatement::GetDatabase() const
{
	return Database.Get();
}

// ---------------------------------------------------------------------------
// - 
// ---------------------------------------------------------------------------

int USqliteStatement::Step() const
{
	const int rc = sqlite3_step( StatementHandler );
	if( (rc != SQLITE_ROW) && (rc != SQLITE_DONE) )
	{
		UE_LOG( LogSqlite, Error, TEXT("USqliteStatement::Step = (%d) %s"), rc, *USqliteStatics::NativeErrorString( rc ) );
	}

	return rc;
}

int USqliteStatement::Finalize()
{
	UE_LOG( LogSqlite, Log, TEXT( "Finalize" ) );

	const int rc = sqlite3_finalize( StatementHandler );
	if( rc == SQLITE_OK )
	{
		Database->StatementFinalized( this );
	}
	else
	{
		UE_LOG( LogSqlite, Error, TEXT("USqliteStatement::Finalize = (%d) %s"), rc, *USqliteStatics::NativeErrorString( rc ) );
	}
	
	return rc;
}

// ---------------------------------------------------------------------------
// - Parameters binding ------------------------------------------------------
// ---------------------------------------------------------------------------

int USqliteStatement::ClearBindings()
{
	return sqlite3_clear_bindings( StatementHandler );
}

int USqliteStatement::Reset() const
{
	return sqlite3_reset( StatementHandler );
}

int USqliteStatement::GetBindParameterCount() const
{
	return sqlite3_bind_parameter_count( StatementHandler );
}

// ---------------------------------------------------------------------------

int USqliteStatement::GetBindParameterIndex( ESqliteDatabaseSimpleExecutionPins& Branch, const FString ColumnName )
{
	const int ColumnIndex = sqlite3_bind_parameter_index( StatementHandler, StringCast<ANSICHAR>( *ColumnName ).Get() );
	if( ColumnIndex == 0 )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return 0;
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return ColumnIndex;
}

FString USqliteStatement::GetBindParameterName( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex )
{
	const char* ColumnName = sqlite3_bind_parameter_name( StatementHandler, ColumnIndex );
	if( ColumnName == nullptr )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return FString( "" );
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return FString( ColumnName );
}

// ---------------------------------------------------------------------------

int USqliteStatement::BindDouble( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, const double Value, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding float value %f to columne %d"), Value, ColumnIndex );

	const int rc = sqlite3_bind_double( StatementHandler, ColumnIndex, Value );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

int USqliteStatement::BindInteger( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, const int Value, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding int value %d to columne %d"), Value, ColumnIndex );

	const int rc = sqlite3_bind_int( StatementHandler, ColumnIndex, Value );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

int USqliteStatement::BindInteger64(ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, const int64 Value, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding int64 value %lld to columne %d"), Value, ColumnIndex );

	const int rc = sqlite3_bind_int64( StatementHandler, ColumnIndex, Value );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

int USqliteStatement::BindText( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, const FString Value, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding text value \"%s\" to columne %d"), *Value, ColumnIndex );

	const int rc = sqlite3_bind_text( StatementHandler, ColumnIndex, StringCast<ANSICHAR>(*Value).Get(), -1, SQLITE_TRANSIENT );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

int USqliteStatement::BindNull( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding NULL value to columne %d"), ColumnIndex );

	const int rc = sqlite3_bind_null( StatementHandler, ColumnIndex );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

int USqliteStatement::BindZeroBlob( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, const int DataSize, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding zero blob of size %d to columne %d"), DataSize, ColumnIndex );

	const int rc = sqlite3_bind_zeroblob( StatementHandler, ColumnIndex, DataSize );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

int USqliteStatement::BindZeroBlob64( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex, const int64 DataSize, USqliteStatement*& Statement )
{
	UE_LOG( LogSqlite, Log, TEXT("Binding zero blob (64) of size %lld to columne %d"), DataSize, ColumnIndex );

	const int rc = sqlite3_bind_zeroblob64( StatementHandler, ColumnIndex, DataSize );
	if( rc != SQLITE_OK )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
	}
	else
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	}

	Statement = this;
	return rc;
}

// ---------------------------------------------------------------------------
// - Result columns ----------------------------------------------------------
// ---------------------------------------------------------------------------

int USqliteStatement::GetColumnCount() const
{
	return sqlite3_column_count( StatementHandler );
}

int USqliteStatement::GetDataCount() const
{
	return sqlite3_data_count( StatementHandler );
}

// ---------------------------------------------------------------------------

FString USqliteStatement::GetColumnName( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex ) const
{
	const char* ColumnName = sqlite3_column_name( StatementHandler, ColumnIndex );
	if( ColumnName == nullptr )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return FString( "" );
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return FString( ColumnName );
}

FString USqliteStatement::GetColumnDatabaseName( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex ) const
{
	const char* ColumnName = sqlite3_column_database_name( StatementHandler, ColumnIndex );
	if( ColumnName == nullptr )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return FString( "" );
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return FString( ColumnName );
}

FString USqliteStatement::GetColumnTableName( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex ) const
{
	const char* ColumnName = sqlite3_column_table_name( StatementHandler, ColumnIndex );
	if( ColumnName == nullptr )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return FString( "" );
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return FString( ColumnName );
}

FString USqliteStatement::GetColumnOriginName( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex ) const
{
	const char* ColumnName = sqlite3_column_origin_name( StatementHandler, ColumnIndex );
	if( ColumnName == nullptr )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return FString( "" );
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return FString( ColumnName );
}

ESqliteType USqliteStatement::GetColumnType( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex ) const
{
	switch( sqlite3_column_type( StatementHandler, ColumnIndex ) )
	{
		case SQLITE_INTEGER:
			Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
			return ESqliteType::Integer;

		case SQLITE_FLOAT:
			Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
			return ESqliteType::Float;

		case SQLITE_TEXT:
			Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
			return ESqliteType::Text;

		case SQLITE_BLOB:
			Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
			return ESqliteType::Blob;

		case SQLITE_NULL:
			Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
			return ESqliteType::Null;

		default:
			Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
			return ESqliteType::None;
	}
}

FString USqliteStatement::GetColumnDeclaredType( ESqliteDatabaseSimpleExecutionPins& Branch, const int ColumnIndex ) const
{
	const char* ColumnDeclaredType = sqlite3_column_decltype( StatementHandler, ColumnIndex );
	if( ColumnDeclaredType == nullptr )
	{
		Branch = ESqliteDatabaseSimpleExecutionPins::OnFail;
		return FString( "" );
	}

	Branch = ESqliteDatabaseSimpleExecutionPins::OnSuccess;
	return FString( ColumnDeclaredType );
}

// ---------------------------------------------------------------------------

int USqliteStatement::GetColumnAsInteger( const int ColumnIndex ) const
{
	return sqlite3_column_int( StatementHandler, ColumnIndex );
}

int64 USqliteStatement::GetColumnAsInteger64( const int ColumnIndex ) const
{
	return sqlite3_column_int64( StatementHandler, ColumnIndex );
}

double USqliteStatement::GetColumnAsDouble( const int ColumnIndex ) const
{
	return sqlite3_column_double( StatementHandler, ColumnIndex );
}

FString USqliteStatement::GetColumnAsString( const int ColumnIndex ) const
{
	return FString( (char*) sqlite3_column_text( StatementHandler, ColumnIndex ) );
}

// ---------------------------------------------------------------------------

const void* USqliteStatement::GetColumnAsBlob( const int ColumnIndex ) const
{
	return sqlite3_column_blob( StatementHandler, ColumnIndex );
}

const unsigned char* USqliteStatement::GetColumnAsText( const int ColumnIndex ) const
{
	return sqlite3_column_text( StatementHandler, ColumnIndex );
}

int USqliteStatement::GetColumnBytes( const int ColumnIndex ) const
{
	return sqlite3_column_bytes( StatementHandler, ColumnIndex );
}

// ---------------------------------------------------------------------------
// - 
// ---------------------------------------------------------------------------

TArray<USqliteData*> USqliteStatement::GetResultSet() const
{
	TArray<USqliteData*> ResultSet;

	const int ColumnCount = sqlite3_column_count( StatementHandler );
	for( int ColumnIndex = 0; ColumnIndex < ColumnCount; ColumnIndex++ )
	{
		switch( const int DataType = sqlite3_column_type( StatementHandler, ColumnIndex ) )
		{
			case SQLITE_INTEGER:
			{
				USqliteInteger* IntegerData = NewObject<USqliteInteger>();

				IntegerData->Value = sqlite3_column_int64( StatementHandler, ColumnIndex );
				ResultSet.Add( IntegerData );
				break;
			}

			case SQLITE_FLOAT:
			{
				USqliteFloat* FloatData = NewObject<USqliteFloat>();

				FloatData->Value = sqlite3_column_double( StatementHandler, ColumnIndex );

				ResultSet.Add( FloatData );
				break;
			}

			case SQLITE_TEXT:
			{
				USqliteText* TextData = NewObject<USqliteText>();

				TextData->Value = sqlite3_column_text( StatementHandler, ColumnIndex );
				TextData->Size = sqlite3_column_bytes( StatementHandler, ColumnIndex );

				ResultSet.Add( TextData );
				break;
			}

			case SQLITE_BLOB:
			{
				USqliteBlob* BlobData = NewObject<USqliteBlob>();

				BlobData->Value = sqlite3_column_text( StatementHandler, ColumnIndex );
				BlobData->Size = sqlite3_column_bytes( StatementHandler, ColumnIndex );

				ResultSet.Add( BlobData );
				break;
			}

			case SQLITE_NULL:
			{
				USqliteNull* NullData = NewObject<USqliteNull>();

				ResultSet.Add( NullData );
				break;
			}

			default:
			{
				UE_LOG( LogSqlite, Error, TEXT( "Unknown Sqlite data type [%d]" ), DataType );
				break;
			}
		}
	}

	return ResultSet;
}

// ============================================================================
// = 
// ============================================================================

bool USqliteStatement::IsBusy() const
{
	return sqlite3_stmt_busy( StatementHandler ) != 0;
}

bool USqliteStatement::IsExplain() const
{
	return sqlite3_stmt_isexplain( StatementHandler ) == 1;
}

bool USqliteStatement::IsReadOnly() const
{
	return sqlite3_stmt_readonly( StatementHandler ) != 0;
}

int USqliteStatement::GetStatementStatus( ESqliteStatementStatus Counter, const bool ResetFlag ) const
{
	return sqlite3_stmt_status( StatementHandler, StaticCast<int>(Counter), ResetFlag );
}
