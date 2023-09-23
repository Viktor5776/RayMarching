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
	void Render( const Camera& camera, const Scene& scene );
	void OnResize( int width, int height );
	Image& GetFinalImage() { return rayMarcherShader.GetImage(); }
	int& GetRenderIterations() { return renderIterations; }
private:
	int renderIterations = 8;
	ComputeShader rayMarcherShader;
};