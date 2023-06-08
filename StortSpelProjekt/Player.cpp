#include "stdafx.h"
#include "PhysicsComponent.h"
#include "Player.h"
#include "DirectXMathHelper.h"
#include "BaseballBat.h"
#include "Component.h"
#include "PacketEnum.h"
#include "HudUI.h"
#include "Mesh.h"
#include "SpaceShip.h"
#include "Grenade.h"
using namespace DirectX;
using ButtonState = DirectX::GamePad::ButtonStateTracker::ButtonState;

//----------------------------------------------- setUp Functions ------------------------------------------------//

void Player::throwItem()
{
	//Calculate the force vector
	DirectX::XMFLOAT3 temp{0,0,0};

	if (this->holdingItem->getId() == ObjID::COMPONENT)
	{
	}
	else
	{
		DirectX::XMStoreFloat3(&temp, (this->forwardVector * 5.f + this->normalVector * 0.5f));
		newNormalizeXMFLOAT3(temp);
		if (this->moveKeyPressed)
		{
			if (this->currentSpeed == this->speed) scalarMultiplicationXMFLOAT3(this->currentSpeed * 0.0075f, temp);
			else scalarMultiplicationXMFLOAT3(this->currentSpeed * 0.085f, temp);
		}
	}

	ItemThrow throwStruct;
	throwStruct.itemId = this->holdingItem->getOnlineId();
	throwStruct.packetId = PacketType::ITEMTHROW;
	throwStruct.xForce = temp.x * FORCE;
	throwStruct.yForce = temp.y * FORCE;
	throwStruct.zForce = temp.z * FORCE;
	throwStruct.playerId = this->onlineID;
	throwStruct.xPos = this->holdingItem->getPosV3().x;
	throwStruct.yPos = this->holdingItem->getPosV3().y;
	throwStruct.zPos = this->holdingItem->getPosV3().z;

	//sending data to server
	if (client != nullptr)
	{
		client->sendStuff<ItemThrow>(throwStruct);
	}


	//Set dynamic so it can be affected by forces
	this->holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::DYNAMIC);
	//Apply the force
	this->holdingItem->getPhysComp()->applyForceToCenter(reactphysics3d::Vector3(temp.x * FORCE, temp.y * FORCE, temp.z * FORCE));

	//You no longer "own" the item
	holdingItem->setPickedUp(false);
	holdingItem = nullptr;
}

void Player::resetRotationMatrix()
{
	this->rotationMX = DirectX::XMMatrixIdentity();
	this->rotation = DirectX::XMMatrixIdentity();
}

void Player::handleItems()
{
	//DirectX::SimpleMath::Vector3 newPos = this->position;
	//newPos += 4 * forwardVector;
	if (this->gamePad == nullptr) return;

	DirectX::XMFLOAT4X4 f1;
	this->forwardKinematics("Character_RightHand", f1);
	DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&f1);
	DirectX::XMVECTOR scale;
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR rot;
	DirectX::XMMatrixDecompose(&scale, &rot, &pos, mat);
	DirectX::SimpleMath::Vector3 newPos = pos;
	//newPos += 4 * forwardVector;

	PhysicsComponent* itemPhysComp = holdingItem->getPhysComp();
	holdingItem->setPos(newPos);
	holdingItem->setRot(rot);
	itemPhysComp->setRotation(rot);
	itemPhysComp->setPosition(reactphysics3d::Vector3({ newPos.x, newPos.y, newPos.z }));

	//Controller functions
	if (state.IsConnected())
	{
		//Throw item
		if (tracker.b == ButtonState::PRESSED)
		{
			this->dropTimer.resetStartTime();
			this->throwingItem = true;
		}
		//Use item
		else if (keyPressTimer.getTimePassed(0.1f) && tracker.x == ButtonState::PRESSED && n1)
		{
			keyPressTimer.resetStartTime();
			if (holdingItem->getId() == BAT)
			{
				n1 = false;
				this->usingBat = true;
				this->usedItem = false;
				this->dropTimer.resetStartTime();
			}
			////sending data to server

			//allocates data to be sent

			if (holdingItem->getId() != BAT)
			{
				itemPhysComp->setType(reactphysics3d::BodyType::DYNAMIC);
				ComponentDropped c;

				std::cout << "Sending droppedComponent packet CompId: " << std::to_string(holdingItem->getOnlineId()) << std::endl;
				c.componentId = this->holdingItem->getOnlineId();
				c.packetId = PacketType::COMPONENTDROPPED;
				c.playerId = this->onlineID;
				if (holdingItem->getId() == GRENADE || holdingItem->getId() == COMPONENT) c.randomizePos = 0;
				else c.randomizePos = 1;
				c.xPos = this->holdingItem->getPosV3().x;
				c.yPos = this->holdingItem->getPosV3().y;
				c.zPos = this->holdingItem->getPosV3().z;
				//sending data to server
				if (this->client != nullptr)
				{
					client->sendStuff<ComponentDropped>(c);
				}
			}
			if (holdingItem->getId() == GRENADE)
			{
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, (this->forwardVector * 5.f + this->normalVector * 0.5f));
				newNormalizeXMFLOAT3(temp);
				if (this->moveKeyPressed)
				{
					if (this->currentSpeed == this->speed) scalarMultiplicationXMFLOAT3(this->currentSpeed * 0.0075f, temp);
					else scalarMultiplicationXMFLOAT3(this->currentSpeed * 0.085f, temp);
				}
				UseGrenade useGrenade;
				useGrenade.packetId = USEGRENADE;
				useGrenade.itemId = this->holdingItem->getOnlineId();
				useGrenade.xForce = temp.x * FORCE;
				useGrenade.yForce = temp.y * FORCE;
				useGrenade.zForce = temp.z * FORCE;

				client->sendStuff<UseGrenade>(useGrenade);

				//Set dynamic so it can be affected by forces
				this->holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::DYNAMIC);
				//Apply the force
				this->holdingItem->getPhysComp()->applyForceToCenter(reactphysics3d::Vector3(temp.x * FORCE, temp.y * FORCE, temp.z * FORCE));
			}
			if (holdingItem->getId() != BAT)
			{
				holdingItem->useItem(this);
				holdingItem->setPickedUp(false);
				holdingItem = nullptr;
			}
		}
	}
	//Keyboard functions
	else
	{
		//Throw item
		if (Input::KeyDown(KeyCode::R) && Input::KeyDown(KeyCode::R))
		{
			this->dropTimer.resetStartTime();
			this->throwingItem = true;
		}
		//Use the Item
		else if (keyPressTimer.getTimePassed(0.1f) && (Input::KeyPress(KeyCode::E) || Input::KeyPress(KeyCode::MOUSE_L)) && n1)
		{
			keyPressTimer.resetStartTime();
			if (holdingItem->getId() == BAT && usedItem)
			{
				n1 = false;
				this->usingBat = true;
				this->usedItem = false;
				this->dropTimer.resetStartTime();
			}
			////sending data to server

			//allocates data to be sent

			if (holdingItem->getId() != BAT)
			{
				itemPhysComp->setType(reactphysics3d::BodyType::DYNAMIC);
				ComponentDropped c;

				std::cout << "Sending droppedComponent packet CompId: " << std::to_string(holdingItem->getOnlineId()) << std::endl;
				c.componentId = this->holdingItem->getOnlineId();
				c.packetId = PacketType::COMPONENTDROPPED;
				c.playerId = this->onlineID;
				if (holdingItem->getId() == GRENADE || holdingItem->getId() == COMPONENT) c.randomizePos = 0;
				else c.randomizePos = 1;
				c.xPos = this->holdingItem->getPosV3().x;
				c.yPos = this->holdingItem->getPosV3().y;
				c.zPos = this->holdingItem->getPosV3().z;
				//sending data to server
				if (this->client != nullptr)
				{
					client->sendStuff<ComponentDropped>(c);
				}
			}
			if (holdingItem->getId() == GRENADE)
			{
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, (this->forwardVector * 1.f + this->normalVector * 0.5f));
				newNormalizeXMFLOAT3(temp);
				if (this->moveKeyPressed)
				{
					if (this->currentSpeed == this->speed) scalarMultiplicationXMFLOAT3(this->currentSpeed * 0.0075f, temp);
					else scalarMultiplicationXMFLOAT3(this->currentSpeed * 0.085f, temp);
				}

				UseGrenade useGrenade;
				useGrenade.packetId = USEGRENADE;
				useGrenade.itemId = this->holdingItem->getOnlineId();
				useGrenade.xForce = temp.x * FORCE;
				useGrenade.yForce = temp.y * FORCE;
				useGrenade.zForce = temp.z * FORCE;

				client->sendStuff<UseGrenade>(useGrenade);

				//Set dynamic so it can be affected by forces
				this->holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::DYNAMIC);
				//Apply the force
				this->holdingItem->getPhysComp()->applyForceToCenter(reactphysics3d::Vector3(temp.x * FORCE, temp.y * FORCE, temp.z * FORCE));
			}
			if (holdingItem->getId() != BAT)
			{
				holdingItem->useItem(this);
				holdingItem->setPickedUp(false);
				holdingItem = nullptr;
			}
		}
	}
	if (throwingItem && this->dropTimer.getTimePassed(0.25))
	{
		this->throwingItem = false;
		this->throwItem();
	}
	if (this->usingBat && this->dropTimer.getTimePassed(0.25) && !this->usedItem)
	{
		holdingItem->useItem(this);
		this->usedItem = true;
		std::cout << "using bat\n";
	}
	else if (this->usingBat && this->dropTimer.getTimePassed(0.5))
	{
		itemPhysComp->setType(reactphysics3d::BodyType::DYNAMIC);
		usingBat = false;
		std::cout << "dropping bat\n";
		ComponentDropped c;

		std::cout << "Sending droppedComponent packet CompId: " << std::to_string(holdingItem->getOnlineId()) << std::endl;
		c.componentId = this->holdingItem->getOnlineId();
		c.packetId = PacketType::COMPONENTDROPPED;
		c.playerId = this->onlineID;
		c.randomizePos = 1;
		c.xPos = this->holdingItem->getPosV3().x;
		c.yPos = this->holdingItem->getPosV3().y;
		c.zPos = this->holdingItem->getPosV3().z;
		//sending data to server
		if (this->client != nullptr)
		{
			client->sendStuff<ComponentDropped>(c);
		}
		holdingItem->setPickedUp(false);
		holdingItem = nullptr;
		n1 = true;
	}
}

//----------------------------------------------- Constructor ------------------------------------------------//

Player::Player(Mesh* useMesh, const AnimationData& data, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const int& id, const int& onlineId, Client* client, const int& team,
	ID3D11ShaderResourceView* redTeamColor, ID3D11ShaderResourceView* blueTeamColor, GravityField* field)
	:AnimatedMesh(useMesh, data, pos, rot, id, field), holdingItem(nullptr), team(team), currentSpeed(0)
{
	this->animIndex = 0;
	this->animSpeed = 1;
	pickUpSfx.load(L"../Sounds/pickupCoin.wav");
	playerHitSound.load(L"../Sounds/mixkit-sick-man-sneeze-2213.wav");
	//walkingSound.setVolume(0.25f);

	this->startPosition = pos;
	this->onlineID = onlineId;
	this->rotationMX = XMMatrixIdentity();
	this->rotation = XMMatrixIdentity();
	resultVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	angleVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	this->client = client;
	DirectX::XMStoreFloat4x4(&rotationFloat, this->rotationMX);
	HudUI::player = this;
	this->dedge = false;

	//Particles
	this->particles = new ParticleEmitter(pos, rot, 26, DirectX::XMFLOAT2(1, 3), 1);
	this->particles2 = new ParticleEmitter(pos, rot, 26, DirectX::XMFLOAT2(1, 3), 1);
	particles2->setSize(1.0f);


	//Color
	DirectX::SimpleMath::Vector3 color = DirectX::Colors::White.v;

	//Set up Fresnel buffer
	fresnelBuffer.Initialize(GPU::device, GPU::immediateContext);
	fresnelBuffer.getData() = DirectX::XMFLOAT4(color.x, color.y, color.z, 1);
	fresnelBuffer.applyData();

	//Item Icon
	float constant = 7.0f;
	DirectX::XMFLOAT3 upDir = this->getUpDirection();
	DirectX::XMFLOAT3 iconPos(upDir.x * constant, upDir.y * constant, upDir.z * constant);
	std::vector<std::string> playernames{ "Team1_r.png", "Team2_b.png", "player3.png", "player4.png" };
	this->playerIcon = new BilboardObject(playernames, iconPos);
	this->playerIcon->setOffset(constant);

	//Team switch
	//switch (team)
	//{
	//case 0:
	//	//this->setSrv(redTeamColor); break;
	//case 1:
	//	//this->setSrv(blueTeamColor); break;
	//}
	movementInfo.deaccTimer = 1.0f;
	movementInfo.jump = 30.f;
	movementInfo.maxVel = 20;
	movementInfo.speed = 200;
}

Player::~Player()
{
	if (this->playerIcon != nullptr)
	{
		delete playerIcon;
	}
	if (this->particles != nullptr)
	{
		delete particles;
		delete particles2;
	}
}

//----------------------------------------------- Functions ------------------------------------------------//


bool Player::movingCross(const DirectX::XMVECTOR& cameraForward, float deltaTime)
{
	//Calculations
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	northEastVector = DirectX::XMVector3TransformCoord(NORTH_EAST, rotationMX);
	northWestVector = DirectX::XMVector3TransformCoord(NORTH_WEST, rotationMX);
	southEastVector = DirectX::XMVector3TransformCoord(SOUTH_EAST, rotationMX);
	southWestVector = DirectX::XMVector3TransformCoord(SOUTH_WEST, rotationMX);

	northEastVector = DirectX::XMVector3Normalize(northEastVector);
	northWestVector = DirectX::XMVector3Normalize(northWestVector);
	southEastVector = DirectX::XMVector3Normalize(southEastVector);
	southWestVector = DirectX::XMVector3Normalize(southWestVector);

	//Walk North-East
	if (Input::KeyDown(KeyCode::W) && Input::KeyDown(KeyCode::D))
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(cameraForward, rightVector);

		if (resultVector.x > -0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		else if (resultVector.x < -0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		return true;
	}

	//Walk North-West
	else if (Input::KeyDown(KeyCode::W) && Input::KeyDown(KeyCode::A))
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(cameraForward, rightVector);

		if (resultVector.x < 0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		else if (resultVector.x > 0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		return true;
	}

	//Walk South-East
	else if (Input::KeyDown(KeyCode::S) && Input::KeyDown(KeyCode::D))
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(-cameraForward, rightVector);

		if (resultVector.x < 0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		else if (resultVector.x > 0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		return true;
	}

	//Walk South-West
	else if (Input::KeyDown(KeyCode::S) && Input::KeyDown(KeyCode::A))
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(-cameraForward, rightVector);

		if (resultVector.x > -0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		else if (resultVector.x < -0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		return true;
	}

	return false;
}

void Player::rotate(const DirectX::XMFLOAT3& grav, const bool& testingVec, const bool& changedPlanet)
{
	if (dedge) return;
	else if (!testingVec) normalVector = DirectX::XMVectorSet(-grav.x, -grav.y, -grav.z, 1.0f);
	else normalVector = DirectX::XMVectorSet(grav.x, grav.y, grav.z, 1.0f);

	//Player jumping to another planet
	if (changedPlanet) flipping = true;
	else if (onGround) flipping = false;

	//Calculations
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	normalVector = DirectX::XMVector3Normalize(normalVector);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	//X-Rotation
	resultVector = DirectX::XMVector3Dot(normalVector, forwardVector);
	if (resultVector.x < 0.f)
	{
		resultVector = DirectX::XMVector3Cross(rightVector, normalVector);
		resultVector = DirectX::XMVector3Normalize(resultVector);
		resultVector = DirectX::XMVector3AngleBetweenNormals(forwardVector, resultVector);

		//Changing planet, rotating slow
		if (flipping)
		{
			angle = resultVector.x * 0.03f;
			rotation *= DirectX::XMMatrixRotationAxis(rightVector, -angle);
			rotationMX *= DirectX::XMMatrixRotationAxis(rightVector, -angle);
		}
		//Rotating fast like normal
		else
		{
			rotation *= DirectX::XMMatrixRotationAxis(rightVector, -resultVector.x);
			rotationMX *= DirectX::XMMatrixRotationAxis(rightVector, -resultVector.x);
		}
	}
	else if (resultVector.x > 0.f)
	{
		resultVector = DirectX::XMVector3Cross(rightVector, normalVector);
		resultVector = DirectX::XMVector3Normalize(resultVector);
		resultVector = DirectX::XMVector3AngleBetweenNormals(forwardVector, resultVector);

		//Changing planet, rotating slow
		if (flipping)
		{
			angle = resultVector.x * 0.03f;
			rotation *= DirectX::XMMatrixRotationAxis(rightVector, angle);
			rotationMX *= DirectX::XMMatrixRotationAxis(rightVector, angle);
		}
		//Rotating fast like normal
		else
		{
			rotation *= DirectX::XMMatrixRotationAxis(rightVector, resultVector.x);
			rotationMX *= DirectX::XMMatrixRotationAxis(rightVector, resultVector.x);
		}
	}

	//Updating vectors
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	//Z-Rotation
	resultVector = DirectX::XMVector3Dot(normalVector, rightVector);
	if (resultVector.z < 0.f)
	{
		resultVector = DirectX::XMVector3Cross(normalVector, forwardVector);
		resultVector = DirectX::XMVector3Normalize(resultVector);
		resultVector = DirectX::XMVector3AngleBetweenNormals(rightVector, resultVector);

		//Changing planet, rotating slow
		if (flipping)
		{
			angle = resultVector.x * 0.03f;
			rotation *= DirectX::XMMatrixRotationAxis(forwardVector, angle);
			rotationMX *= DirectX::XMMatrixRotationAxis(forwardVector, angle);
		}
		//Rotating fast like normal
		else
		{
			rotation *= DirectX::XMMatrixRotationAxis(forwardVector, resultVector.z);
			rotationMX *= DirectX::XMMatrixRotationAxis(forwardVector, resultVector.z);
		}
	}
	else if (resultVector.z > 0.f)
	{
		resultVector = DirectX::XMVector3Cross(normalVector, forwardVector);
		resultVector = DirectX::XMVector3Normalize(resultVector);
		resultVector = DirectX::XMVector3AngleBetweenNormals(rightVector, resultVector);

		//Changing planet, rotating slow
		if (flipping)
		{
			angle = resultVector.x * 0.03f;
			rotation *= DirectX::XMMatrixRotationAxis(forwardVector, -angle);
			rotationMX *= DirectX::XMMatrixRotationAxis(forwardVector, -angle);
		}
		//Rotating fast like normal
		else
		{
			rotation *= DirectX::XMMatrixRotationAxis(forwardVector, -resultVector.z);
			rotationMX *= DirectX::XMMatrixRotationAxis(forwardVector, -resultVector.z);
		}
	}

	//Updating vectors
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);
}

void Player::move(const DirectX::XMVECTOR& cameraForward, const DirectX::XMVECTOR& cameraRight, const float& deltaTime)
{
	if (state.IsConnected()) return;
	if (!ready)
	{
		this->velocity = DirectX::SimpleMath::Vector3(0, 0, 0);
		return;
	}
	if (dedge || flipping) return;
	this->speed = movementInfo.speed;
	this->currentSpeed = this->speed;
	int maxSpeedVar = 1.f;
	if (this->holdingItem && this->holdingComp)
		this->currentSpeed *= 0.50f;
	//else if (this->holdingItem && this->holdingItem->getId() == ObjID::POTION)
	//{
	//	this->currentSpeed *= 2.f;
	//	maxSpeedVar = 2.0f;
	//}

	//Running
	if (!isHoldingComp())
	{
		if (Input::KeyDown(KeyCode::SHIFT) && this->onGround)
		{
			this->currentSpeed *= 1.5f;
		}
	}
	if ((this->onGround && getLength(this->velocity) > movementInfo.maxVel * (this->currentSpeed / this->speed)))
	{
		this->velocity.Normalize();
		this->velocity *= movementInfo.maxVel * (this->currentSpeed / this->speed);
	}
	if (!this->onGround)
		this->currentSpeed *= 0.025f;

	//Jumping
	if (onGround && Input::KeyDown(KeyCode::SPACE))
	{
		//Need to be changed
		this->position += this->normalVector * 1.0f;
		DirectX::SimpleMath::Vector3 angles = DirectX::XMVector3AngleBetweenNormals(this->velocity, this->forwardVector);
		angles = cos(angles.x) * this->velocity;
		this->velocity -= angles * 0.5f;
		this->velocity += this->normalVector * this->movementInfo.jump;

	}

	////Jumping
	//if (onGround)
	//{
	//	jumpTime = maxJumpTime;
	//}
	//if (onGround && Input::KeyDown(KeyCode::SPACE))
	//{
	//	this->velocity = this->normalVector * jumpForce;
	//	this->position += this->normalVector * 0.7f;
	//	stoppedJump = false;
	//}
	//if (Input::KeyDown(KeyCode::SPACE) && !stoppedJump)
	//{
	//	if (jumpTime > 0)
	//}

	//??
	
	if (!this->doneWithAnim)
	{
		return;
	}
	if (!Input::KeyDown(KeyCode::W) && !Input::KeyDown(KeyCode::A) && !Input::KeyDown(KeyCode::S) && !Input::KeyDown(KeyCode::D))
	{
		this->moveKeyPressed = false;
		if (this->onGround)
		{
			DirectX::SimpleMath::Vector3 retardationVector = 4.f * (DirectX::XMVector3Normalize(this->velocity)) * this->movementInfo.deaccTimer * this->movementInfo.maxVel * deltaTime;
			if (getLength(this->velocity) > getLength(retardationVector))
				this->velocity -= retardationVector;
			else
				this->velocity = DirectX::SimpleMath::Vector3(0, 0, 0);
		}
		return;
	}
	else
	{
		this->moveKeyPressed = true;
	}

	//PC movement
	if (movingCross(cameraForward, deltaTime)) {}

	//Walking forward
	else
	{
		velocity += forwardVector * deltaTime * this->currentSpeed;

		bool forward = Input::KeyDown(KeyCode::W) || Input::KeyDown(KeyCode::D);
		int temp = (2 * forward - 1);

		if (Input::KeyDown(KeyCode::A) || Input::KeyDown(KeyCode::D))
			cameraVec = cameraRight;
		else
			cameraVec = cameraForward;

		resultVector = DirectX::XMVector3Dot(temp * cameraVec, rightVector);

		if (resultVector.x < -0.05f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(temp * cameraVec, forwardVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.5f);
		}
		else if (resultVector.x > 0.05f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(temp * cameraVec, forwardVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.5f);
		}
		else
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(temp * cameraVec, forwardVector);
			if (resultVector.x > XM_PIDIV2) rotation *= DirectX::XMMatrixRotationAxis(normalVector, 0.5f);
		}
	}

}

bool Player::moveCrossController(const DirectX::XMVECTOR& cameraForward, float deltaTime)
{
	//Calculations
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	northEastVector = DirectX::XMVector3TransformCoord(NORTH_EAST, rotationMX);
	northWestVector = DirectX::XMVector3TransformCoord(NORTH_WEST, rotationMX);
	southEastVector = DirectX::XMVector3TransformCoord(SOUTH_EAST, rotationMX);
	southWestVector = DirectX::XMVector3TransformCoord(SOUTH_WEST, rotationMX);

	northEastVector = DirectX::XMVector3Normalize(northEastVector);
	northWestVector = DirectX::XMVector3Normalize(northWestVector);
	southEastVector = DirectX::XMVector3Normalize(southEastVector);
	southWestVector = DirectX::XMVector3Normalize(southWestVector);

	//Walking North-East
	if (posY > 0.0f && posX > 0.0f)
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(cameraForward, rightVector);

		if (resultVector.x > -0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		else if (resultVector.x < -0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		return true;
	}

	//Walk North-West
	else if (posY > 0.0f && posX < 0.0f)
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(cameraForward, rightVector);

		if (resultVector.x < 0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		else if (resultVector.x > 0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, northWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		return true;
	}

	//Walk South-East
	else if (posY < 0.0f && posX > 0.0f)
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(-cameraForward, rightVector);

		if (resultVector.x < 0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		else if (resultVector.x > 0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southEastVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		return true;
	}

	else if (posY < 0.0f && posX < 0.0f)
	{
		this->moveKeyPressed = true;
		velocity += forwardVector * deltaTime * this->currentSpeed;
		resultVector = DirectX::XMVector3Dot(-cameraForward, rightVector);

		if (resultVector.x > -0.4f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.3f);
		}
		else if (resultVector.x < -0.6f)
		{
			resultVector = DirectX::XMVector3AngleBetweenNormalsEst(forwardVector, southWestVector);
			rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.3f);
		}
		return true;
	}

	return false;
}

void Player::moveController(const DirectX::XMVECTOR& cameraForward, const DirectX::XMVECTOR& cameraRight, const float& deltaTime)
{
	if (!ready)
	{
		this->velocity = DirectX::SimpleMath::Vector3(0, 0, 0);
		return;
	}
	if (dedge || flipping) return;
	this->speed = movementInfo.speed;
	this->currentSpeed = this->speed;
	int maxSpeedVar = 1.f;
	if (this->holdingItem && this->holdingComp)
		this->currentSpeed *= 0.50f;

	if (state.IsConnected())
	{
		posX = state.thumbSticks.leftX;
		posY = state.thumbSticks.leftY;



		//Running
		if (!isHoldingComp())
		{
			if (state.IsRightTriggerPressed() && this->onGround || state.IsYPressed() && this->onGround)
			{
				this->currentSpeed *= 1.5f;
			}
		}
		if ((this->onGround && getLength(this->velocity) > movementInfo.maxVel * (this->currentSpeed / this->speed)))
		{
			this->velocity.Normalize();
			this->velocity *= movementInfo.maxVel * (this->currentSpeed / this->speed);
		}
		if (!this->onGround)
			this->currentSpeed *= 0.025f;


		//Jumping
		if (onGround && state.IsAPressed())
		{
			//Need to be changed
			this->position += this->normalVector * 1.0f;
			DirectX::SimpleMath::Vector3 angles = DirectX::XMVector3AngleBetweenNormals(this->velocity, this->forwardVector);
			angles = cos(angles.x) * this->velocity;
			this->velocity -= angles * 0.5f;
			this->velocity += this->normalVector * this->movementInfo.jump;

		}

		//???
		if (!this->doneWithAnim)
		{
			return;
		}
		if (!abs(state.thumbSticks.leftX) > 0 && !abs(state.thumbSticks.leftY) > 0)
		{
			this->moveKeyPressed = false;
			if (this->onGround)
			{
				DirectX::SimpleMath::Vector3 retardationVector = 4.f * (DirectX::XMVector3Normalize(this->velocity)) * this->movementInfo.deaccTimer * this->movementInfo.maxVel * deltaTime;
				if (getLength(this->velocity) > getLength(retardationVector))
					this->velocity -= retardationVector;
				else
					this->velocity = DirectX::SimpleMath::Vector3(0, 0, 0);
			}
			return;
		}
		else
		{
			this->moveKeyPressed = true;
		}

		//Controller movement
		if (moveCrossController(cameraForward, deltaTime)) {}



		//Walking forward
		else
		{
			velocity += forwardVector * deltaTime * this->currentSpeed;

			bool forward = state.IsLeftThumbStickUp() || state.IsLeftThumbStickRight();
			int temp = (2 * forward - 1);

			if (state.IsLeftThumbStickLeft() || state.IsLeftThumbStickRight())
				cameraVec = cameraRight;
			else
				cameraVec = cameraForward;

			resultVector = DirectX::XMVector3Dot(temp * cameraVec, rightVector);

			if (resultVector.x < -0.05f)
			{
				resultVector = DirectX::XMVector3AngleBetweenNormalsEst(temp * cameraVec, forwardVector);
				rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x * 0.5f);
			}
			else if (resultVector.x > 0.05f)
			{
				resultVector = DirectX::XMVector3AngleBetweenNormalsEst(temp * cameraVec, forwardVector);
				rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x * 0.5f);
			}
			else
			{
				resultVector = DirectX::XMVector3AngleBetweenNormalsEst(temp * cameraVec, forwardVector);
				if (resultVector.x > XM_PIDIV2) rotation *= DirectX::XMMatrixRotationAxis(normalVector, 0.5f);
			}
		}
	}

	//if (posX == 0.0f && posY == 0.0f) this->moveKeyPressed = false;
}

int Player::getItemOnlineType() const
{
	if (this->holdingItem == nullptr)
	{
		return -1;
	}
	return holdingItem->getOnlineType();
}

int Player::getItemOnlineId() const
{
	return holdingItem->getOnlineId();
}

bool Player::pickupItem(const std::vector <Item*>& items, const std::vector <Component*>& components)
{

	if (this->isHoldingItem() || dedge) return false;
	bool successfulPickup = false;

	//Controller pickup
	if (state.IsConnected())
	{
		if (tracker.x == GamePad::ButtonStateTracker::PRESSED && !this->eKeyDown && this->keyPressTimer.getTimePassed(0.1f) && this->holdingItem != nullptr)
		{
			this->eKeyDown = true;
		}
		if (tracker.x == GamePad::ButtonStateTracker::UP && this->eKeyDown)
		{
			this->eKeyDown = false;
			//Checking items
			for (int i = 0; i < items.size(); i++)
			{
				if (this->withinRadius(items[i], 8))
				{
					addItem(items[i]);
					this->isHoldingItem();
					this->holdingComp = false;
					holdingItem->getPhysComp()->getRigidBody()->resetForce();
					holdingItem->getPhysComp()->getRigidBody()->resetTorque();
					holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
					keyPressTimer.resetStartTime();
					pickUpSfx.stop();
					pickUpSfx.play();
					return true;
				}
			}

			//Checking components
			for (int i = 0; i < components.size(); i++)
			{
				if (this->withinRadius(components[i], 8))
				{
					addItem(components[i]);
					this->holdingComp = true;
					holdingItem->getPhysComp()->getRigidBody()->resetForce();
					holdingItem->getPhysComp()->getRigidBody()->resetTorque();
					holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
					keyPressTimer.resetStartTime();
					pickUpSfx.stop();
					pickUpSfx.play();
					return true;
				}
			}
		}
	}
	//Keyboard pickup

	else if (Input::KeyPress(KeyCode::E) || Input::KeyPress(KeyCode::MOUSE_L)) /*&&*/ /*this->eKeyDown == true &&*/ /*this->holdingItem != nullptr*/
	{
		std::cout << "stuff2" << std::endl;
		//Checking items
		this->eKeyDown = false;

		for (int i = 0; i < items.size(); i++)
		{
			if (this->withinRadius(items[i], 8.f))
			{

				addItem(items[i]);
				this->holdingComp = false;
				holdingItem->getPhysComp()->getRigidBody()->resetForce();
				holdingItem->getPhysComp()->getRigidBody()->resetTorque();
				holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
				keyPressTimer.resetStartTime();
				pickUpSfx.stop();
				pickUpSfx.play();
				return true;
			}
		}

		//Checking components
		for (int i = 0; i < components.size(); i++)
		{
			if (this->withinRadius(components[i], 8.f))
			{
				addItem(components[i]);
				this->holdingComp = true;
				holdingItem->getPhysComp()->getRigidBody()->resetForce();
				holdingItem->getPhysComp()->getRigidBody()->resetTorque();
				holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
				keyPressTimer.resetStartTime();
				pickUpSfx.stop();
				pickUpSfx.play();
				return true;
			}
		}
	}

	//else if (Input::KeyPress(KeyCode::E)/* && this->eKeyDown == false*/ &&  this->keyPressTimer.getTimePassed(0.1))
	//{
	//	std::cout << "stuff" << std::endl;
	//	this->eKeyDown = true;

	//}
	return successfulPickup;
}

void Player::setOnlineID(const int& id)
{
	this->onlineID = id;
}

void Player::hitByBat(const reactphysics3d::Vector3& force)
{
	playerHitSound.stop();
	playerHitSound.play();

	this->physComp->setType(reactphysics3d::BodyType::DYNAMIC);
	this->dedge = true;
	if (state.IsConnected()) this->gamePad->SetVibration(0, 0.5f, 0.5f, 0.f, 0.f);
	this->physComp->applyForceToCenter(force);
	this->physComp->applyWorldTorque(force);
	timer.resetStartTime();

	if (this->holdingItem)
	{
		ComponentDropped cDropped;
		cDropped.packetId = COMPONENTDROPPED;
		cDropped.playerId = this->onlineID;
		cDropped.componentId = this->holdingItem->getOnlineId();
		cDropped.randomizePos = 0;
		cDropped.xPos = this->holdingItem->getPosV3().x;
		cDropped.yPos = this->holdingItem->getPosV3().y;
		cDropped.zPos = this->holdingItem->getPosV3().z;

		client->sendStuff<ComponentDropped>(cDropped);
		this->releaseItem();
	}
}

void Player::addItem(Item* itemToHold)
{
	if (!this->holdingItem)
	{
		this->holdingItem = itemToHold;
		this->holdingItem->setPickedUp(true);
		if (holdingItem->getId() == ObjID::COMPONENT) this->holdingComp = true;
		else this->holdingComp = false;
	}
	holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::DYNAMIC);
}

void Player::releaseItem()
{
	if (this->holdingItem != nullptr)
	{
		this->holdingItem->setPickedUp(false);
		this->holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::DYNAMIC);
		this->holdingItem = nullptr;
	}
}

bool Player::checkForStaticCollision(const std::vector<Planet*>& gameObjects, const std::vector<SpaceShip*>& spaceShips)
{
	SimpleMath::Vector3 vecPoint = this->position;
	vecPoint += 1.f * forwardVector;
	reactphysics3d::Vector3 point(vecPoint.x, vecPoint.y, vecPoint.z);

	int gameObjSize = (int)gameObjects.size();
	for (int i = 0; i < gameObjSize; i++)
	{
		//if (gameObjects[i]->getPlanetCollider()->getType() != reactphysics3d::BodyType::STATIC || gameObjects[i] == this->holdingItem) continue; 
		if (gameObjects[i]->getPlanetCollider()->testPointInside(point))
		{
			//this->position -= 1.f * forwardVector;
			return true;
		}
	}
	for (int i = 0; i < spaceShips.size(); i++)
	{
		if (spaceShips[i]->getPhysComp()->testPointInside(point))
		{
			this->position -= 1.f * forwardVector;
			return true;
		}
	}
	return false;
}

bool Player::checkSwimStatus(const std::vector<Planet*>& planets)
{
	//Checks if the player is inside planet
	for (int i = 0; i < planets.size(); i++)
	{
		planetVector = DirectX::XMVectorSet(planets[i]->getSize(), planets[i]->getSize(), planets[i]->getSize(), 0.0f);
		playerVector = XMVectorSubtract(planets[i]->getPlanetPosition(), this->position);
		playerVector = XMVector3Length(playerVector);

		//The player is swimming
		if (XMVector3Less(playerVector, planetVector))
		{
			this->position += normalVector * 3.f;
			return true;
		}
	}
	return false;
}

bool Player::raycast(const std::vector<SpaceShip*>& gameObjects, const std::vector<Planet*>& planets, DirectX::XMFLOAT3& hitPos, DirectX::XMFLOAT3& hitNormal)
{
	if (!dedge)
	{
		DirectX::SimpleMath::Quaternion dx11Quaternion = DirectX::XMQuaternionRotationMatrix(this->rotation);
		reactphysics3d::Quaternion reactQuaternion = reactphysics3d::Quaternion(dx11Quaternion.x, dx11Quaternion.y, dx11Quaternion.z, dx11Quaternion.w);
		this->physComp->setRotation(reactQuaternion);
		if (holdingItem != nullptr) this->holdingItem->getPhysComp()->setRotation(reactQuaternion);
	}
	reactphysics3d::Ray ray(reactphysics3d::Vector3(this->position.x, this->position.y, this->position.z), reactphysics3d::Vector3(this->getRayCastPos()));
	reactphysics3d::RaycastInfo rayInfo;

	bool testingVec = false;
	onGround = false;

	int gameObjSize = (int)gameObjects.size();
	for (int i = 0; i < gameObjSize; i++)
	{
		if (gameObjects[i]->getPhysComp()->raycast(ray, rayInfo))
		{
			//Maybe somehow return the index of the triangle hit to calculate new Normal
			hitPos = DirectX::XMFLOAT3(rayInfo.worldPoint.x, rayInfo.worldPoint.y, rayInfo.worldPoint.z);
			hitNormal = DirectX::XMFLOAT3(rayInfo.worldNormal.x, rayInfo.worldNormal.y, rayInfo.worldNormal.z);
			onGround = true;
			DirectX::SimpleMath::Vector3 vecToHitPos = this->position - hitPos;
			float lengthToMove = 1.f - getLength(vecToHitPos);
			vecToHitPos.Normalize();
			this->position += lengthToMove * vecToHitPos;
			return true;
		}
	}
	gameObjSize = (int)planets.size();
	for (int i = 0; i < gameObjSize; i++)
	{
		if (planets[i]->getPlanetCollider()->raycast(ray, rayInfo))
		{
			//Maybe somehow return the index of the triangle hit to calculate new Normal
			hitPos = DirectX::XMFLOAT3(rayInfo.worldPoint.x, rayInfo.worldPoint.y, rayInfo.worldPoint.z);
			hitNormal = DirectX::XMFLOAT3(rayInfo.worldNormal.x, rayInfo.worldNormal.y, rayInfo.worldNormal.z);
			onGround = true;
			DirectX::SimpleMath::Vector3 vecToHitPos = this->position - hitPos;
			float lengthToMove = 1.f - getLength(vecToHitPos);
			vecToHitPos.Normalize();
			this->position += lengthToMove * vecToHitPos;
			return true;
		}
	}
	return false;
}

bool Player::withinRadius(Item* itemToLookWithinRadius, const float& radius) const
{
	DirectX::XMFLOAT3 objPos = itemToLookWithinRadius->getPos();
	DirectX::XMFLOAT3 selfPos = this->getPos();
	bool inRange = false;
	subtractionXMFLOAT3(objPos, this->getPos());
	float lengthToVec = getLength(objPos);
	if (lengthToVec <= radius) inRange = true;
	return inRange;
}

void Player::colliedWIthComponent(const std::vector<Component*>& components)
{
	if (holdingComp && holdingItem) return;
	bool collided = false;
	for (int i = 0; i < components.size(); i++) collided = this->physComp->testBodiesOverlap(components[i]->getPhysComp());
	if (collided) this->setSpeed(this->speed * 0.5f);
}

void Player::orbiting()
{
	if (!onGround)
	{
		if (orbitTimer.getTimePassed(10.f))
		{
			std::cout << "SETTING BACK TO PLANET\n";
			this->physComp->resetForce();
			this->physComp->resetTorque();
			this->physComp->setType(reactphysics3d::BodyType::STATIC);
			this->resetRotationMatrix();
			this->position = startPosition;
			this->physComp->setPosition(reactphysics3d::Vector3({ this->position.x, this->position.y, this->position.z }));
			this->physComp->setType(reactphysics3d::BodyType::KINEMATIC);
			this->velocity = DirectX::SimpleMath::Vector3(0, 0, 0);
			orbitTimer.resetStartTime();
			onGround = true;
		}
	}
	else orbitTimer.resetStartTime();
}

void Player::stateMachine(const float dt)
{
	if (state.IsConnected())
	{
		if (!doneWithAnim)
		{

		}
		else if (!this->onGround)
		{
			this->animIndex = 5;
			this->animSpeed = 1;
		}
		else if (tracker.x == ButtonState::PRESSED && this->holdingItem != nullptr)
		{
			this->animIndex = 4;
			this->animSpeed = 2.5;
			this->doneWithAnim = false;
		}
		else if (tracker.b == ButtonState::PRESSED && this->holdingItem != nullptr)
		{
			this->animSpeed = 2;
			this->animIndex = 3;
			this->doneWithAnim = false;
		}
		else if (abs(state.thumbSticks.leftX) > 0 || abs(state.thumbSticks.leftY) > 0)
		{
			if (tracker.rightTrigger == ButtonState::HELD || tracker.y == ButtonState::HELD)// sprint
			{
				this->animSpeed = 1;
				if (!isHoldingComp())
				{
					this->animIndex = 2;
				}
				else
				{
					this->animIndex = 1;
				}
			}
			else if (abs(state.thumbSticks.leftX) > 0.5 || abs(state.thumbSticks.leftY) > 0.5)
			{
				this->animSpeed = 1.5;
				this->animIndex = 1;
			}
			else
			{
				this->animSpeed = 1;
				this->animIndex = 1;
			}
		}
		else
		{
			this->animSpeed = 1;
			this->animIndex = 0;
		}
	}
	else
	{
		if (!doneWithAnim)
		{

		}
		else if (!this->onGround)
		{
			this->animIndex = 5;
			this->animSpeed = 1;
		}
		else if ((GetAsyncKeyState('E') || Input::KeyPress(KeyCode::MOUSE_L)) && this->holdingItem != nullptr )
		{
			this->animIndex = 4;
			this->animSpeed = 2.5;
			this->doneWithAnim = false;
		}
		else if (GetAsyncKeyState('R') && this->holdingItem != nullptr)
		{
			this->animSpeed = 2;
			this->animIndex = 3;
			this->doneWithAnim = false;
		}
		else if (GetAsyncKeyState('W') || GetAsyncKeyState('D') || GetAsyncKeyState('S') || GetAsyncKeyState('A'))
		{
			if (GetAsyncKeyState(VK_LSHIFT))
			{
				this->animSpeed = 1;
				if (!isHoldingComp())
				{
					this->animIndex = 2;
				}
				else
				{
					this->animIndex = 1;
				}
			}
			else
			{
				this->animSpeed = 1;
				this->animIndex = 1;
			}
		}
		else
		{
			this->animSpeed = 1;
			this->animIndex = 0;
		}
	}
	if (!ready)
	{
		this->updateAnim(dt, 0, 1);
	}
	else
	{
		this->updateAnim(dt, this->animIndex, this->animSpeed);
	}
}

void Player::giveItemMatrix()
{
	if (this->holdingItem == nullptr)
	{
		return;
	}
	DirectX::XMFLOAT4X4 f1;
	this->forwardKinematics("Character_RightHand", f1);
	this->holdingItem->setMatrix(f1);
}

bool Player::getHitByBat() const
{
	return dedge;
}

float Player::getSpeed()const
{
	return this->currentSpeed;
}

void Player::getAnimSpeed(float& speed)
{
	speed = this->animSpeed;
}

void Player::drawIcon()
{
	if (this->playerIcon != nullptr)
	{
		this->playerIcon->bindAndDraw(this->team, 0);
	}
}

void Player::drawParticles()
{
	if (this->particles != nullptr/* && moveKeyPressed*/)
	{
		this->particles->BindAndDraw(0);
	}
	if (this->currentSpeed > 200)
	{
		particles2->setColor(DirectX::SimpleMath::Vector3(this->fresnelBuffer.getData().x, this->fresnelBuffer.getData().y, this->fresnelBuffer.getData().z));
		particles2->BindAndDraw(4);
	}
}

int Player::getTeam() const
{
	return this->team;
}

DirectX::XMVECTOR Player::getUpVector() const
{
	return this->normalVector;
}

DirectX::XMVECTOR Player::getForwardVector() const
{
	return this->forwardVector;
}

DirectX::XMVECTOR Player::getRightVector() const
{
	return this->rightVector;
}

DirectX::XMMATRIX Player::getRotationMX() const
{
	return this->rotationMX;
}

DirectX::SimpleMath::Vector3 Player::getVelocity() const
{
	return this->velocity;
}

reactphysics3d::Vector3 Player::getRayCastPos() const
{
	static SimpleMath::Vector3 returnValue;
	returnValue = this->position;
	returnValue += this->normalVector * -1.5f;
	return reactphysics3d::Vector3(returnValue.x, returnValue.y, returnValue.z);
}

Item* Player::getItem()const
{
	return this->holdingItem;
}

int Player::getOnlineID() const
{
	return this->onlineID;
}

bool Player::isHoldingItem() const
{
	bool isHolding = false;
	if (this->holdingItem == nullptr)
	{
		isHolding = false;
	}
	else
	{
		isHolding = true;
	}
	return isHolding;
}

void Player::setSpeed(float speed)
{
	if (speed == 1.f)
	{
		this->movementInfo.speed = 300.f;
		this->movementInfo.maxVel = 30.f;
	}
	else
	{
		this->movementInfo.speed = 200.f;
		this->movementInfo.maxVel = 20.f;
	}
}

void Player::update()
{
	if (holdingItem != nullptr)
	{
		this->handleItems();
	}
	if (!dedge)
	{
		DirectX::XMFLOAT4X4 f1;
		this->forwardKinematics("Character_Hips", f1);
		DirectX::XMFLOAT3 hipPos;
		hipPos.x = f1._41;
		hipPos.y = f1._42;
		hipPos.z = f1._43;

		this->physComp->setPosition(reactphysics3d::Vector3({ hipPos.x, hipPos.y, hipPos.z }));
	}
	else if (dedge)
	{
		this->position = this->physComp->getPosV3();
		reactphysics3d::Quaternion reactQuaternion;
		DirectX::SimpleMath::Quaternion dx11Quaternion;
		reactQuaternion = this->physComp->getRotation();
		dx11Quaternion = DirectX::SimpleMath::Quaternion(DirectX::SimpleMath::Vector4(reactQuaternion.x, reactQuaternion.y, reactQuaternion.z, reactQuaternion.w));
		this->rotation = DirectX::XMMatrixRotationRollPitchYawFromVector(dx11Quaternion.ToEuler());
		if (timer.getTimePassed(5.f))
		{
			dedge = false;
			if (state.IsConnected()) this->gamePad->SetVibration(0, 0.f, 0.f, 0.f, 0.f);
			this->physComp->resetForce();
			this->physComp->resetTorque();
			this->physComp->setType(reactphysics3d::BodyType::STATIC);
			this->resetRotationMatrix();
			this->physComp->setPosition(reactphysics3d::Vector3({ this->position.x, this->position.y, this->position.z }));
			this->physComp->setType(reactphysics3d::BodyType::KINEMATIC);
			this->flipping = true;
		}
	}
	//Update icon movement
	if (this->playerIcon != nullptr)
	{
		DirectX::XMFLOAT4X4 f1;
		this->forwardKinematics("Character_Head", f1);
		DirectX::XMFLOAT3 headPos = DirectX::XMFLOAT3(f1._41, f1._42, f1._43);
		float constant = playerIcon->getOffset();
		DirectX::XMFLOAT3 upDir = this->getUpDirection();
		DirectX::XMFLOAT3 itemPos(upDir.x * constant, upDir.y * constant, upDir.z * constant);
		this->playerIcon->setPosition(DirectX::XMFLOAT3(headPos.x + itemPos.x, headPos.y + itemPos.y, headPos.z + itemPos.z));
	}
	//Update particle movement
	if (this->particles != nullptr)
	{
		this->particles->setPosition(this->position);
		this->particles->setRotation(this->getUpDirection());
		this->particles->updateBuffer();

		this->particles2->setPosition(this->position);
		this->particles2->setRotation(this->getUpDirection());
		this->particles2->updateBuffer();
	}
}

void Player::setTeam(const int& team)
{
	this->team = team;

	//Team switch
	//switch (team)
	//{
	//case 0:
	//	mesh->matKey[0] = "pintoRed.png"; break;
	//case 1:
	//	mesh->matKey[0] = "pintoBlue.png"; break;
	//}
}

void Player::setVibration(float vibration1, float vibration2)
{
	if (state.IsConnected()) this->gamePad->SetVibration(0, vibration1, vibration2, 0.f, 0.f);
}

void Player::setGamePad(DirectX::GamePad* gamePad)
{
	this->gamePad = gamePad;
}


void Player::isReady(bool ready)
{
	this->ready = ready;
}

bool Player::readyToPlay() const
{
	return this->ready;
}

void Player::setStartPosition(DirectX::SimpleMath::Vector3 startPosition)
{
	this->startPosition = startPosition;

}

void Player::requestingPickUpItem(const std::vector<Item*>& items)
{
	if (holdingItem || dedge) return;
	if (state.IsConnected())
	{
		if (tracker.x == GamePad::ButtonStateTracker::PRESSED && !this->eKeyDown && this->keyPressTimer.getTimePassed(0.1f))
		{
			this->eKeyDown = true;
		}
		if (tracker.x == GamePad::ButtonStateTracker::UP && this->eKeyDown)
		{
			this->eKeyDown = false;
			std::cout << "items.size = " << std::to_string(items.size()) << std::endl;
			for (int i = 0; i < items.size(); i++)
			{
				if (this->withinRadius(items[i], 8.f))
				{
					if (items[i]->getId() == ObjID::GRENADE)
					{
						if (((Grenade*)items[i])->hasBeenUsed())
							continue;
					}
					//addItem(items[i]);

					//holdingItem->getPhysComp()->getRigidBody()->resetForce();
					//holdingItem->getPhysComp()->getRigidBody()->resetTorque();
					//holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
					ComponentRequestingPickUp rqstCmpPickUp;
					rqstCmpPickUp.componentId = items[i]->getOnlineId();
					rqstCmpPickUp.packetId = PacketType::COMPONENTREQUESTINGPICKUP;
					rqstCmpPickUp.playerId = this->getOnlineID();
					if (items[i]->getId() == ObjID::COMPONENT) this->holdingComp = true;
					else this->holdingComp = false;
					std::cout << "requesting pickup componentId: " << std::to_string(rqstCmpPickUp.componentId) << std::endl;
					//skickar en f�rfr�gan att plocka upp item
					keyPressTimer.resetStartTime();
					client->sendStuff<ComponentRequestingPickUp>(rqstCmpPickUp);
					break;
				}
			}
		}
	}
	else
	{
		if ((GetAsyncKeyState('E') || Input::KeyPress(KeyCode::MOUSE_L)) && this->eKeyDown == false && this->keyPressTimer.getTimePassed(0.1f))
		{
			this->eKeyDown = true;
		}
		else if ((GetAsyncKeyState('E') || Input::KeyPress(KeyCode::MOUSE_L)) == 0 && this->eKeyDown == true)
		{
			this->eKeyDown = false;
			std::cout << "items.size = " << std::to_string(items.size()) << std::endl;
			for (int i = 0; i < items.size(); i++)
			{
				if (this->withinRadius(items[i], 8.f))
				{
					if (items[i]->getId() == ObjID::GRENADE)
					{
						if (((Grenade*)items[i])->hasBeenUsed())
							continue;
					}
					//addItem(items[i]);

					//holdingItem->getPhysComp()->getRigidBody()->resetForce();
					//holdingItem->getPhysComp()->getRigidBody()->resetTorque();
					//holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
					ComponentRequestingPickUp rqstCmpPickUp;
					rqstCmpPickUp.componentId = items[i]->getOnlineId();
					rqstCmpPickUp.packetId = PacketType::COMPONENTREQUESTINGPICKUP;
					rqstCmpPickUp.playerId = this->getOnlineID();
					if (items[i]->getId() == ObjID::COMPONENT) this->holdingComp = true;
					else this->holdingComp = false;
					std::cout << "requesting pickup componentId: " << std::to_string(rqstCmpPickUp.componentId) << std::endl;
					//skickar en f�rfr�gan att plocka upp item
					keyPressTimer.resetStartTime();
					client->sendStuff<ComponentRequestingPickUp>(rqstCmpPickUp);
					break;
				}
			}
		}
	}
}

void Player::updateController()
{
	this->state = gamePad->GetState(0);
	tracker.Update(state);
}

playerMovement* Player::getPlayerMovementInfo()
{
	return &this->movementInfo;
}

void Player::pushPlayer(std::vector<Player*>& players)
{
	if (this->holdingItem != nullptr) return;
	DirectX::SimpleMath::Vector3 hitBox = this->position + 1.25f * 2.5f * this->forwardVector;
	DirectX::SimpleMath::Vector3 playerToBox;

	for (int i = 0; i < players.size(); i++)
	{
		if (i == onlineID) continue;
		playerToBox = players[i]->getPosV3() - hitBox;
		std::cout << "Distance to player" << getLength(playerToBox) << "\n";
		if (getLength(playerToBox) < 1.25f * 3.5f)
		{
			DirectX::SimpleMath::Vector3 tempForce;
			tempForce = DirectX::XMVector3Normalize(this->position - players[i]->getPosV3()) * -movementInfo.pushPower;

			PlayerPush pushPlayerData;
			pushPlayerData.affectedPlayerId = i;
			pushPlayerData.packetId = PacketType::PLAYERPUSHED;
			pushPlayerData.xForce = tempForce.x;
			pushPlayerData.yForce = tempForce.y;
			pushPlayerData.zForce = tempForce.z;
			client->sendStuff<PlayerPush>(pushPlayerData);
		}
	}
}

void Player::addForceToPlayer(const DirectX::SimpleMath::Vector3& pushForce)
{
	if (!this->onGround) return;
	this->tempPushVar = 0.25f;
	this->velocity += pushForce;
}

void Player::itemRecvFromServer(Item* item)
{
	addItem(item);

	holdingItem->getPhysComp()->getRigidBody()->resetForce();
	holdingItem->getPhysComp()->getRigidBody()->resetTorque();
	holdingItem->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
}

bool Player::isHoldingComp()
{
	if (holdingComp)
	{
		if (this->holdingItem != nullptr)
		{
			//this->setSpeed(this->speed );
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool Player::getFlip() const
{
	return this->flipping;
}

void Player::updateVelocity(const DirectX::SimpleMath::Vector3& gravityVector)
{
	this->velocity += gravityVector;
}

void Player::resetVelocity()
{
	if (tempPushVar > 0 || !ready) return;
	DirectX::SimpleMath::Vector3 angles = DirectX::XMVector3AngleBetweenNormals(this->velocity, this->forwardVector);
	//float testValue = abs(cos(angles.x));
	float testValue = cos(angles.x);

	this->velocity *= testValue;
	this->velocity = DirectX::XMVector3Normalize(this->forwardVector) * getLength(this->velocity);
}

void Player::velocityMove(const float& dt)
{
	this->position += velocity * dt;
	if (this->tempPushVar > 0.f) this->tempPushVar -= dt;
}

void Player::drawFresnel(float interval)
{
	//If picked up "potion"
	if (currentSpeed > 200 && animIndex != 0)
	{
		//Variables
		static float time = 0;
		static float scl2 = 1;
		static float scl3 = 1;
		static float scl4 = 1;

		float constant = 0.03f;
		DirectX::XMFLOAT3 scl = this->scale;
		DirectX::XMFLOAT3 pos = this->position;

		//Time
		time += Time::DeltaTimeInSeconds();
		float dt = Time::DeltaTimeInSeconds();
		float percent = time / interval;

		//Reset if reached interval
		if (time > interval)
		{
			pos1 = this->position;
			time = 0;
			scl2 = scl.x;
		}
		else if (time > (interval / 2))
		{
			pos3 = this->position;
			scl3 = scl.x;
		}
		else if (time > (interval / 4))
		{
			pos4 = this->position;
			scl4 = scl.x;
		}

		//Change color
		DirectX::SimpleMath::Color currentColor = DirectX::SimpleMath::Color::Lerp(DirectX::Colors::Cyan.v, DirectX::Colors::Orange.v, percent);
		this->fresnelBuffer.getData() = DirectX::XMFLOAT4(currentColor.x, currentColor.y, currentColor.z, 3.f);
		this->fresnelBuffer.applyData();
		GPU::immediateContext->PSSetConstantBuffers(2, 1, fresnelBuffer.getReferenceOf());

		//Set temp scale + pos
		this->position = pos1;
		scl2 = scl2 - dt * constant;
		this->scale = DirectX::XMFLOAT3(scl2, scl2, scl2);

		//set transparancy based on distance
		DirectX::XMFLOAT3 restultV = { pos.x - pos1.x, pos.y - pos1.y, pos.z - pos1.z };
		float dist1 = sqrt(restultV.x * restultV.x + restultV.y * restultV.y + restultV.z * restultV.z);
		if (dist1 < 1) { dist1 = 1; };
		this->fresnelBuffer.getData() = DirectX::XMFLOAT4(currentColor.x, currentColor.y, currentColor.z, dist1);
		this->fresnelBuffer.applyData();

		//draw as fresnel
		this->draw();

		//Set temp scale + pos
		this->position = pos3;
		scl3 = scl3 - dt * constant;
		this->scale = DirectX::XMFLOAT3(scl3, scl3, scl3);

		//set transparancy based on distance
		DirectX::XMFLOAT3 restultV2 = { pos.x - pos3.x, pos.y - pos3.y, pos.z - pos3.z };
		dist1 = sqrt(restultV2.x * restultV2.x + restultV2.y * restultV2.y + restultV2.z * restultV2.z);
		if (dist1 < 1) { dist1 = 1; };
		this->fresnelBuffer.getData() = DirectX::XMFLOAT4(currentColor.x, currentColor.y, currentColor.z, dist1);
		this->fresnelBuffer.applyData();

		//draw as fresnel
		this->draw();

		//Set temp scale + pos
		this->position = pos4;
		scl4 = scl4 - dt * constant;
		this->scale = DirectX::XMFLOAT3(scl4, scl4, scl4);

		//set transparancy based on distance
		DirectX::XMFLOAT3 restultV3 = { pos.x - pos4.x, pos.y - pos4.y, pos.z - pos4.z };
		dist1 = sqrt(restultV3.x * restultV3.x + restultV3.y * restultV3.y + restultV3.z * restultV3.z);
		if (dist1 < 1) { dist1 = 1; };
		this->fresnelBuffer.getData() = DirectX::XMFLOAT4(currentColor.x, currentColor.y, currentColor.z, dist1);
		this->fresnelBuffer.applyData();

		//draw as fresnel
		this->draw();

		//Reset scale& pos
		this->scale = scl;
		this->position = pos;
	}
	else
	{
		pos1 = this->position;
		pos3 = this->position;
		pos4 = this->position;
	}

	if (this->isHoldingComp())
	{
		float constant = 0.02f;
		DirectX::XMFLOAT3 scl = this->scale;
		DirectX::XMFLOAT3 pos = this->position;

		//Change color
		DirectX::SimpleMath::Color currentColor;
		if (team == 0)
		{
			currentColor = DirectX::Colors::Red.v;
		}
		else
		{
			currentColor = DirectX::Colors::Blue.v;
		}

		this->fresnelBuffer.getData() = DirectX::XMFLOAT4(currentColor.x, currentColor.y, currentColor.z, 0.1f);
		this->fresnelBuffer.applyData();
		GPU::immediateContext->PSSetConstantBuffers(2, 1, fresnelBuffer.getReferenceOf());

		//Set temp scale + pos
		float scl2 = scl.x + constant;
		this->scale = DirectX::XMFLOAT3(scl2, scl2, scl2);

		//draw as fresnel
		this->draw();

		//Reset scale& pos
		this->scale = scl;
	}
}
