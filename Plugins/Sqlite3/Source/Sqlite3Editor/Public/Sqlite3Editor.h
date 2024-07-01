// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSqlite3EditorModule : public IModuleInterface
{
public:
	static const FName AssetCategoryName;
	static const FText AssetCatgegoryDisplayName;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
