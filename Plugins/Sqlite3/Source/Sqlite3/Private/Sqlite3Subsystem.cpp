// (c)2024+ Laurent Menten

#include "Sqlite3Subsystem.h"
#include "platform/Sqlite3Platform.h"

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

#include "Misc/MessageDialog.h"

#if PLATFORM_WINDOWS

#include <shlobj.h>

#elif PLATEFORM_LINUX

// no include required

#elif PLATFORM_PS4
#error PS4 platform not supported...
#elif PLATFORM_XBOXONE
#error XBOX-1 platform not supported...
#elif PLATFORM_MAC
#error MAC platform not supported...
#elif PLATFORM_IOS
#error IOS platform not supported...
#elif PLATFORM_ANDROID
#error ANDROID platform not supported...
#elif PLATFORM_WINRT
#error WIN-RT platform not supported...
#elif PLATFORM_HTML5
#error HTML5 platform not supported...
#else	
#error Unknown platform not supported...
#endif

#include "Sqlite3Log.h"
#include "SqliteStatics.h"
#include "SqliteDatabase.h"

#define LOCTEXT_NAMESPACE "FSqlite3Module"

// ============================================================================
// 
// ============================================================================

static const FText MessageBoxTitle = FText::FromString( "Sqlite subsystem" );

// ============================================================================
// 
// ============================================================================

void USqlite3Subsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	UE_LOG( LogSqlite, Log, TEXT( "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --" ) );
	UE_LOG( LogSqlite, Log, TEXT( "--      Sqlite subsystem initialization      --" ) );
	UE_LOG( LogSqlite, Log, TEXT( "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --" ) );

	UE_LOG( LogSqlite, Log, TEXT("Sqlite library version: %s [%s]."),
		*FString( sqlite3_libversion() ),
		*FString( sqlite3_sourceid() ) );

	// ---------------------------------------------------------------------------
	// Compute default database directory name for game in the user directory.
	//	Windows: %AppData% / %ProjectName% /
	//	Linux: ~ / .%ProjectName /
	// ---------------------------------------------------------------------------

#if PLATFORM_WINDOWS

	PWSTR LocalAppDataPath;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, nullptr, &LocalAppDataPath);

	DefaultDatabaseDirectory = FString(LocalAppDataPath) / FString("");

#elif PLATFORM_LINUX

	DefaultDatabaseDirectory = FString("~") / FString(".") ;
	
#endif

	const TCHAR* ProjectName = FApp::GetProjectName();
	DefaultDatabaseDirectory += FString(ProjectName) / "";

	UE_LOG( LogSqlite, Log, TEXT("Default database directory: '%s'."), *DefaultDatabaseDirectory );

	// ----------------------------------------------------------------------------
	// Create default database directory if it does not exists
	// ----------------------------------------------------------------------------

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if( !PlatformFile.DirectoryExists( *DefaultDatabaseDirectory ) )
	{
		if( !PlatformFile.CreateDirectoryTree( *DefaultDatabaseDirectory ) )
		{
			const FText Format = LOCTEXT("SqliteDirectoryError", "Error creating directory '{Directory}'.\nAborting.");

			FFormatNamedArguments Args;
			Args.Add( TEXT( "Directory" ), FText::FromString( DefaultDatabaseDirectory ) );

			const FText msg = FText::Format( Format, Args );

			FMessageDialog::Open( EAppMsgType::Ok, msg, MessageBoxTitle );

			UE_LOG( LogSqlite, Fatal, TEXT("Default database directory could not be created. Aborting.") );
			return;
		}

		UE_LOG( LogSqlite, Log, TEXT("Default database directory successfully created.") );
	}

	// ---------------------------------------------------------------------------
	// Initialize library
	// ---------------------------------------------------------------------------

	if( ! InitializeSqlite() )
	{
		const FText Format = LOCTEXT( "SqliteLibraryInitializationError", "Error initializing Sqlite library : ({ErrorCode}) {ErrorString}.\nAborting." );

		FFormatNamedArguments Args;
		Args.Add( TEXT( "ErrorCode" ), SqliteInitializationStatus );
		Args.Add( TEXT( "ErrorString" ), FText::FromString( USqliteStatics::NativeErrorString( SqliteInitializationStatus ) ) );

		const FText msg = FText::Format( Format, Args );

		FMessageDialog::Open( EAppMsgType::Ok, msg, MessageBoxTitle );

		UE_LOG( LogSqlite, Fatal, TEXT("Library not initialized. Aborting.") );
	}
	else
	{
		UE_LOG( LogSqlite, Log, TEXT("Library successfully initialized") );
	}

	UE_LOG(LogSqlite, Log, TEXT("-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"));
}

bool USqlite3Subsystem::InitializeSqlite()
{
	if( ! bIsSqliteInitialized )
	{
#if SQLITE_OS_OTHER

		SqliteInitializationStatus = sqlite3_ue_config();
		if( SqliteInitializationStatus != SQLITE_OK )
		{
			LOG_SQLITE_ERROR( SqliteInitializationStatus, "Sqlite Platform specific initialization failed." );

			return bIsSqliteInitialized;
		}

#endif

		SqliteInitializationStatus = sqlite3_initialize();
		if( SqliteInitializationStatus != SQLITE_OK )
		{
			LOG_SQLITE_ERROR( SqliteInitializationStatus, "Sqlite initialization failed." );

			return bIsSqliteInitialized;
		}

		bIsSqliteInitialized = true;
	}

	return bIsSqliteInitialized;
}

// ============================================================================
// 
// ============================================================================

void USqlite3Subsystem::Deinitialize()
{
	UE_LOG( LogSqlite, Log, TEXT( "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --" ) );
	UE_LOG( LogSqlite, Log, TEXT( "--     Sqlite subsystem deinitialization     --" ) );
	UE_LOG( LogSqlite, Log, TEXT( "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --" ) );

	// ---------------------------------------------------------------------------
	// Handle database finalization
	// ---------------------------------------------------------------------------

	for( const auto& db : Databases )
	{

		UE_LOG( LogSqlite, Log, TEXT("Finalizing database '%s'"), *db->GetDatabaseFileName() );

		db->Finalize();
	}

	Databases.Empty();

	// ---------------------------------------------------------------------------
	// Deinitialize external library
	// ---------------------------------------------------------------------------

	DeinitializeSqlite();

	UE_LOG( LogSqlite, Log, TEXT("Library successfully deinitialized") );

	UE_LOG(LogSqlite, Log, TEXT("-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"));
}

void USqlite3Subsystem::DeinitializeSqlite()
{
	if( bIsSqliteInitialized )
	{
		sqlite3_shutdown();
	}
}

// ============================================================================
// 
// ============================================================================

USqlite3Subsystem* USqlite3Subsystem::GetInstance()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance( GEngine->GameViewport->GetWorld() );
	return GameInstance->GetSubsystem<USqlite3Subsystem>();
}

bool USqlite3Subsystem::IsSqliteInitialized()
{
	return bIsSqliteInitialized;
}

int USqlite3Subsystem::getSqliteInitializationStatus()
{
	return SqliteInitializationStatus;
}

FString USqlite3Subsystem::GetDefaultDatabaseDirectory()
{
	return DefaultDatabaseDirectory;
}

void USqlite3Subsystem::RegisterDatabase( USqliteDatabase* Database )
{
	Databases.Add( Database );
}

#undef LOCTEXT_NAMESPACE