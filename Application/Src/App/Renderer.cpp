#include "Renderer.h"
namespace Utils
{
    static uint32_t ConvertToRGBA( const Vec4F& color )
    {
        uint8_t r = (uint8_t)(color.x * 255.0f);
        uint8_t g = (uint8_t)(color.y * 255.0f);
        uint8_t b = (uint8_t)(color.z * 255.0f);
        uint8_t a = (uint8_t)(color.w * 255.0f);

        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;

        return result;
    }
}

Renderer::Renderer( Graphics& gfx )
    :
    finalImage( 0, 0, nullptr, gfx )
{
}

void Renderer::Render( const Camera& camera  )
{
    activeCamera = &camera;

    for( int y = 0; y < finalImage.GetHeight(); y++ )
    {
        for( int x = 0; x < finalImage.GetWidth(); x++ )
        {
            Vec4F color = PerPixel( Vec2F{ (float)x,(float)y } );
            color = Vec4F::Clamp( color, Vec4F( 0.0f ), Vec4F( 1.0f ) );

            ImageData[y * finalImage.GetWidth() + x] = Utils::ConvertToRGBA( color );
        }
    }

    finalImage.SetData( ImageData );
}

void Renderer::OnResize( int width, int height )
{
    if( !finalImage.Active() || finalImage.GetWidth() != width || finalImage.GetHeight() != height )
    {
        finalImage = Image( width, height, nullptr, finalImage.gfx );
        delete[] ImageData;
        ImageData = new uint32_t[width * height];
    }
}

Vec4F Renderer::PerPixel( Vec2F coord )
{
    const Vec3F sphereOrigin{ 0,0,0 };
    const float radius = 1.0f;

    const int maxIterations = 100;
    const float surfaceDistance = 0.01f;
    const float maxDistance = 100.0f;

    Ray ray = Ray{ .Origin = activeCamera->GetPosition(),
        .Direction = activeCamera->GetRayDirections()[int( (int)coord.y * finalImage.GetWidth() + (int)coord.x )]
    };

    for( size_t i = 0; i < maxIterations; i++ )
	{
		float distance = signedDistanceSphere( ray.Origin, sphereOrigin, radius );
		if( distance < surfaceDistance )
		{
			return Vec4F( 1.0f, 0.0f, 0.0f, 1.0f );
		}

		if( distance > maxDistance )
		{
			break;
		}

		ray.Origin += ray.Direction * distance;
	} 

    return Vec4F( 0.2f, 0.4f, 1.0f, 1.0f );
}