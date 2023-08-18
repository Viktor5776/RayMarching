#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Graphics.h"

namespace Hydro
{
	class Image
	{
	public:
		Image( unsigned int width, unsigned int height, void* data, Graphics& gfx )
			:
			width( width ),
			height( height ),
			gfx( gfx )
		{
			if( width == 0 || height == 0 || data == nullptr )
			{
				return;
			}
			
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			textureDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = data;
			subresourceData.SysMemPitch = width * 4;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
			gfx.GetDevice()->CreateTexture2D( &textureDesc, &subresourceData, &pTexture );

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			gfx.GetDevice()->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView );
		}
		Image operator=( const Image& rhs )
		{
			width = rhs.width;
			height = rhs.height;
			gfx = rhs.gfx;
			pTextureView = rhs.pTextureView;

			return *this;
		}
		void SetData( void* data )
		{
			if( width == 0 || height == 0 ||  data == nullptr )
			{
				return;
			}

			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			textureDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pSysMem = data;
			subresourceData.SysMemPitch = width * 4;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
			gfx.GetDevice()->CreateTexture2D( &textureDesc, &subresourceData, &pTexture );

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			gfx.GetDevice()->CreateShaderResourceView( pTexture.Get(), &srvDesc, &pTextureView );
		}
		int GetWidth()
		{
			return width;
		}
		int GetHeight()
		{
			return height;
		}
		void* GetData()
		{
			return (void*)pTextureView.Get();
		}
		bool Active()
		{
			return width != 0 && height != 0;
		}
		Graphics& gfx;
	private:
		unsigned int width, height;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}