#pragma once
#include "Item.h"
#include "TimeStruct.h"

class Grenade : public Item
{
private:
	float force;
	TimeStruct timer;
	bool destructionIsImmenent;
	float explosionScale;
	float timeToExplode = 2.0f;
public:
	Grenade(const int& onlineId);
	~Grenade();
	bool explode();
	void resetExplosion();
	void updateExplosion(float delta);
	float getScaleExplosion()const;

	virtual void use(const Player* playerHoldingItem) override;
};