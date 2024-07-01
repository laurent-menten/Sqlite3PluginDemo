// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SqliteData.h"
#include "SqliteInteger.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteInteger : public USqliteData
{
	GENERATED_BODY()

	friend class USqliteStatement;

private:
	int64 Value;

public:
	virtual FString ToString() const override;
};
