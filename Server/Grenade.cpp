#include "Grenade.h"
#include "ObjectId.h"


Grenade::Grenade(const int& onlineId)
	:Item(onlineId, ObjID::GRENADE), force(0.f)
{
	this->destructionIsImmenent = false;
}

Grenade::~Grenade()
{

}

bool Grenade::explode()
{
	return (this->destructionIsImmenent && this->timer.getTimePassed(timeToExplode));
}

void Grenade::resetExplosion()
{
	this->destructionIsImmenent = false;
	this->explosionScale = 0.1f;
}

void Grenade::updateExplosion(float delta)
{
	//float delta = Time::DeltaTimeInSeconds();
	//float delta = 0.0001f;
	if (explosionScale < 27)
	{
		explosionScale = (explosionScale + (delta * 50));
	}
}

float Grenade::getScaleExplosion() const
{
	return this->explosionScale;
}

void Grenade::use(const Player* playerHoldingItem)
{
	std::cout << "Used grenade\n";
	this->destructionIsImmenent = true;
	timer.resetStartTime();
}
