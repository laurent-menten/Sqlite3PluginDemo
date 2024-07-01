// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SqliteData.h"
#include "SqliteBlob.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteBlob : public USqliteData
{
	GENERATED_BODY()

	friend class USqliteStatement;

private:
	const void* Value;
	int Size;

public:
	virtual FString ToString() const override;
};
