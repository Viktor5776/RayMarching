#include "Renderer.h"

Renderer::Renderer( Graphics& gfx )
    :
    rayMarcherShader( gfx, L"RayMarcher.cso" )
{
}

void Renderer::Render( const Camera& camera  )
{
    activeCamera = &camera;

    rayMarcherShader.Dispatch( *activeCamera );
}

void Renderer::OnResize( int width, int height )
{
    rayMarcherShader.OnResize( width, height );
}
