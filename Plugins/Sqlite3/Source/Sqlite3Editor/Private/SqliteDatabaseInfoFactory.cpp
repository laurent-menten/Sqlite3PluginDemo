// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfoFactory.h"
#include "SqliteDatabaseInfo.h"
#include "SqliteDatabaseInfoTypeActions.h"
#include <IAssetTools.h>
#include <Modules/ModuleManager.h>

USqliteDatabaseInfoFactory::USqliteDatabaseInfoFactory() 
{
	SupportedClass = USqliteDatabaseInfo::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

bool USqliteDatabaseInfoFactory::IsMacroFactory() const
{
	return false;
}

UObject* USqliteDatabaseInfoFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	checkf( InClass->IsChildOf( USqliteDatabaseInfoFactory::StaticClass() ), TEXT("Invalid class for database") );

	const auto NewDatabase = NewObject<USqliteDatabaseInfo>( InParent, InClass, InName, Flags | RF_Transactional, Context );
	
	return NewDatabase;
}
