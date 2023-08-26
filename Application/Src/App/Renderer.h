#pragma once
#include "../Win/Image.h"
#include "../Utils/Vec4.h"
#include "ComputeShader.h"
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
	Image& GetFinalImage() { return rayMarcherShader.GetImage(); }
private:
	const Camera* activeCamera = nullptr;
	ComputeShader rayMarcherShader;
};