// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "SqliteDatabaseInfo.h"

class FSqliteDatabaseInfoTypeActions : public FAssetTypeActions_Base
{
	friend class FsqLite3EditorModule;
private:
	EAssetTypeCategories::Type AssetCategory;

public:
	FSqliteDatabaseInfoTypeActions( EAssetTypeCategories::Type InAssetCategory );

	virtual void OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual FText GetName() const override;

	virtual uint32 GetCategories() override;

	virtual FColor GetTypeColor() const override;

	virtual FText GetAssetDescription( const FAssetData& AssetData ) const override;

	virtual UClass* GetSupportedClass() const override;


	virtual UThumbnailInfo* GetThumbnailInfo( UObject* Asset ) const override;

};
