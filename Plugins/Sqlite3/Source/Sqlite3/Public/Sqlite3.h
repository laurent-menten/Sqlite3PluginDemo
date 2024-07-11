// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "Sqlite3Commons.h"

class FSqlite3Module : public IModuleInterface
{
private:
	ISqlite3Editor* Sqlite3Editor;

public:
	FSqlite3Module();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// ------------------------------------------------------------------------
	// - 
	// ------------------------------------------------------------------------

	static inline FSqlite3Module& GetModule()
	{
		static const FName ModuleName = "Sqlite3";
		return FModuleManager::LoadModuleChecked< FSqlite3Module >(ModuleName);
	}

	static inline bool IsModuleLoaded()
	{
		static const FName ModuleName = "Sqlite3";
		return FModuleManager::Get().IsModuleLoaded(ModuleName);
	}

	virtual void RegisterSqlite3EditorInterface( ISqlite3Editor* InSqlite3Editor );

	ISqlite3Editor* GetEditor() const;
};
