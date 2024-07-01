// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SqliteData.h"
#include "SqliteFloat.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteFloat : public USqliteData
{
	GENERATED_BODY()

	friend class USqliteStatement;

private:
	double Value;

public:
	virtual FString ToString() const override;
};
