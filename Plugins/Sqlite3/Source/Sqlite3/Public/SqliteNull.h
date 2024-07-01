// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SqliteData.h"
#include "SqliteNull.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteNull : public USqliteData
{
	GENERATED_BODY()

public:
	virtual FString ToString() const override;
};
