// (c)2024+ Laurent Menten

#include "SqliteDatabaseInfoThumbnailRenderer.h"

#include "Sqlite3EditorLog.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"

USqliteDatabaseInfoThumbnailRenderer::USqliteDatabaseInfoThumbnailRenderer()
{
	static ConstructorHelpers::FObjectFinder<UTexture> TextureFinder( TEXT( "Texture2D'/Sqlite3/database-icon.database-icon'" ) );

	UE_LOG( LogSqliteEditor, Error, TEXT("database-icon texture loaded : %d - %hs"), TextureFinder.Succeeded(), this == GetClass()->GetDefaultObject() ? " DEFAULT" : "" );

	Texture = TextureFinder.Object;
}

void USqliteDatabaseInfoThumbnailRenderer::Draw( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily )
{
	UE_LOG( LogSqliteEditor, Error, TEXT("draw database-icon") );

	Super::Draw( Texture.Get(), X, Y, Width, Height, Viewport, Canvas, bAdditionalViewFamily );
}

void USqliteDatabaseInfoThumbnailRenderer::GetThumbnailSize( UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight ) const
{
	UE_LOG( LogSqliteEditor, Error, TEXT("get size database-icon") );

	Super::GetThumbnailSize( Texture.Get(), Zoom, OutWidth, OutHeight );
}
