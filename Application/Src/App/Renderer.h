#pragma once
#include "../Win/Image.h"
#include "../Utils/Vec4.h"
#include "Ray.h"
#include "Camera.h"

using namespace Hydro;

class Renderer
{
public:
	Renderer( Graphics& gfx );
	void Render( const Camera& camera );
	void OnResize( int width, int height );
	Image& GetFinalImage() { return finalImage; }
private:
	Vec4F PerPixel( Vec2F coord );
private:
	const Camera* activeCamera = nullptr;
	Image finalImage;
	uint32_t* ImageData = nullptr;
};