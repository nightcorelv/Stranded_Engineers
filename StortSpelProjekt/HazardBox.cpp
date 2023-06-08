#include "stdafx.h"
#include "HazardBox.h"
#include "HudUI.h"

HazardBox::HazardBox(Mesh* useMesh, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, GravityField* field, const DirectX::XMFLOAT3& scale)
	:GameObject(useMesh, pos, rot, HAZARD, field, scale)
{
	//Set rotation to gravity field
	this->setRot(this->getRotOrientedToGrav());

	//Particles
	this->particles = new ParticleEmitter(pos, this->getRotOrientedToGrav(), 26, DirectX::XMFLOAT2(2, 5), 2);
	this->particles->setColor(1, 1, 1);
	this->particles->setSize(3.0f);

	//Color
	DirectX::SimpleMath::Vector3 color = DirectX::Colors::Cyan.v;

	//Set up Fresnel buffer
	fresnelBuffer.Initialize(GPU::device, GPU::immediateContext);
	fresnelBuffer.getData() = DirectX::XMFLOAT4(color.x, color.y, color.z, 0.1f);
	fresnelBuffer.applyData();
}

HazardBox::~HazardBox()
{
	if (this->particles != nullptr)
	{
		delete this->particles;
	}
}

void HazardBox::CheckZap(Player * playerToCheck)
{
	float constant = 5;
	if (this->withinBox(playerToCheck, constant, constant, constant))
	{
		std::cout << "player zapped" << std::endl;
	}
}

void HazardBox::drawParticles()
{
	//Update particle movement
	if (this->particles != nullptr)
	{
		this->particles->setPosition(this->position);
		this->particles->setRotation(this->getUpDirection());
		this->particles->updateBuffer();
	}

	if (this->particles != nullptr)
	{
			this->particles->BindAndDraw(0);//Add lightning particle
	}
}

void HazardBox::drawFresnel()
{
	//Get scale
	float constant = 0.5f;
	DirectX::XMFLOAT3 origScl =	this->getScale();
	DirectX::XMFLOAT3 newScl = { origScl.x + constant, origScl.y + constant, origScl.z + constant };

	//Apply new scale & draw
	GPU::immediateContext->PSSetConstantBuffers(2, 1, fresnelBuffer.getReferenceOf());
	this->mesh->UpdateCB(this->position, this->rotation, newScl);
	this->mesh->DrawWithMat(1);

	//Reset scale
	this->scale = origScl;
}
