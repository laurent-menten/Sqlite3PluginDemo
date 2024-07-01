// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfoFactory.h"
#include "SqliteDatabaseInfo.h"
#include "SqliteDatabaseInfoTypeActions.h"
#include <IAssetTools.h>
#include <Modules/ModuleManager.h>

USqliteDatabaseInfoFactory::USqliteDatabaseInfoFactory() 
{
	SupportedClass = USqliteDatabaseInfo::StaticClass();
	bEditAfterNew = true;
	bCreateNew = true;
}

bool USqliteDatabaseInfoFactory::IsMacroFactory() const
{
	return false;
}

UObject* USqliteDatabaseInfoFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check( InClass->IsChildOf( USqliteDatabaseInfoFactory::StaticClass() ) );

	auto Database = NewObject<USqliteDatabaseInfo>( InParent, InClass, InName, Flags | RF_Transactional, Context );
	return Database;
}
