#include "stdafx.h"
#include "LightHandler.h"


//------------------------------------------------------------------------------- SETUP FUNCTIONS -------------------------------------------------------------------------------

bool CreateLtBuffer(ID3D11Device* device, StructuredBuffer<LightStruct>& lightBuffer, std::vector<Light>& lights)
{
	std::vector<LightStruct> structVector;
	for (int i = 0; i < lights.size(); i++)
	{
		//Get data
		DirectX::XMFLOAT3 position = lights.at(i).getPosition();
		DirectX::XMFLOAT3 color = lights.at(i).getColor();
		DirectX::XMFLOAT3 direction = lights.at(i).getDirection();

		//Change to XMFLOAT4
		DirectX::XMFLOAT4X4 matrix;
		DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(position.x, position.y, position.z, 0);
		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);
		DirectX::XMFLOAT4 dir = DirectX::XMFLOAT4(direction.x, direction.y, direction.z, 0);
		float ca = lights.at(i).getConeAngle();
		float typ = (float)lights.at(i).getType();
		float rng = lights.at(i).getRange();
		float fall = lights.at(i).getFalloff();
		DirectX::XMMATRIX tempMatrix = lights.at(i).getViewMatrix();
		XMStoreFloat4x4(&matrix, tempMatrix);

		//Create & push back struct
		LightStruct lightArray(pos, col, dir, ca, typ, rng, fall, matrix);
		structVector.push_back(lightArray);
	}

	lightBuffer.Initialize(GPU::device, GPU::immediateContext, structVector);
	lightBuffer.applyData();



	return true; //!FAILED(hr);
}


bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, Microsoft::WRL::ComPtr<ID3D11Texture2D>& dsTexture, std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>>& dsViews, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderView, int nrOfLights)
{
	int WidthAndHeight = 512;

	D3D11_TEXTURE2D_DESC textureDesc = {};											//skapa svartvit textur som representerar djup i en scen
	textureDesc.Width = WidthAndHeight;
	textureDesc.Height = WidthAndHeight;
	textureDesc.MipLevels = 1u;														//olika niv��er av kompression
	textureDesc.ArraySize = nrOfLights;												//en buffer
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;								// MapUsageTypless(usage);
	textureDesc.SampleDesc.Count = 1;												// defaultv�rden
	textureDesc.SampleDesc.Quality = 0;												//Sample quality
	textureDesc.Usage = D3D11_USAGE_DEFAULT;										//s�ger hur den ska anv�ndas n�r vi kommer �t den ()
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;	//binda till depth stencil
	textureDesc.CPUAccessFlags = 0;													// angeom cpu ska l�sa eller skriva (vi beh�ver varken eller s� d�rf�r 0)
	textureDesc.MiscFlags = 0;														//misc flagga


	//depth texture
	if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, dsTexture.GetAddressOf())))
	{
		std::cerr << "failed to create depth stencil texture" << std::endl;
		return false;
	}

	//DepthStencil views
	for (int i = 0; i < nrOfLights; i++)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.FirstArraySlice = i;
		descView.Texture2DArray.ArraySize = 1;


		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> tempDsView;
		//crate depth stencil, assign it to dsView
		if (FAILED(device->CreateDepthStencilView(dsTexture.Get(), &descView, tempDsView.GetAddressOf())))
		{
			std::cerr << "failed to create depth stencil texture" << std::endl;
			return false;
		}

		//push 
		dsViews.push_back(tempDsView);
	}


	//ShaderResource view 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;//DXGI_FORMAT_R24G8_TYPELESS
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc.Texture2DArray.ArraySize = nrOfLights;
	shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	shaderResourceViewDesc.Texture2DArray.MipLevels = 1u;
	shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;


	//create shader resource view 
	HRESULT hr = device->CreateShaderResourceView(dsTexture.Get(), &shaderResourceViewDesc, shaderView.GetAddressOf());
	return !FAILED(hr);
}


bool CreateNumLTBuffer(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11Buffer>& lightBuffer, int nrOfLights)
{
	int buff = nrOfLights;

	D3D11_BUFFER_DESC cBuffDesc = { 0 };
	cBuffDesc.ByteWidth = 16;													//size of buffer //Kolla senare funktion f�r att hitta n�rmaste multipel av 16 f�r int!
	cBuffDesc.Usage = D3D11_USAGE_DYNAMIC;										//sets interaction with gpu and cpu
	cBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;							//Specifies the type of buffer
	cBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//Specifies cpu acess
	cBuffDesc.MiscFlags = 0;													//Misc flags
	cBuffDesc.StructureByteStride = 0;											//Size of each element in structure
	D3D11_SUBRESOURCE_DATA cBufData = { 0 };									//holds matrix data
	cBufData.pSysMem = &buff;													//pointer to data


	HRESULT hr = device->CreateBuffer(&cBuffDesc, &cBufData, lightBuffer.GetAddressOf());
	return !FAILED(hr);
}


bool CreateViewBuffer(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11Buffer>& viewBuffer, Light& light)
{
	DirectX::XMMATRIX view = { light.getViewMatrix() };

	D3D11_BUFFER_DESC cBuffDesc;
	cBuffDesc.ByteWidth = sizeof(DirectX::XMMATRIX);		//size of buffer
	cBuffDesc.Usage = D3D11_USAGE_DYNAMIC;					//sets interaction with gpu and cpu
	cBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;		//Specifies the type of buffer
	cBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		//Specifies cpu acess
	cBuffDesc.MiscFlags = 0;								//Misc flags
	cBuffDesc.StructureByteStride = 0;						//Size of each element in structure
	D3D11_SUBRESOURCE_DATA cBufData;						//holds matrix data
	cBufData.pSysMem = &view;								//pointer to geometry data

	HRESULT hr = device->CreateBuffer(&cBuffDesc, &cBufData, viewBuffer.GetAddressOf());
	return !FAILED(hr);
}

//------------------------------------------------------------------------------- CONSTRUCTOR -------------------------------------------------------------------------------


LightHandler::LightHandler()
	:shadowHeight(GPU::windowHeight), shadowWidth(GPU::windowWidth)
{
	//Create depth stencil, textureArr, depthViews & shader resource views 
	if (!CreateDepthStencil(GPU::device, this->shadowWidth, this->shadowHeight, this->depthTextures, this->depthViews, this->shadowSrv, this->LightCap))
	{
		std::cerr << "error creating Dstencil/Srv!" << std::endl;
	}
	debugMesh = new Mesh("../Meshes/Sphere");
}

LightHandler::~LightHandler()
{
	for (int i = 0; i < boundingSphere.size(); i++)
	{
		if (boundingSphere.at(i) != nullptr)
		{
			delete boundingSphere.at(i);
		}
	}
	delete debugMesh;
}

// ------------------------------------------------------------------------------- FUNCTIONS -------------------------------------------------------------------------------

void LightHandler::addLight(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &color, const DirectX::XMFLOAT3 &direction, const DirectX::XMFLOAT3 &UpDir, int type, float coneAngle, float range, float falloff)
{
	//Check if light limit has been reached
	if (this->lights.size() < this->LightCap)
	{
		//Skapa ljus
		this->lights.push_back(Light(color, position, direction, UpDir, coneAngle, type, range, falloff));


		//current light id
		int lightID = (int)this->lights.size();
		

		//For first light create buffer, else unmap/remap buffer with new info
		if (lightID == 1)
		{
			//Create structured buffer containing all light data
			if (!CreateLtBuffer(GPU::device, this->lightBuffer, this->lights))
			{
				std::cout << "error creating lightBuffer!" << std::endl;
			}


			//Create buffer with int for nrof lights.
			if (!CreateNumLTBuffer(GPU::device, this->numLightBuffer, lightID))
			{
				std::cerr << "error creating numlightBuffer!" << std::endl;
			}

		}
		else
		{
			//Unmap structured buffer & numlight buffer
			this->updateBuffers();
		}

		//create& push back new viewMatrix buffer
		Microsoft::WRL::ComPtr <ID3D11Buffer> tempBuffer;
		if (!CreateViewBuffer(GPU::device, tempBuffer, this->lights.at(lightID - 1)))
		{
			std::cerr << "error creating viewBuffer!" << std::endl;
		}
		this->viewBuffers.push_back(tempBuffer);

		//Create Debug Mesh



		this->boundingSphere.push_back(new GameObject(debugMesh, position, direction, lightID));
		this->boundingSphere.back()->updateBuffer();

	}
	else
	{
		std::cout << "Light limit reached! failed creating light" << std::endl;
	}
}

bool LightHandler::updateBuffers()
{

	//---------------------------------------- Structured Buffer ----------------------------------------
	std::vector<LightStruct> structVector;
	for (int i = 0; i < lights.size(); i++)
	{
		//Get data
		DirectX::XMFLOAT3 position = lights.at(i).getPosition();
		DirectX::XMFLOAT3 color = lights.at(i).getColor();
		DirectX::XMFLOAT3 direction = lights.at(i).getDirection();

		//Change to XMFLOAT4
		DirectX::XMFLOAT4X4 matrix;
		DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(position.x, position.y, position.z, 0);
		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);
		DirectX::XMFLOAT4 dir = DirectX::XMFLOAT4(direction.x, direction.y, direction.z, 0);
		float ca = lights.at(i).getConeAngle();
		float typ = (float)lights.at(i).getType();
		float rng = lights.at(i).getRange();
		float fall = lights.at(i).getFalloff();
		DirectX::XMMATRIX tempMatrix = lights.at(i).getViewMatrix();
		XMStoreFloat4x4(&matrix, tempMatrix);

		//Create & push back struct
		LightStruct lightArray(pos, col, dir, ca, typ, rng, fall, matrix);
		structVector.push_back(lightArray);
	}

	//Unmap/map Buffer
	this->lightBuffer.remapBuffer(GPU::device, GPU::immediateContext, structVector);
	this->lightBuffer.applyData();


	//---------------------------------------- NrLight Buffer ----------------------------------------

	//Ger nrOf lights
	int nrOfLights = (UINT)this->lights.size();

	//Map
	D3D11_MAPPED_SUBRESOURCE map;
	ZeroMemory(&map, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = GPU::immediateContext->Map(this->numLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &nrOfLights, 16); //kanske ger problem!!!

	//UnMap
	GPU::immediateContext->Unmap(this->numLightBuffer.Get(), 0);
	return !FAILED(hr);
}

void LightHandler::setPosition(const DirectX::XMFLOAT3 &position, const int &lightIndex)
{
	this->lights.at(lightIndex).setPosition(position);
	
	//Matrix update
	updateViewMatrix(lightIndex);

	//Mesh update
	this->boundingSphere.at(lightIndex)->setPos(position);
}

void LightHandler::setDirection(const DirectX::XMFLOAT3 &direction, const int& lightIndex)
{
	this->lights.at(lightIndex).setDirection(direction);

	//Matrix update
	updateViewMatrix(lightIndex);

	//Mesh update
	this->boundingSphere.at(lightIndex)->setRot(direction);
}

void LightHandler::setUpDirection(const DirectX::XMFLOAT3 &direction, const int& lightIndex)
{
	this->lights.at(lightIndex).setUpDirection(direction);

	//Matrix update
	updateViewMatrix(lightIndex);
}

void LightHandler::setColor(const DirectX::XMFLOAT3 &color, const int& lightIndex)
{
	this->lights.at(lightIndex).setColor(color);
}

void LightHandler::setConeAngle(const float &angle, const int& lightIndex)
{
	this->lights.at(lightIndex).setConeAngle(angle);
}

void LightHandler::setLightType(const int &type, const int& lightIndex)
{
	this->lights.at(lightIndex).setLightType(type);
}

void LightHandler::setRange(const float &range, const int& lightIndex)
{
	this->lights.at(lightIndex).setRange(range);
}

void LightHandler::setFalloff(const float &falloff, const int& lightIndex)
{
	this->lights.at(lightIndex).setFalloff(falloff);
}

bool LightHandler::updateViewMatrix(const int &lightIndex)
{
	//---------------------------------------- View Buffer ----------------------------------------

	//Update Debug meshes
	for (int i = 0; i < this->boundingSphere.size(); i++)
	{
		this->boundingSphere.at(i)->updateBuffer();
	}

	//Get matrix
	DirectX::XMMATRIX view = this->lights.at(lightIndex).getViewMatrix();

	//Map
	D3D11_MAPPED_SUBRESOURCE map;
	ZeroMemory(&map, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HRESULT hr = GPU::immediateContext->Map(this->viewBuffers.at(lightIndex).Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	memcpy(map.pData, &view, sizeof(DirectX::XMMATRIX));

	//UnMap
	GPU::immediateContext->Unmap(this->viewBuffers.at(lightIndex).Get(), 0);
	return !FAILED(hr);
}

ID3D11Buffer* LightHandler::getViewBuffer(const int &ltIndex) const
{
	return this->viewBuffers.at(ltIndex).Get();
}

int LightHandler::getNrOfLights() const
{
	return (UINT)this->lights.size();
}


void LightHandler::drawShadows(const int &lightIndex, const std::vector<GameObject*> &gameObjects, Camera* stageCamera)
{
	//Variables
	ID3D11RenderTargetView* nullRtv{ nullptr };

	if (stageCamera != nullptr)
	{
		//Set view buffer
		stageCamera->VSbindViewBuffer(1);
	}
	else
	{
		//Set view buffer
		GPU::immediateContext->VSSetConstantBuffers(1, 1, this->viewBuffers.at(lightIndex).GetAddressOf());

		//Clear Depth Stencil
		GPU::immediateContext->ClearDepthStencilView(this->depthViews.at(lightIndex).Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

		//Set render targets
		GPU::immediateContext->OMSetRenderTargets(1, &nullRtv, this->depthViews.at(lightIndex).Get());
	}

	//Draw Objects
	for (int i = 0; i < gameObjects.size(); i++)	
	{
			gameObjects.at(i)->draw();
	}			
}

void LightHandler::drawShadows(const int& lightIndex, const std::vector<Player*>& gameObjects, Camera* stageCamera)
{
	//Variables
	ID3D11RenderTargetView* nullRtv{ nullptr };
	ID3D11DepthStencilView* nullDsView{ nullptr };

	if (stageCamera != nullptr)
	{
		//Set view buffer
		stageCamera->VSbindViewBuffer(1);
	}
	else
	{
		//Set view buffer
		GPU::immediateContext->VSSetConstantBuffers(1, 1, this->viewBuffers.at(lightIndex).GetAddressOf());

		//Set render targets
		GPU::immediateContext->OMSetRenderTargets(1, &nullRtv, this->depthViews.at(lightIndex).Get());
	}

	//Draw Objects
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->draw();
	}

	//Unbind render targets & Depth Stencil
	GPU::immediateContext->OMSetRenderTargets(1, &nullRtv, nullDsView);
}

void LightHandler::drawShadows(const int& lightIndex, const std::vector<Planet*>& planets, Camera* stageCamera)
{
	//Variables
	ID3D11RenderTargetView* nullRtv{ nullptr };
	ID3D11DepthStencilView* nullDsView{ nullptr };

	if (stageCamera != nullptr)
	{
		//Set view buffer
		stageCamera->DSbindViewBuffer(1);
	}
	else
	{
		//Set view buffer
		GPU::immediateContext->DSSetConstantBuffers(1, 1, this->viewBuffers.at(lightIndex).GetAddressOf());

		//Set render targets
		GPU::immediateContext->OMSetRenderTargets(1, &nullRtv, this->depthViews.at(lightIndex).Get());
	}


	//Draw Objects
	for (int i = 0; i < planets.size(); i++)
	{
		planets[i]->drawObjectWithNormalMap();
	}
}

void LightHandler::bindLightBuffers()
{
	GPU::immediateContext->PSSetShaderResources(3, 1, this->shadowSrv.GetAddressOf());				//Bind Srv's //ShadowMap(s)
	this->lightBuffer.BindToPS(4);																	//Srv for light structuredBuffer content (pos, color, lightViewMatrix)
	GPU::immediateContext->PSSetConstantBuffers(2, 1, this->numLightBuffer.GetAddressOf());			//Bind CBuffers's //Buffer for nr Lights
}

void LightHandler::drawDebugMesh()
{
	for (int i = 0; i < this->boundingSphere.size(); i++)
	{
		this->boundingSphere.at(i)->draw();
	}
}

void LightHandler::unbindSrv()
{
	//Unbind shadowmap & structuredBuffer srv
	ID3D11ShaderResourceView* nullsrv{ nullptr };
	GPU::immediateContext->PSSetShaderResources(3, 1, &nullsrv);
	GPU::immediateContext->PSSetShaderResources(4, 1, &nullsrv);
	GPU::immediateContext->PSSetShaderResources(5, 1, &nullsrv);
}
