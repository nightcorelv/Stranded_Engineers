#pragma once
#include "GameObject.h"
#include "Player.h"


class HazardBox : public GameObject
{
private:
	ParticleEmitter *particles;
	ConstantBufferNew<DirectX::XMFLOAT4> fresnelBuffer;	//fresnel color buffer

public:
	HazardBox(Mesh* useMesh, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, GravityField* field = nullptr, const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1, 1, 1));
	~HazardBox();
	void CheckZap(Player * playerToCheck);
	void drawParticles();
	void drawFresnel(); //for drawing on ring submesh
};
