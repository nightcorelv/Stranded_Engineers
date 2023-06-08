#include "stdafx.h"
#include "BasicRenderer.h"

bool BasicRenderer::setUpInputLayout(ID3D11Device* device, const std::string& vShaderByteCode, ID3D11InputLayout* iLayout)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, (UINT)std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool BasicRenderer::setUp_NormalMap_InputLayout(ID3D11Device* device, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, (UINT)std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout_NormalMap);

	return !FAILED(hr);
}

bool BasicRenderer::setUpInputLayoutAnim(ID3D11Device* device, const std::string& vShaderByteCode, ID3D11InputLayout*& iLayout)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, (UINT)std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &this->animLayout);
	return !FAILED(hr);
}

bool BasicRenderer::setUp_Sky_InputLayout(ID3D11Device* device, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[1] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, (UINT)std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout_Skybox);

	return !FAILED(hr);
}

bool BasicRenderer::setUp_PT_InputLayout(ID3D11Device* device, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[7] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DELTA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"START_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"DIRECTION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ISON", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BASE_OFFSET", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, (UINT)std::size(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &pt_inputLayout);

	return !FAILED(hr);
}

bool BasicRenderer::setUpBlendState()
{
	D3D11_BLEND_DESC desc{};
	D3D11_RENDER_TARGET_BLEND_DESC& brt = desc.RenderTarget[0];

	brt.BlendEnable = true;
	brt.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	brt.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;

	brt.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	brt.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

	brt.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	brt.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;

	brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL; //write all channels
	HRESULT hr = GPU::device->CreateBlendState(&desc, &this->blendState);

	return !FAILED(hr);
}

bool BasicRenderer::setUpFresnelBlendState()
{
	D3D11_BLEND_DESC desc{};
	D3D11_RENDER_TARGET_BLEND_DESC& brt = desc.RenderTarget[0];

	brt.BlendEnable = true;
	brt.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	brt.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;

	brt.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;//D3D11_BLEND_OP_ADD;
	brt.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_MAX;//D3D11_BLEND_OP_ADD;

	brt.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	brt.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;

	brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL; //write all channels
	HRESULT hr = GPU::device->CreateBlendState(&desc, &this->fresnelBlendState);

	return !FAILED(hr);
}

bool BasicRenderer::setUpSampler(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0.0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = device->CreateSamplerState(&samplerDesc, &sampler);
	return !FAILED(hr);
}

bool BasicRenderer::setUpShadowSampler(ID3D11Device* device)
{
	//filter Nearst Neighbhor or Bilinear for 2D textures, linear get a smoother then zoom in to the texture
	//min is minification, mag is bigger, mip is mipmapping

	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
	desc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
	desc.MaxAnisotropy = 1;
	desc.BorderColor[0] = 1.0f;
	desc.MinLOD = 0;
	desc.MaxLOD = 0;

	HRESULT hr = device->CreateSamplerState(&desc, &shadowSampler);
	return !FAILED(hr);
}

bool BasicRenderer::setUpShadowRastirizer(ID3D11Device* device)
{
	//control over how polygons are rendered, do things like render in wireframe or draw both the front and back faces. 

	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 1.0f;
	desc.DepthClipEnable = true;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;

	HRESULT hr = GPU::device->CreateRasterizerState(&desc, &shadowRastirizer);
	return !FAILED(hr);
}

bool BasicRenderer::setUpRastirizer(ID3D11Device* device)
{
	//control over how polygons are rendered, do things like render in wireframe or draw both the front and back faces. 

	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = true;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = true;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	HRESULT hr = GPU::device->CreateRasterizerState(&desc, &Rastirizer);
	return !FAILED(hr);
}

BasicRenderer::BasicRenderer()
	:clearColour{ 0.0f, 0.0f, 0.0f, 0.0f }, halveraFps(false)
{
}

BasicRenderer::~BasicRenderer()
{
	rtv->Release();
	dsView->Release();
	dsView2->Release();			//Used for binding as render target
	dsTexture2->Release();		//	 ----------||------------
	depthSrv->Release();
	dsTexture->Release();
	sampler->Release();
	dsState->Release();

	inputLayout->Release();
	vShader->Release();
	pShader->Release();
	pShader_NoShadow->Release();
	ApShader->Release();
	vs_Skybox->Release();
	ps_Skybox->Release();
	inputLayout_Skybox->Release();

	pt_inputLayout->Release();
	pt_vShader->Release();
	pt_pShader->Release();
	pt_UpdateShader->Release();
	pt_gShader->Release();

	animLayout->Release();
	vShaderAnim->Release();

	shadowSampler->Release();
	shadowRastirizer->Release();
	bill_vShader->Release();
	bill_pShader->Release();
	bill_gShader->Release();
	PT_dsState->Release();
	blendState->Release();
	Fresnel_PS->Release();
	fresnelBlendState->Release();
	InvFresnel_PS->Release();

	inputLayout_NormalMap->Release();
	ps_normalMap->Release();
	vs_normalMap->Release();
	pt_UpdatePlayer->Release();
	hullShader->Release();
	domainShader->Release();
	basic_VertexShader->Release();
	postProcess->Release();
	backBufferUAV->Release();
	rtv2->Release();
	srv2->Release();
	//Rastirizer->Release();

	domainShader2->Release();
	hullShader2->Release();
	vs_normalMap2->Release();
}

void BasicRenderer::lightPrePass()
{
	ID3D11PixelShader* nullShader(nullptr);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(this->inputLayout);
	immediateContext->RSSetViewports(1, &shadowViewport);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->PSSetShader(nullShader, nullptr, 0);
}

bool BasicRenderer::initiateRenderer(ID3D11DeviceContext* immediateContext, ID3D11Device* device, IDXGISwapChain* swapChain, UINT WIDTH, UINT HEIGHT)
{
	int WidthAndHeight = 512;

	std::string vShaderByteCode;
	std::string vShaderByteCodeForAnim;
	this->immediateContext = immediateContext;

	if (this->immediateContext == nullptr)													return false;
	if (!CreateRenderTargetView(device, swapChain, rtv))									return false;
	if (!CreateNewRenderTargetView(device, rtv2, srv2))										return false;
	if (!CreateUnorderedView(device, swapChain, backBufferUAV, srvTest))					return false;
	if (!CreateDepthStencil(device, WIDTH, HEIGHT, dsTexture, dsView))						return false;
	if (!CreateDSState(dsState))															return false;
	if (!CreateDepthStencilAndSrv(device, WIDTH, HEIGHT, dsTexture2, dsView2, depthSrv))	return false;
	if (!LoadVertexShader(device, vShader, vShaderByteCode, "VertexShader"))				return false;
	if (!LoadVertexShader(device, basic_VertexShader, vShaderByteCode, "basic_VertexShader"))		return false;
	if (!LoadVertexShader(device, this->vShaderAnim, vShaderByteCodeForAnim, "vShaderSkinning"))	return false;
	if (!setUpInputLayoutAnim(device, vShaderByteCodeForAnim, this->animLayout))			return false;
	if (!setUpInputLayout(device, vShaderByteCode, this->inputLayout))						return false;
	if (!LoadPixelShader(device, pShader, "PixelShader"))									return false;
	if (!LoadPixelShader(device, pShader_NoShadow, "PixelShader_NoShadow"))					return false;
	if (!LoadPixelShader(device, ApShader, "AmbientPixelShader"))							return false;
	if (!LoadVertexShader(device, pt_vShader, vShaderByteCode, "PT_VertexShader"))			return false;
	if (!setUp_PT_InputLayout(device, vShaderByteCode))										return false;
	if (!LoadPixelShader(device, pt_pShader, "PT_PixelShader"))								return false;
	if (!LoadGeometryShader(device, pt_gShader, "PT_GeometryShader"))						return false;
	if (!LoadComputeShader(device, pt_UpdateShader, "PT_UpdateShader"))						return false;
	if (!setUpSampler(device))																return false;
	if (!setUpShadowSampler(device))														return false;
	if (!setUpShadowRastirizer(device))   													return false;
	if (!LoadVertexShader(device, vs_Skybox, vShaderByteCode, "Skybox_VS"))					return false;
	if (!setUp_Sky_InputLayout(device, vShaderByteCode))									return false;
	if (!LoadPixelShader(device, ps_Skybox, "Skybox_PS"))									return false;
	if (!LoadVertexShader(device, bill_vShader, vShaderByteCode, "Bilboard_VS"))			return false;
	if (!LoadPixelShader(device, bill_pShader, "Bilboard_PS"))								return false;
	if (!LoadGeometryShader(device, bill_gShader, "Bilboard_GS"))							return false;
	if (!CreatePT_DSState(PT_dsState))														return false;
	if (!setUpBlendState())																	return false;
	if (!LoadPixelShader(device, Fresnel_PS, "Fresnel_PS"))									return false;
	if (!LoadPixelShader(device, InvFresnel_PS, "InvFresnel_PS"))							return false;
	if (!setUpFresnelBlendState())															return false;


	

	//Normal map layout
	if (!LoadVertexShader(device, vs_normalMap, vShaderByteCode, "NormalMap_VS"))			return false;
	if (!setUp_NormalMap_InputLayout(device, vShaderByteCode))								return false;

	if (!LoadPixelShader(device, ps_normalMap, "NormalMap_PS"))								return false;

	if (!LoadVertexShader(device, vs_normalMap2, vShaderByteCode, "NormalMap_VS2"))			return false;

	if (!LoadComputeShader(device, pt_UpdatePlayer, "PT_UpdatePlayer"))						return false;
	if (!LoadHullShader(device, hullShader, "HullShader"))									return false;
	if (!LoadDomainShader(device, domainShader, "DomainShader"))							return false;
	if (!LoadComputeShader(device, postProcess, "PostProcess"))								return false;
	if (!LoadDomainShader(device, domainShader2, "DomainShader2"))							return false;
	if (!LoadHullShader(device, hullShader2, "Hullshader2"))								return false;


	SetViewport(viewport, GPU::windowWidth, GPU::windowHeight);
	SetViewport(shadowViewport, WidthAndHeight, WidthAndHeight);
	
	return true;
}


void BasicRenderer::setUpScene()
{

	if (halveraFps)
	{
		immediateContext->ClearRenderTargetView(rtv2, clearColour);
		immediateContext->OMSetRenderTargets(1, &rtv2, dsView);
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	}
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->RSSetState(shadowRastirizer);
	immediateContext->PSSetSamplers(1, 1 ,&shadowSampler);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	
	//Unbind shadowmap & structuredBuffer srv
	ID3D11ShaderResourceView* nullRsv{ nullptr };
	immediateContext->PSSetShaderResources(3, 1, &nullRsv);
	immediateContext->PSSetShaderResources(4, 1, &nullRsv);
}

void BasicRenderer::changeToAnimation(bool shadow)
{
	immediateContext->RSSetState(shadowRastirizer);
	immediateContext->PSSetSamplers(1, 1, &shadowSampler);

	if (shadow)
	{
		immediateContext->PSSetShader(pShader, nullptr, 0);
	}
	else
	{
		immediateContext->PSSetShader(pShader_NoShadow, nullptr, 0);
	}
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->IASetInputLayout(this->animLayout);
	immediateContext->VSSetShader(this->vShaderAnim, nullptr, 0);
}
void BasicRenderer::setUpScene(Camera& stageCamera, const bool& shadow)
{				
	if (halveraFps)
	{
		immediateContext->ClearRenderTargetView(rtv2, clearColour);
		immediateContext->OMSetRenderTargets(1, &rtv2, dsView);
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	}
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	shadow ? immediateContext->PSSetShader(pShader, nullptr, 0) : immediateContext->PSSetShader(pShader_NoShadow, nullptr, 0);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->PSSetShaderResources(5, 1, &depthSrv);
	immediateContext->PSSetSamplers(1, 1, &shadowSampler);
	stageCamera.PSbindPositionBuffer(1);
	stageCamera.VSbindViewBuffer(1);
}



void BasicRenderer::setUpSceneNormalMap(Camera& stageCamera)
{

	//immediateContext->IASetInputLayout(nullptr);
	//immediateContext->VSSetShader(nullptr, nullptr, 0);
	//immediateContext->PSSetShader(nullptr, nullptr, 0);
	//ID3D11RenderTargetView* nullRTV{ nullptr };
	//immediateContext->OMSetRenderTargets(1, &nullRTV, nullptr);
	//immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(inputLayout_NormalMap);
	immediateContext->VSSetShader(vs_normalMap, nullptr, 0);
	immediateContext->PSSetShader(ps_normalMap, nullptr, 0);


	//immediateContext->RSSetViewports(1, &viewport);
	//immediateContext->RSSetState(shadowRastirizer);
	//immediateContext->PSSetSamplers(0, 1, &sampler);

	//Unbind shadowmap & structuredBuffer srv
	//ID3D11ShaderResourceView* nullSRV{ nullptr };
	//immediateContext->PSSetShaderResources(3, 1, &nullSRV);
	//immediateContext->PSSetShaderResources(4, 1, &nullSRV);


	/*
{
	immediateContext->RSSetState(shadowRastirizer);

	if (shadow)
	{
		immediateContext->PSSetShader(pShader, nullptr, 0);
		immediateContext->PSSetShader(ps_normalMap, nullptr, 0);
	}
	else
	{
		immediateContext->PSSetShader(pShader_NoShadow, nullptr, 0);
	}

	immediateContext->IASetInputLayout(animLayout);
	immediateContext->IASetInputLayout(inputLayout_NormalMap);


	immediateContext->VSSetShader(vs_normalMap, nullptr, 0);
	immediateContext->VSSetShader(vShaderAnim, nullptr, 0);

	*/
}

void BasicRenderer::geometryPass(Camera& stageCamera)
{
	//re-use same depth buffer as geometry pass.
	immediateContext->OMSetBlendState(blendState, nullptr, 0xffffffffu);
	immediateContext->OMSetDepthStencilState(PT_dsState,0);							
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->CSSetShader(pt_UpdateShader, nullptr, 0);							//Set ComputeShader
	immediateContext->VSSetShader(pt_vShader, nullptr, 0);								//SetVTXShader
	immediateContext->PSSetShader(pt_pShader, nullptr, 0);								//Set PSShader
	immediateContext->GSSetShader(pt_gShader, nullptr, 0);								//SetGeoShader
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);		//Set how topology
	immediateContext->IASetInputLayout(pt_inputLayout);									//Input layout = float3 position for each vertex
	stageCamera.GSbindViewBuffer(0);													//Set matrix [world],[view]
	stageCamera.GSbindPositionBuffer(1);												//Set camera pos for 
	stageCamera.CSbindUpBuffer(2);
	stageCamera.GSbindUpBuffer(3);
	if (halveraFps)
	{
		immediateContext->OMSetRenderTargets(1, &rtv2, dsView);								//SetRtv
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &rtv, dsView);								//SetRtv
	}
}

void BasicRenderer::playerParticlePass()
{
	immediateContext->CSSetShader(pt_UpdatePlayer, nullptr, 0);							//Set ComputeShader
}

void BasicRenderer::depthPrePass()
{
	ID3D11RenderTargetView* nullRtv{ nullptr };
	immediateContext->ClearDepthStencilView(dsView2, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	if (halveraFps)
	{
		immediateContext->OMSetRenderTargets(1, &rtv2, dsView2);
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &rtv, dsView2);
	}
}

void BasicRenderer::depthUnbind()
{
	GPU::immediateContext->OMSetDepthStencilState(nullptr, 0);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->IASetInputLayout(inputLayout);
}

void BasicRenderer::skyboxPrePass()
{
	immediateContext->OMSetDepthStencilState(dsState, 0);
	immediateContext->IASetInputLayout(inputLayout_Skybox);		

	if (halveraFps)
	{
		immediateContext->OMSetRenderTargets(1, &rtv2, dsView);
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	}

	immediateContext->VSSetShader(vs_Skybox, nullptr, 0);								//SetVTXShader
	immediateContext->PSSetShader(ps_Skybox, nullptr, 0);								//Set PSShader
}

void BasicRenderer::bilboardPrePass(Camera& stageCamera)
{
	immediateContext->OMSetDepthStencilState(PT_dsState, 0);
	immediateContext->OMSetBlendState(blendState, nullptr, 0xffffffffu);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	immediateContext->IASetInputLayout(inputLayout_Skybox);
	immediateContext->VSSetShader(bill_vShader, nullptr, 0);
	immediateContext->PSSetShader(bill_pShader, nullptr, 0);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->GSSetShader(bill_gShader, nullptr, 0);
	stageCamera.GSbindViewBuffer(1);													//Set matrix [world],[view]
	stageCamera.GSbindPositionBuffer(2);												//Set camera pos for 
	stageCamera.GSbindUpBuffer(3);

	if (halveraFps)
	{
		immediateContext->OMSetRenderTargets(1, &rtv2, dsView);
	}
	else
	{
		immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	}
}

void BasicRenderer::bindAmbientShader()
{
	immediateContext->PSSetShader(ApShader, nullptr, 0);
}

void BasicRenderer::geometryUnbind()
{
	//Variables
	ID3D11GeometryShader* nullShader{ nullptr };
	ID3D11UnorderedAccessView* nullUav{ nullptr };
	ID3D11BlendState* nullBlendstate{ nullptr };
	ID3D11Buffer* nullbuffer{ nullptr };

	//Unbind shader & UAV, Reset Topology
	GPU::immediateContext->OMSetDepthStencilState(nullptr, 0);
	GPU::immediateContext->GSSetShader(nullShader, nullptr, 0);													//Unbinding
	GPU::immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);						//Reset Topology
	GPU::immediateContext->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);									//Unbind UAV
	GPU::immediateContext->OMSetBlendState(nullBlendstate, nullptr, 0xffffffffu);								//Unbind blendstate
	GPU::immediateContext->PSSetConstantBuffers(0, 1, &nullbuffer);
}

void BasicRenderer::fresnelPrePass(Camera& stageCamera)
{ 
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->OMSetBlendState(fresnelBlendState, nullptr, 0xffffffffu);
	immediateContext->OMSetDepthStencilState(PT_dsState, 0);
	immediateContext->PSSetShader(Fresnel_PS, nullptr, 0);
	stageCamera.PSbindPositionBuffer(1);
}

void BasicRenderer::invFresnelPrePass()
{
	immediateContext->PSSetShader(InvFresnel_PS, nullptr, 0);
}

void BasicRenderer::bindAnimVs()
{
	immediateContext->IASetInputLayout(this->animLayout);
	immediateContext->VSSetShader(this->vShaderAnim, nullptr, 0);
}

void BasicRenderer::fresnelAnimPrePass(Camera& stageCamera)
{
	immediateContext->IASetInputLayout(this->animLayout);
	immediateContext->VSSetShader(this->vShaderAnim, nullptr, 0);
	immediateContext->OMSetBlendState(fresnelBlendState, nullptr, 0xffffffffu);
	immediateContext->OMSetDepthStencilState(PT_dsState, 0);
	immediateContext->PSSetShader(Fresnel_PS, nullptr, 0);
	stageCamera.PSbindPositionBuffer(1);
}

void BasicRenderer::tesselationPrePass(Camera& stageCamera)
{
	immediateContext->VSSetShader(basic_VertexShader, nullptr, 0);
	immediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	immediateContext->HSSetShader(hullShader, nullptr, 0);
	immediateContext->DSSetShader(domainShader, nullptr, 0);
	stageCamera.DSbindViewBuffer(1);
}

void BasicRenderer::resetTopology()
{
	ID3D11HullShader* nullShader{ nullptr };
	ID3D11DomainShader* nullShader1{ nullptr };

	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->HSSetShader(nullShader, nullptr, 0);
	immediateContext->DSSetShader(nullShader1, nullptr, 0);
}

void BasicRenderer::postProcessPass()
{
	if (halveraFps)
	{
		ID3D11RenderTargetView* nullRtv{ nullptr };
		ID3D11UnorderedAccessView* nullUav{ nullptr };
		ID3D11ShaderResourceView* nullSrv{ nullptr };

		immediateContext->OMSetRenderTargets(1, &nullRtv, nullptr);

		immediateContext->CSSetShader(postProcess, nullptr, 0);

		immediateContext->CSSetShaderResources(0, 1, &srv2);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &backBufferUAV, nullptr);
		//immediateContext->CSSetSamplers(0, 1, &sampler);
		immediateContext->Dispatch(GPU::windowWidth, GPU::windowHeight, 1);
		immediateContext->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
		immediateContext->CSSetShaderResources(0, 1, &nullSrv);
		immediateContext->OMSetRenderTargets(1, &rtv, nullptr);
	}
	else
	{
		//std::cout << "Post process not run, FXAA turned off" << std::endl;
	}

}

void BasicRenderer::normaltasseletion(Camera& stageCamera, bool shadow)
{
	
	immediateContext->IASetInputLayout(inputLayout_NormalMap);
	immediateContext->VSSetShader(vs_normalMap2, nullptr, 0);
	immediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	immediateContext->HSSetShader(hullShader2, nullptr, 0);
	immediateContext->DSSetShader(domainShader2, nullptr, 0);
	if (!shadow)
	{
		immediateContext->PSSetShader(ps_normalMap, nullptr, 0);
	}
	stageCamera.DSbindViewBuffer(1);
}
