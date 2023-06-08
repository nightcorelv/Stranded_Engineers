#pragma once
#include <wrl.h>
#include <vector>
#include <iostream>
#include <d3d11.h>
#include <DirectXMath.h>
#include "GPU.h"
#include "ConstantBufferNew.h"
#include "TimeStruct.h"


class ParticleEmitter
{
private:
	std::vector < Microsoft::WRL::ComPtr<ID3D11Texture2D>> PT_texture;					//Textures
	std::vector < Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> PT_TXView;			//Texture views
	Microsoft::WRL::ComPtr <ID3D11UnorderedAccessView> PT_UAV;			//UAV for vertex shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> PT_vertexBuffer;				//Vertex buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> emitterPosBuffer;				//Position buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> timeBuffer;					//Time buffer, contains delta time
	ConstantBufferNew<DirectX::XMFLOAT4> colorBuffer;
	ConstantBufferNew<DirectX::XMFLOAT4> sizeBuffer;

	TimeStruct tStruct;
	std::vector<particleStruct> PT_Data;								//Particle Data (pos, delta time, lifetime)//Add speed?
	DirectX::XMFLOAT2 minMaxLifetime;									//Min and max lifetime of particles
	DirectX::XMFLOAT3 Position;											//Emitter Position
	DirectX::XMFLOAT3 Rotation;											//Emitter Rotation
	int nrOfParticles;													//Nr of points in buffer
	bool active;														//Particle emitter state
	bool renderPassComplete;											//Particles need to render one time after switched to inactive
	float speed;

public:

	ParticleEmitter(const DirectX::XMFLOAT3 &Pos, const DirectX::XMFLOAT3 &Rot, const int &nrOfPT, const DirectX::XMFLOAT2 &minMaxLifetime, int randRange = 10, float speed = 0.07f);
	void BindAndDraw(int textureIndex);
	void updateBuffer();												//Updates position, rotation & state (emitter on or off)
	ID3D11Buffer* getVTXBuffer() const;
	ID3D11Buffer* getPosBuffer() const;
	ID3D11UnorderedAccessView* getUAV() const;
	int getNrOfParticles() const;
	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT3 getRotation() const;
	DirectX::XMFLOAT3 getColor();
	float getSpeed() const;
	void setSpeed(const float & speed);
	void setPosition(const DirectX::XMFLOAT3 &Pos);
	void setRotation(const DirectX::XMFLOAT3 &Rot);
	void setActive(const bool &onOrOff);
	void setPassComplete(const bool &onOrOff);
	void setColor(const DirectX::SimpleMath::Vector3& color);
	void setColor(const float &x, const float &y, const float &z);
	void setSize(const float& size);
	bool isActive();
	bool isPassComplete();

private:

	void updateTimeBuffer(const float &delta);
};