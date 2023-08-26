#pragma once
#include <d3d11.h>       // D3D interface
#include <DirectXMath.h>
#pragma comment( lib, "d3d11.lib" )
#include <d3dcompiler.h> // Shader compiler
#pragma comment( lib, "d3dcompiler.lib" )

#include <Windows.h>
#include <wrl.h>
#include "Color.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

namespace Hydro
{
	class Graphics
	{
	public:
		Graphics( HWND hWnd );
		~Graphics();
		void OnResize( int width, int height );
		void EndFrame();
		void BeginFrame();
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTarget;
	};
}