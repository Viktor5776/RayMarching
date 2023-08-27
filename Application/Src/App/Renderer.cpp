#include "Renderer.h"

Renderer::Renderer( Graphics& gfx )
    :
    rayMarcherShader( gfx, L"RayMarcher.cso" )
{
}

void Renderer::Render( const Camera& camera, const Scene& scene )
{
    rayMarcherShader.Dispatch( camera, scene );
}

void Renderer::OnResize( int width, int height )
{
    rayMarcherShader.OnResize( width, height );
}
