// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "SqliteDatabaseInfo.h"
#include "SqliteDatabaseInfoValidator.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3EDITOR_API USqliteDatabaseInfoValidator : public UEditorValidatorBase
{
	GENERATED_BODY()

public:
	USqliteDatabaseInfoValidator();
	
	virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const override;
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context) override;

	static void GenerateCreateTableSqlCommands( USqliteDatabaseInfo* DatabaseInfos );
	static FString GenerateCreateTableSqlCommand( const FDatabaseTable& CustomTable );
};
