// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SqliteData.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteData : public UObject
{
	GENERATED_BODY()

public:
	virtual FString ToString() const;

	UFUNCTION( BlueprintCallable, Category="Sqlite3", meta = (DisplayName = "ToString") )
	virtual FString ToString_Blueprint() const;
};
