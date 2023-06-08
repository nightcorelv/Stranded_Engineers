#pragma once
#include "GameObject.h"
#include "ParticleEmitter.h"
#include "BilboardObject.h"
#include "TimeStruct.h"
#include "Sound.h"
#include <GamePad.h>

class Player;

class Item : public GameObject
{
private:
	int onlineType;
	int onlineId;

protected:
	TimeStruct tStruct;
	BilboardObject* itemIcon;
	ParticleEmitter* particles;
	ConstantBufferNew<DirectX::XMFLOAT4> fresnelBuffer;	//fresnel color buffer
	DirectX::SimpleMath::Vector3 color;
	Sound sfx;
	bool withinPlayerReach;
	bool pickedUp;
	DirectX::GamePad* gamePad = nullptr;
	float pickUpRange = 8.0f;
public:
	Item(Mesh* useMesh, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const int& id, const int& onlineId, const int& onlineType, GravityField* field = nullptr, DirectX::GamePad* gamePad = nullptr);
	Item(const std::string& objectPath, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const int& id, const int& onlineId, const int& onlineType, GravityField* field = nullptr, DirectX::GamePad * gamePad = nullptr);
	virtual ~Item();
	virtual void useItem(const Player* playerHoldingItem) = 0;
	virtual void drawIcon();
	virtual void drawParticles();
	void setPickedUp(bool pickedUp);
	void checkDistance(GameObject* playerTocheck);
	//virtual void useItem();
	void throwItem();
	void setGamepad(DirectX::GamePad*& gamePad);
	int getOnlineType()const;
	int getOnlineId()const;
	float getPickUpRange() const;
	virtual void update() override;
	virtual void drawFresnel();
	virtual void draw() override;
};