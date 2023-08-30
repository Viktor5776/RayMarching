#pragma once
#include "../Win/Graphics.h"
#include <string>
#include "../Win/Image.h"
#include "Camera.h"
#include "Scene.h"

using namespace Hydro;

class ComputeShader
{
public:
	ComputeShader( Graphics& gfx, const std::wstring& path );
	Image& GetImage() { return image; }
	void OnResize( int width, int height );
	void Dispatch( const Camera& camera, const Scene& scene );
private:
	Graphics& gfx;
	Image image;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pOutputTexture;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pOutputUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pSkyboxTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSkyboxSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSkyboxSampler;
};