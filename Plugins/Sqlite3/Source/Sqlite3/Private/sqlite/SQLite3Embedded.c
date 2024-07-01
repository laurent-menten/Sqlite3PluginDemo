// Copyright Epic Games, Inc. All Rights Reserved.

/** Needed to define USING_CODE_ANALYSIS, and allow direct access to the Sqlite API from outside this module */

#if PLATFORM_WINDOWS
	#include "Windows/WindowsPlatformCodeAnalysis.h"

	#define DLLEXPORT __declspec(dllexport)
	#define DLLIMPORT __declspec(dllimport)
#elif defined(__clang__)
	#include "Clang/ClangPlatformCodeAnalysis.h"

	#define DLLEXPORT __attribute__((visibility("default")))
	#define DLLIMPORT __attribute__((visibility("default")))
#endif

/** Include Sqlite, but not if we're building for analysis as the code emits warnings */

#if !defined(USING_CODE_ANALYSIS) || !USING_CODE_ANALYSIS
	#include "HAL/PreprocessorHelpers.h"
	#include COMPILED_PLATFORM_HEADER(PlatformCompilerPreSetup.h)

	UE_COMPILER_THIRD_PARTY_INCLUDES_START

		#include "sqlite/sqlite3.c-inline"

		#if defined(SQLITE_ENABLE_SQLLOG)
			#include "sqlite/test_sqllog.c-inline"
		#endif

	UE_COMPILER_THIRD_PARTY_INCLUDES_END

#endif
