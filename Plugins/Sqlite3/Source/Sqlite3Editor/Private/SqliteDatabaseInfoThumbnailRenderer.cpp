// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfoThumbnailRenderer.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"

USqliteDatabaseInfoThumbnailRenderer::USqliteDatabaseInfoThumbnailRenderer()
{
	static ConstructorHelpers::FObjectFinder<UTexture> TextureFinder( TEXT( "Texture2D'/Sqlite3/database-icon.database-icon'" ) );

	UE_LOG(LogTemp, Error, TEXT("XXX Texture loaded : %d"), TextureFinder.Succeeded());

	Texture = TextureFinder.Object;
}

void USqliteDatabaseInfoThumbnailRenderer::Draw( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily )
{
	Super::Draw( Texture, X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily );
}

void USqliteDatabaseInfoThumbnailRenderer::GetThumbnailSize( UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight ) const
{
	Super::GetThumbnailSize( Texture, Zoom, OutWidth, OutHeight );
}
