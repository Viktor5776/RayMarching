#pragma once
#include "../Win/Image.h"
#include "../Utils/Vec4.h"
#include "Ray.h"
#include "Camera.h"
#include "Scene.h"

using namespace Hydro;

class Renderer
{
public:
	Renderer( Graphics& gfx );
	void Render( const Camera& camera );
	void OnResize( int width, int height );
	Image& GetFinalImage() { return finalImage; }
private:
	struct HitPayload
	{
		float HitDistance = 0.0f;
		Vec3F WorldPosition;
		Vec3F WorldNormal;
	};

	Vec4F PerPixel( Vec2F coord );
	HitPayload MarchRay( Ray ray, int maxIterations, float surfaceDistance, float maxDistance );
private:
	const Camera* activeCamera = nullptr;
	Image finalImage;
	uint32_t* ImageData = nullptr;
};