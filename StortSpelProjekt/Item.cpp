#include "stdafx.h"
#include "Item.h"
#include "Time.h"


Item::Item(Mesh* useMesh, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const int& id, const int& onlineId, const int& onlineType, GravityField* field, DirectX::GamePad* gamePad)
	:GameObject(useMesh, pos, rot, id, field), pickedUp(false), itemIcon(nullptr), particles(nullptr), withinPlayerReach(false), onlineId(onlineId),onlineType(onlineType), gamePad(gamePad)
{
	//Initilize timer
	tStruct.startTime;
}


Item::Item(const std::string& objectPath, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const int& id, const int& onlineId, const int& onlineType, GravityField* field, DirectX::GamePad* gamePad)
	:GameObject(objectPath, pos, rot, id, field), pickedUp(false), itemIcon(nullptr), particles(nullptr), withinPlayerReach(false), onlineId(onlineId), onlineType(onlineType),gamePad(gamePad)
{
	//Initilize timer
	tStruct.startTime;
}

Item::~Item()
{
	if (this->itemIcon != nullptr)
	{
		delete this->itemIcon;
	}
	if (this->particles != nullptr)
	{
		delete this->particles;
	}
}



void Item::drawIcon()
{
	if (this->itemIcon != nullptr && !pickedUp)
	{
		if (withinPlayerReach && this->gamePad == nullptr)
		{
			this->itemIcon->bindAndDraw(2, 0);
		}
		else if (this->gamePad != nullptr && withinPlayerReach)
		{
			auto state = this->gamePad->GetState(0);
			if ( state.IsConnected())
			{
				this->itemIcon->bindAndDraw(3, 0);
			}
			else
			{
				this->itemIcon->bindAndDraw(2, 0);
			}
		}
		else
		{
			this->itemIcon->bindAndDraw(0, 0);
		}
	}
}

void Item::setPickedUp(bool pickedUp)
{
	this->pickedUp = pickedUp;
}

void Item::drawParticles()
{
	if (this->particles != nullptr && pickedUp)
	{
		if (tStruct.getDt() < 1)
		{	
			this->particles->setColor(1, 1, 1);
			this->particles->setSize(3.0f);
			this->particles->BindAndDraw(0);
		}
	}
	else if (!this->pickedUp)
	{
		this->particles->setColor(color);
		this->particles->setSize(1.0f);
		this->particles->BindAndDraw(4);
	}
}

void Item::checkDistance(GameObject* playerToCheck)
{
	if (this->withinRadious(playerToCheck, pickUpRange))
	{
		this->withinPlayerReach = true;
	}
	else
	{
		this->withinPlayerReach = false;
	}
}

void Item::throwItem()
{
}

void Item::setGamepad(DirectX::GamePad*& gamePad)
{
	this->gamePad = gamePad;
}

float Item::getPickUpRange() const
{
	return this->pickUpRange;
}

void Item::draw()
{

	//if touching planet && not held
	if (!pickedUp)
	{
		////timer const
		//static float ttest = 0;
		//ttest += Time::DeltaTimeInSeconds() * 0.3f;

		////Store pos to reset to
		//DirectX::XMFLOAT3 storedPos = this->position;

		////Get rotation oriented to gravity
		//DirectX::XMFLOAT3 rotation = this->getRotOrientedToGrav();

		////create matrix from rot oriented to gravity
		//DirectX::XMMATRIX rotn = DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, 1.0f));
		//

		////set rot to 0 & pos to origo
		//this->setRot(DirectX::XMFLOAT3(0, 0, 0));
		//this->setPos(0, 0, 0);


		////add constant
		//this->setRot(DirectX::XMFLOAT3(0 + ttest, 0, 0));





		//DirectX::XMVECTOR scl;
		//DirectX::XMVECTOR rt;
		////multiply 
		//DirectX::XMMATRIX rotnn( rotn * this->rotation);
		//DirectX::XMMatrixDecompose(&scl, &rt, &scl,rotnn);

		//DirectX::XMFLOAT3 finalRot;
		//DirectX::XMStoreFloat3(&finalRot, rt);



		////set variables
		//this->setRot(finalRot);
		//this->setPos(storedPos);


		////std::cout << "this rot X: " << this->getRotXM().x << " Y: " << this->getRotXM().y << " Z: " << this->getRotXM().z << std::endl;
		////std::cout << "grav rot X: " << rotation.x << " Y: " << rotation.y << " Z: " << rotation.z << std::endl;


		//Set position
		float constant = 2.0f;
		DirectX::XMFLOAT3 upDir = this->getUpDirection();
		DirectX::XMFLOAT3 offsetPos(upDir.x * constant, upDir.y * constant, upDir.z * constant);


		DirectX::SimpleMath::Vector3 test = this->position + offsetPos;
		this->mesh->UpdateCB(test, this->rotation, this->scale);
		this->mesh->DrawWithMat();

		//this->mesh->position = offsetPos;
		//mesh->position = mesh->position + offsetPos;
		//std::cout << "itpos X: " << this->position.x << " Y: " << this->position.y << " Z: " << this->position.z << std::endl;
		//std::cout << "ofset X: " << offsetPos.x << " Y: " << offsetPos.y <<	 " Z: " << offsetPos.z << std::endl;
	}
	else
	{
		this->mesh->UpdateCB(position, rotation, scale);
		this->mesh->DrawWithMat();
	}

}

int Item::getOnlineType() const
{
	return onlineType;
}

int Item::getOnlineId() const
{
	return onlineId;
}

void Item::update()
{
	//Update movement
	this->updateRotation();

	//Update icon movement
	if (this->itemIcon != nullptr)
	{
		float constant = itemIcon->getOffset();
		DirectX::XMFLOAT3 upDir = this->getUpDirection();
		DirectX::XMFLOAT3 itemPos(upDir.x * constant, upDir.y * constant, upDir.z * constant);		
		this->itemIcon->setPosition(this->position + itemPos);
	}

	//Update particle movement
	if (this->particles != nullptr && !this->pickedUp)
	{
		tStruct.resetStartTime();
		DirectX::XMFLOAT3 rot= this->getRotOrientedToGrav();
		this->particles->setPosition(this->position);
		this->particles->setRotation(this->getUpDirection());
		this->particles->updateBuffer();
	}


	//Update mesh movement
	



	/*
	
	void GameObject::draw()
{
	this->mesh->UpdateCB(position, rotation, scale);
	this->mesh->DrawWithMat();
}

	*/
	/*
		//Set rotation to gravity field
	this->setRot(this->getRotOrientedToGrav());

	//Icon initiation
	float constant = 60.0f;

	DirectX::XMFLOAT3 upDir = this->getUpDirection();
	DirectX::XMFLOAT3 test(upDir.x * constant, upDir.y * constant, upDir.z * constant);
	this->rocketStatusQuad = new BilboardObject(filenames, test);
	this->rocketStatusQuad->setOffset(constant);

	//CapZone
	float constant2 = 40.0f;
	DirectX::XMFLOAT3 offset2(upDir.x * constant2, upDir.y * constant2, upDir.z * constant2);
	zone = new CaptureZone(zoneMesh, offset2, this->getRotXM(),field, DirectX::XMFLOAT3(8, 8, 8));
	*/
}

void Item::drawFresnel()
{
	if (!this->pickedUp)
	{
		float constant = 0.2f;
		GPU::immediateContext->PSSetConstantBuffers(2, 1, this->fresnelBuffer.getReferenceOf());
		DirectX::XMFLOAT3 test = this->scale;
		this->scale = DirectX::XMFLOAT3(test.x + constant, test.y + constant, test.z + constant);
		this->draw();
		this->scale = test;
	}
}
