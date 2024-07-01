// Copyright Epic Games, Inc. All Rights Reserved.

using Microsoft.CodeAnalysis;
using System.Threading;
using UnrealBuildTool;

public class Sqlite3 : ModuleRules
{
	public Sqlite3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicSystemLibraries.Add("shell32.lib");

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        // ==========================================================================
        // = Sqlite library configuration ===========================================
        // ==========================================================================

        // Allow direct access to the Sqlite API from outside this module
        // Note: There are additional macros defined inside SqliteEmbedded.c to allow this API export to work
        PublicDefinitions.Add("SQLITE_API=SQLITE3_API");

        // We call sqlite3_initialize ourselves during module init
        PrivateDefinitions.Add("SQLITE_OMIT_AUTOINIT");

        // Use the math.h version of isnan rather than the Sqlite version to avoid a -ffast-math error
        PrivateDefinitions.Add("SQLITE_HAVE_ISNAN=1");

        // Turn on enforcement of foreign key constraints
        PrivateDefinitions.Add("SQLITE_DEFAULT_FOREIGN_KEYS=1");

        // Make Sqlite thread safe
        PrivateDefinitions.Add("SQLITE_THREADSAFE=1");

        // Enable FTS
        PrivateDefinitions.Add("SQLITE_ENABLE_FTS4");
        PrivateDefinitions.Add("SQLITE_ENABLE_FTS5");

        // Enable RBU
        PrivateDefinitions.Add("SQLITE_ENABLE_RBU");

 /*
        // Enable SQLLOG
        //
        // TODO: correct link problems
        // Sqlite3Embedded.c.obj : error LNK2019: unresolved external symbol getpid referenced in function testSqllog
        // Sqlite3Embedded.c.obj : error LNK2019: unresolved external symbol access referenced in function testSqllog
 
        PrivateDefinitions.Add("SQLITE_ENABLE_SQLLOG");
 */

        // Enable Offset func
        PrivateDefinitions.Add("SQLITE_ENABLE_OFFSET_SQL_FUNC");

        // Enable PreUpdate hook
        PrivateDefinitions.Add("SQLITE_ENABLE_PREUPDATE_HOOK");

        // Enable De-serialization and RTree Subsystem
        PrivateDefinitions.Add("SQLITE_ENABLE_DESERIALIZE");
        PrivateDefinitions.Add("SQLITE_ENABLE_RTREE");

        // Enable Json extension
        PrivateDefinitions.Add("SQLITE_ENABLE_JSON1");

        // Enable extra metadata
        PrivateDefinitions.Add("SQLITE_ENABLE_COLUMN_METADATA");

        // Enable session and snapshot extensions
        PrivateDefinitions.Add("SQLITE_ENABLE_SESSION");
        PrivateDefinitions.Add("SQLITE_ENABLE_SNAPSHOT");

/*
        // Use ICU with Sqlite if it's available
        //
        // TODO: correct link problems
        // icu.lib(resbund.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(characterproperties.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(servnotf.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(serv.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(servls.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(brkeng.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(locid.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(uresbund.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(umutex.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(unifiedcache.obj) : error LNK2001: unresolved external symbol __imp__Mtx_init_in_situ
        // icu.lib(resbund.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(characterproperties.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(servnotf.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(serv.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(servls.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(brkeng.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(locid.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(uresbund.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(umutex.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(unifiedcache.obj) : error LNK2001: unresolved external symbol __imp__Mtx_destroy_in_situ
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Mtx_lock referenced in function "public: void __cdecl std::condition_variable_any::wait<class std::mutex>(class std::mutex &)"(??$wait@Vmutex@std@@@condition_variable_any@std@@QEAAXAEAVmutex@1@@Z)
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Mtx_unlock referenced in function "public: void __cdecl std::condition_variable_any::wait<class std::mutex>(class std::mutex &)"(??$wait@Vmutex@std@@@condition_variable_any@std@@QEAAXAEAVmutex@1@@Z)
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Cnd_init_in_situ referenced in function "public: __cdecl std::condition_variable_any::condition_variable_any(void)"(?? 0condition_variable_any@std@@QEAA@XZ)
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Cnd_destroy_in_situ referenced in function "public: __cdecl icu_64::UConditionVar::~UConditionVar(void)"(?? 1UConditionVar@icu_64@@QEAA@XZ)
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Cnd_wait referenced in function "public: void __cdecl std::condition_variable_any::wait<class std::mutex>(class std::mutex &)"(??$wait@Vmutex@std@@@condition_variable_any@std@@QEAAXAEAVmutex@1@@Z)
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Cnd_broadcast referenced in function "void __cdecl icu_64::umtx_initImplPostInit(struct icu_64::UInitOnce &)"(?umtx_initImplPostInit@icu_64@@YAXAEAUUInitOnce@1@@Z)
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol __imp__Cnd_signal referenced in function umtx_condSignal_64
        // icu.lib(umutex.obj) : error LNK2019: unresolved external symbol "__declspec(dllimport) void __cdecl std::_Throw_C_error(int)"(__imp_ ? _Throw_C_error@std@@YAXH@Z) referenced in function "public: void __cdecl std::condition_variable_any::wait<class std::mutex>(class std::mutex &)"(??$wait@Vmutex@std@@@condition_variable_any@std@@QEAAXAEAVmutex@1@@Z)

        if (Target.bCompileICU)
        {
            PrivateDefinitions.Add("SQLITE_ENABLE_ICU");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "ICU");
        }
*/

        // Should we use the Unreal HAL rather than the SQLite platform implementations?
        if (Target.bCompileCustomSQLitePlatform)
        {
            // Note: The Unreal HAL doesn't provide an implementation of shared memory (as not all platforms implement it),
            // nor does it provide an implementation of granular file locks. These two things affect the concurrency of an
            // SQLite database as only one FSqliteDatabase can have the file open at the same time.

            PrivateDefinitions.Add("SQLITE_OS_OTHER=1");            // We are a custom OS
            PrivateDefinitions.Add("SQLITE_ZERO_MALLOC");           // We provide our own malloc implementation
            PrivateDefinitions.Add("SQLITE_MUTEX_NOOP");            // We provide our own mutex implementation
            PrivateDefinitions.Add("SQLITE_OMIT_LOAD_EXTENSION");   // We disable extension loading
        }

        // Enable Sqlite debug checks?
        //PrivateDefinitions.Add("SQLITE_DEBUG");

        // The platform Sqlite implementation generates a lot of these warnings
        bEnableUndefinedIdentifierWarnings = false;

        // Ensure the platform Sqlite implementation is always compiled in isolation
        bUseUnity = false;
    }
}
