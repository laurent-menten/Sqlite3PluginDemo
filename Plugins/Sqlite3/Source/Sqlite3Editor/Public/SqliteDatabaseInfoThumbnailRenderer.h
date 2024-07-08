// (c)2024+ Laurent Menten

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/TextureThumbnailRenderer.h"
#include "SqliteDatabaseInfoThumbnailRenderer.generated.h"

/**
 *
 */
UCLASS()
class  SQLITE3EDITOR_API USqliteDatabaseInfoThumbnailRenderer : public UTextureThumbnailRenderer
{
	GENERATED_BODY()

private:
	UPROPERTY()
    TObjectPtr<UObject> Texture;

public:
    USqliteDatabaseInfoThumbnailRenderer();

    virtual void Draw( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily ) override;

    virtual void GetThumbnailSize( UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight ) const override;
};
