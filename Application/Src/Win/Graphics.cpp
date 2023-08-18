#include "Graphics.h"
#include <assert.h>

namespace Hydro
{
	Graphics::Graphics( HWND hWnd )
		:
		pDevice( nullptr ),
		pDeviceContext( nullptr ),
		pSwapChain( nullptr ),
		pRenderTarget( nullptr )
	{
		//Settings for swap chain
		DXGI_SWAP_CHAIN_DESC swap_chain_descr = { 0 };
		swap_chain_descr.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_descr.BufferDesc.RefreshRate.Denominator = 1u;
		swap_chain_descr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap_chain_descr.SampleDesc.Count = 1u;
		swap_chain_descr.SampleDesc.Quality = 0;
		swap_chain_descr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_descr.BufferCount = 1u;
		swap_chain_descr.OutputWindow = hWnd;
		swap_chain_descr.Windowed = true;

		//Set Flags
		D3D_FEATURE_LEVEL feature_level;
		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		//Initilize D3D11
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			flags,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&swap_chain_descr,
			&pSwapChain,
			&pDevice,
			&feature_level,
			&pDeviceContext );

		//Check Errors
		assert( S_OK == hr && pSwapChain && pDevice && pDeviceContext );

		//Create RenderTarget
		Microsoft::WRL::ComPtr<ID3D11Texture2D> frameBuffer;
		hr = pSwapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			(void**)frameBuffer.GetAddressOf()
		);
		assert( SUCCEEDED( hr ) );
		
		hr = pDevice->CreateRenderTargetView(
			frameBuffer.Get(), 0, pRenderTarget.GetAddressOf()
		);
		assert( SUCCEEDED( hr ) );
		

		//Set Viewport
		RECT winRect;
		GetClientRect( hWnd, &winRect );
		D3D11_VIEWPORT viewport = {};
		viewport.Width = (FLOAT)(winRect.right - winRect.left);
		viewport.Height = (FLOAT)(winRect.bottom - winRect.top);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pDeviceContext->RSSetViewports( 1u, &viewport );

		//Set Output Merger
		pDeviceContext->OMSetRenderTargets( 1, pRenderTarget.GetAddressOf(), NULL);

		//Init Imgui
		ImGui_ImplDX11_Init( pDevice.Get(), pDeviceContext.Get() );
	}

	Graphics::~Graphics()
	{
		ImGui_ImplDX11_Shutdown();
		if( pDeviceContext) pDeviceContext->ClearState();
	}

	void Graphics::OnResize( int width, int height )
	{
		//Resize components
		pDeviceContext->OMSetRenderTargets( 0, 0, 0 );
		pRenderTarget->Release();
		HRESULT hr = pSwapChain->ResizeBuffers( 0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0 );
		assert( SUCCEEDED( hr ) );

		Microsoft::WRL::ComPtr<ID3D11Texture2D> frameBuffer;
		hr = pSwapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			(void**)frameBuffer.GetAddressOf()
		);
		assert( SUCCEEDED( hr ) );
		hr = pDevice->CreateRenderTargetView(
			frameBuffer.Get(), 0, pRenderTarget.GetAddressOf()
		);
		assert( SUCCEEDED( hr ) );


		//Set Viewport
		D3D11_VIEWPORT viewport = {};
		viewport.Width = (FLOAT)width;
		viewport.Height = (FLOAT)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pDeviceContext->RSSetViewports( 1u, &viewport );

		//Set Output Merger
		pDeviceContext->OMSetRenderTargets( 1, pRenderTarget.GetAddressOf(), NULL );
	}

	void Graphics::EndFrame()
	{
		//Render Imgui frame
		ImGui::Render();
		const float background[] = { 0.025f,0.12f,0.2f,1.0f };
		pDeviceContext->OMSetRenderTargets(1, pRenderTarget.GetAddressOf(), nullptr );
		pDeviceContext->ClearRenderTargetView( pRenderTarget.Get(), background );
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

		if( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		pSwapChain->Present( 1u, 0 );
	}

	void Graphics::BeginFrame()
	{
		//Start imgui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		
	}
	ID3D11Device* Graphics::GetDevice()
	{
		return pDevice.Get();
	}
}
