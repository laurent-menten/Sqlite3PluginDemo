// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "SqliteDatabaseInfoValidator.generated.h"

struct FDatabaseTable;
class USqliteDatabaseInfo;

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

	static void GenerateCreateDatabaseSqlCommands( USqliteDatabaseInfo* DatabaseInfos );
	static FString GenerateCreateTableSqlCommand( const FDatabaseTable& CustomTable, const FString& SchemaName = FString("") );

private:
	static void SqliteCheck( USqliteDatabaseInfo* DatabaseInfos, FDataValidationContext& Context );
};
