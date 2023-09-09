#include "ComputeShader.h"
#include "../Win/Texture.h"

ComputeShader::ComputeShader( Graphics& gfx, const std::wstring& path )
	:
	gfx( gfx ),
	image( 0, 0, nullptr, gfx )
{
	//Load shader
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob( path.c_str(), &pBlob );
	auto hr = gfx.GetDevice()->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pComputeShader );
	assert( SUCCEEDED( hr ) );

	//Load skybox
	Texture skybox = Texture( "Src/App/Textures/Skybox.bmp" );
	
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = skybox.GetWidth();
	textureDesc.Height = skybox.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = (void*)skybox.GetData();
	subresourceData.SysMemPitch = skybox.GetWidth() * 4;

	hr = gfx.GetDevice()->CreateTexture2D( &textureDesc, &subresourceData, &pSkyboxTexture );
	assert( SUCCEEDED( hr ) );

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = gfx.GetDevice()->CreateShaderResourceView( pSkyboxTexture.Get(), &srvDesc, &pSkyboxSRV );
	assert( SUCCEEDED( hr ) );

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = gfx.GetDevice()->CreateSamplerState( &samplerDesc, &pSkyboxSampler );
	assert( SUCCEEDED( hr ) );

	gfx.GetDeviceContext()->CSSetSamplers( 0, 1, pSkyboxSampler.GetAddressOf() );

	OnResize( 0, 0 );
}

void ComputeShader::OnResize( int width, int height )
{
	if( (image.Active() && width == image.GetWidth() && height == image.GetHeight()) || width == 0 || height == 0 )
		return;
		
	
	image = Image( width, height, nullptr, gfx );

	//Create output texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	auto hr = gfx.GetDevice()->CreateTexture2D( &textureDesc, nullptr, &pOutputTexture );
	assert( SUCCEEDED( hr ) );

	//Create output UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	hr = gfx.GetDevice()->CreateUnorderedAccessView( pOutputTexture.Get(), &uavDesc, &pOutputUAV );
	assert( SUCCEEDED( hr ) );
}

void ComputeShader::Dispatch( const Camera& camera, const Scene& scene )
{
	//Set Constant buffer
	struct ConstantBuffer
	{
		Matrix4F inverseProjection;
		Matrix4F inverseView;
		Vec3F cameraPosition;
		float pad = 0.0f;
		Scene scene;
	};

	ConstantBuffer cb;
	cb.inverseProjection = camera.GetInverseProjection();
	cb.inverseView = camera.GetInverseView();
	cb.cameraPosition = camera.GetPosition();
	cb.scene = scene;

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof( ConstantBuffer );
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbData = {};
	cbData.pSysMem = &cb;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	auto hr = gfx.GetDevice()->CreateBuffer( &cbDesc, &cbData, &pConstantBuffer );
	assert( SUCCEEDED( hr ) );

	gfx.GetDeviceContext()->CSSetConstantBuffers( 0, 1, pConstantBuffer.GetAddressOf() );
	//Set other resources
	gfx.GetDeviceContext()->CSSetShader( pComputeShader.Get(), nullptr, 0 );
	gfx.GetDeviceContext()->CSSetUnorderedAccessViews( 0, 1, pOutputUAV.GetAddressOf(), nullptr );
	gfx.GetDeviceContext()->CSSetShaderResources( 0, 1, pSkyboxSRV.GetAddressOf() );


	//Dispatch
	gfx.GetDeviceContext()->Dispatch( image.GetWidth() / 8, image.GetHeight() / 8, 1 );

	//Unbind resources
	gfx.GetDeviceContext()->CSSetShader( nullptr, nullptr, 0 );
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	gfx.GetDeviceContext()->CSSetUnorderedAccessViews( 0, 1, &nullUAV, nullptr );
	ID3D11ShaderResourceView* nullSRV = nullptr;
	gfx.GetDeviceContext()->CSSetShaderResources( 0, 1, &nullSRV );


	//Copy output texture to image
	image.SetTexture( pOutputTexture.Get() );
}
