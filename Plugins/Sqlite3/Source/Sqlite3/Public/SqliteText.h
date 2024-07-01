// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SqliteData.h"
#include "SqliteText.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3_API USqliteText : public USqliteData
{
	GENERATED_BODY()

	friend class USqliteStatement;

private:
	const unsigned char* Value;
	int Size;

public:
	virtual FString ToString() const override;
};
