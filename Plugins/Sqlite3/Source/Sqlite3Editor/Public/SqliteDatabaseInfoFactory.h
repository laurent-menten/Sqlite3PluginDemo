// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SqliteDatabaseInfoFactory.generated.h"

/**
 * 
 */
UCLASS()
class SQLITE3EDITOR_API USqliteDatabaseInfoFactory : public UFactory
{
	GENERATED_BODY()
	
protected:
	virtual bool IsMacroFactory() const;

public:
	USqliteDatabaseInfoFactory();

	virtual UObject* FactoryCreateNew( UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn ) override;
};