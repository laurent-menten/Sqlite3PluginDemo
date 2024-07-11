// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SqliteDatabaseInfoTypeActions.h"
#include "Modules/ModuleManager.h"

class USqliteDatabaseInfoValidator;

class FSqlite3EditorModule : public IModuleInterface
{
	friend class USqliteDatabaseInfoValidator;
	
private:
	static USqliteDatabaseInfoValidator* Validator;

	TSharedPtr<FSqliteDatabaseInfoTypeActions> SqliteDatabaseInfoTypeActions;

public:
	static const FName AssetCategoryName;
	static const FText AssetCatgegoryDisplayName;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
