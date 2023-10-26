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

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4( 1.00f,1.00f,1.00f,1.00f );
		style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f,0.50f,0.50f,1.00f );
		style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.13f,0.14f,0.15f,1.00f );
		style.Colors[ImGuiCol_ChildBg] = ImVec4( 0.13f,0.14f,0.15f,1.00f );
		style.Colors[ImGuiCol_PopupBg] = ImVec4( 0.13f,0.14f,0.15f,1.00f );
		style.Colors[ImGuiCol_Border] = ImVec4( 0.43f,0.43f,0.50f,0.50f );
		style.Colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f,0.00f,0.00f,0.00f );
		style.Colors[ImGuiCol_FrameBg] = ImVec4( 0.25f,0.25f,0.25f,1.00f );
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.38f,0.38f,0.38f,1.00f );
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.67f,0.67f,0.67f,0.39f );
		style.Colors[ImGuiCol_TitleBg] = ImVec4( 0.08f,0.08f,0.09f,1.00f );
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.08f,0.08f,0.09f,1.00f );
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f,0.00f,0.00f,0.51f );
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.14f,0.14f,0.14f,1.00f );
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.02f,0.02f,0.02f,0.53f );
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.31f,0.31f,0.31f,1.00f );
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.41f,0.41f,0.41f,1.00f );
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.51f,0.51f,0.51f,1.00f );
		style.Colors[ImGuiCol_CheckMark] = ImVec4( 0.11f,0.64f,0.92f,1.00f );
		style.Colors[ImGuiCol_SliderGrab] = ImVec4( 0.11f,0.64f,0.92f,1.00f );
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.08f,0.50f,0.72f,1.00f );
		style.Colors[ImGuiCol_Button] = ImVec4( 0.25f,0.25f,0.25f,1.00f );
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4( 0.38f,0.38f,0.38f,1.00f );
		style.Colors[ImGuiCol_ButtonActive] = ImVec4( 0.67f,0.67f,0.67f,0.39f );
		style.Colors[ImGuiCol_Header] = ImVec4( 0.22f,0.22f,0.22f,1.00f );
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.25f,0.25f,0.25f,1.00f );
		style.Colors[ImGuiCol_HeaderActive] = ImVec4( 0.67f,0.67f,0.67f,0.39f );
		style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.41f,0.42f,0.44f,1.00f );
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4( 0.26f,0.59f,0.98f,0.95f );
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4( 0.00f,0.00f,0.00f,0.00f );
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.29f,0.30f,0.31f,0.67f );
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.26f,0.59f,0.98f,0.95f );
		style.Colors[ImGuiCol_Tab] = ImVec4( 0.08f,0.08f,0.09f,0.83f );
		style.Colors[ImGuiCol_TabHovered] = ImVec4( 0.33f,0.34f,0.36f,0.83f );
		style.Colors[ImGuiCol_TabActive] = ImVec4( 0.23f,0.23f,0.24f,1.00f );
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4( 0.08f,0.08f,0.09f,1.00f );
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.13f,0.14f,0.15f,1.00f );
		style.Colors[ImGuiCol_DockingPreview] = ImVec4( 0.26f,0.59f,0.98f,0.70f );
		style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.20f,0.20f,0.20f,1.00f );
		style.Colors[ImGuiCol_PlotLines] = ImVec4( 0.61f,0.61f,0.61f,1.00f );
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f,0.43f,0.35f,1.00f );
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f,0.70f,0.00f,1.00f );
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f,0.60f,0.00f,1.00f );
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.26f,0.59f,0.98f,0.35f );
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4( 0.11f,0.64f,0.92f,1.00f );
		style.Colors[ImGuiCol_NavHighlight] = ImVec4( 0.26f,0.59f,0.98f,1.00f );
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f,1.00f,1.00f,0.70f );
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f,0.80f,0.80f,0.20f );
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.80f,0.80f,0.80f,0.35f );
		style.GrabRounding = style.FrameRounding = 2.3f;

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
	ID3D11DeviceContext* Graphics::GetDeviceContext()
	{
		return pDeviceContext.Get();
	}
}
