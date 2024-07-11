// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealGameUtils.h"

#define LOCTEXT_NAMESPACE "FUnrealGameUtilsModule"

void FUnrealGameUtilsModule::StartupModule()
{
	UE_LOG( LogTemp, Warning, TEXT( "Unreal game utils ... %p" ), this );
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUnrealGameUtilsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FUnrealGameUtilsModule, UnrealGameUtils )
