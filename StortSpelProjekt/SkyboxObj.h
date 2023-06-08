#pragma once
//#include<stdafx.h>
#include <wrl.h>
#include <vector>
#include <iostream>
#include "GPU.h"
#include "GameObject.h"

class SkyboxObj
{
private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> textureCubes;
	std::vector < Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureCubeViews;
	GameObject* skyBoxObj;
	Mesh* skyBoxMesh;
public:

	SkyboxObj();
	~SkyboxObj();
	bool AddSkybox(int skyBoxFolderIndex, std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>>& textureCubes, std::vector < Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textureCubeViews);
	void draw(int index = 0);
};