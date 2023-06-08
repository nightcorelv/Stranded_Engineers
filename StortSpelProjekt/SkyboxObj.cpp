#include "stdafx.h"
#include "SkyboxObj.h"
#include "stb_image.h"


SkyboxObj::SkyboxObj()
{
	//Initiates with random skybox
	int skyboxIndex = std::rand() % 7;

	//Create SRV's & textures
	if (!this->AddSkybox(6, textureCubes, textureCubeViews))
	{
		std::cerr << "error creating Skybox RenderTargets!" << std::endl;
	}
	this->skyBoxMesh = new Mesh("../Meshes/Skybox2");
	this->skyBoxObj = new GameObject(skyBoxMesh, DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 0, 0), 0);
}

bool SkyboxObj::AddSkybox(int skyBoxFolderIndex, std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> &textureCubes, std::vector < Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> &textureCubeViews)
{
	//variables
	std::string skybox = "Skybox/" + std::to_string(skyBoxFolderIndex) + "/";
	std::vector<std::string> filenames{ skybox + "0.png", skybox + "1.png", skybox + "2.png", skybox + "3.png", skybox + "4.png", skybox + "5.png" };// "boxTest.jpg", "boxTest.jpg", "boxTest.jpg", "boxTest.jpg", "boxTest.jpg", "boxTest.jpg" 
	std::vector<unsigned char*> images;
	int wth = 0;
	int hth = 0;
	int channels = 0;

	for (int i = 0; i < 6; i++)
	{
		unsigned char* img = stbi_load(("../Textures/" + filenames.at(i)).c_str(), &wth, &hth, &channels, STBI_rgb_alpha);
		if (img == NULL)
		{
			return 0;
		}
		else
		{
			images.push_back(img);
		}

	}


	D3D11_TEXTURE2D_DESC textureDesc = {};												//skapa svartvit textur som representerar djup i en scen
	textureDesc.Width = wth;
	textureDesc.Height = hth;
	textureDesc.MipLevels = 1u;															//olika nivååer av kompression
	textureDesc.ArraySize = 6;												//en buffer
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;									//MapUsageTypless(usage);
	textureDesc.SampleDesc.Count = 1;													//Defaultvärden
	textureDesc.SampleDesc.Quality = 0;													//Sample quality
	textureDesc.Usage = D3D11_USAGE_DEFAULT;											//säger hur den ska användas när vi kommer åt den ()
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	//binda till depth stencil
	textureDesc.CPUAccessFlags = 0;														// angeom cpu ska läsa eller skriva (vi behöver varken eller så därför 0)
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;							//misc flagga


	D3D11_SUBRESOURCE_DATA data[6];
	for (int i = 0; i < 6; i++)
	{
		data[i].pSysMem = images[i];
		data[i].SysMemPitch = wth * 4;
		data[i].SysMemSlicePitch = 0;
	}


	//create texture
	ID3D11Texture2D* tempTex = nullptr;
	if (FAILED(GPU::device->CreateTexture2D(&textureDesc, data, &tempTex)))
	{
		std::cerr << "failed to create texture" << std::endl;
		return false;
	}
	textureCubes.push_back(tempTex);

	//ShaderResource view 
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.TextureCube.MipLevels = 1;
	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;

	//create shader resource view 
	ID3D11ShaderResourceView* tempSrv = nullptr;
	HRESULT hr = GPU::device->CreateShaderResourceView(tempTex, &shaderResourceViewDesc, &tempSrv);
	textureCubeViews.push_back(tempSrv);

	for (int i = 0; i < 6; i++)
	{
		stbi_image_free(images[i]);
	}

	return !FAILED(hr);
}

SkyboxObj::~SkyboxObj()
{
	delete this->skyBoxObj;
	delete this->skyBoxMesh;
}

void SkyboxObj::draw(int index)
{
	ID3D11ShaderResourceView* nullsrv{ nullptr };
	GPU::immediateContext->PSSetShaderResources(4, 1, textureCubeViews[index].GetAddressOf());
	this->skyBoxObj->draw();
	GPU::immediateContext->PSSetShaderResources(4, 1, &nullsrv);
}

