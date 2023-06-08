#pragma once

#include "GameObject.h"
#include "Input.h"
#include "BilboardObject.h"
#include "ParticleEmitter.h"
#include "Potion.h"
#include "TimeStruct.h"
#include "Client.h"
#include "AnimatedMesh.h"
#include "Planet.h"
#include "Time.h"
#include <GamePad.h>
#include <iostream>
#define FORCE 2500

class Item;
class SpaceShip;
class Component;

struct playerMovement
{
	float speed;
	float maxVel;
	float deaccTimer;
	float jump;
	float pushPower = 10.f;
};

class Player : public AnimatedMesh
{
private:
	//Movement variables
	playerMovement movementInfo;
	float tempPushVar = 0.0f;
	float angle = 0.f;
	float speed = 20.f;
	float currentSpeed = 0.f;
	float retardationFactor = 1.0f;
	float jumpHeight = 30.f;
	DirectX::SimpleMath::Vector3 resultVector;
	DirectX::SimpleMath::Vector3 angleVector;
	DirectX::SimpleMath::Vector3 velocity; //FINALLY ADDED THIS F*****G STUPID VARIABLE
	DirectX::SimpleMath::Vector3 startPosition;
	DirectX::XMVECTOR cameraVec;

	//Player rotation
	DirectX::XMMATRIX rotationMX;
	DirectX::XMFLOAT4X4 rotationFloat;

	//SFX
	Sound pickUpSfx;
	Sound walkingSound;
	Sound playerHitSound;

	//Status
	int team;
	int onlineID;
	bool ready = false;
	bool dedge = false;
	bool flipping = false;
	bool onGround = false;
	bool holdingComp = false;
	bool moveKeyPressed = false;

	//Controller variables
	float posX = 0.0f;
	float posY = 0.0f;
	float throttle = 0.0f;
	float totalPos = 0.0f;
	DirectX::GamePad* gamePad;
	DirectX::GamePad::ButtonStateTracker tracker;
	DirectX::GamePad::State state;

	//Other variables
	Client* client;
	TimeStruct timer;
	TimeStruct keyPressTimer;
	Item* holdingItem;

	TimeStruct orbitTimer;
	ParticleEmitter* particles;
	ParticleEmitter* particles2;
	BilboardObject* playerIcon;
	ConstantBufferNew<DirectX::XMFLOAT4> fresnelBuffer;	//fresnel color buffer
		
														//Fresnel positions
	DirectX::XMFLOAT3 pos1{ 0,0,0 };
	DirectX::XMFLOAT3 pos3{ 0,0,0 };
	DirectX::XMFLOAT3 pos4{ 0,0,0 };

	const DirectX::XMVECTOR DEFAULT_UP = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_FORWARD = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::SimpleMath::Vector3 normalVector = DEFAULT_UP;
	DirectX::SimpleMath::Vector3 rightVector = DEFAULT_RIGHT;
	DirectX::SimpleMath::Vector3 forwardVector = DEFAULT_FORWARD;

	const DirectX::XMVECTOR NORTH_EAST = DirectX::XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR NORTH_WEST = DirectX::XMVectorSet(-1.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR SOUTH_EAST = DirectX::XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR SOUTH_WEST = DirectX::XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR northEastVector = NORTH_EAST;
	DirectX::XMVECTOR northWestVector = NORTH_WEST;
	DirectX::XMVECTOR southEastVector = SOUTH_EAST;
	DirectX::XMVECTOR southWestVector = SOUTH_WEST;
	DirectX::SimpleMath::Vector3 playerVector;
	DirectX::SimpleMath::Vector3 planetVector;
	int animIndex = 0;
	bool eKeyDown = false;
	float animSpeed = 1;
	bool usingBat = false;
	TimeStruct dropTimer;
	bool usedItem = true;
	bool throwingItem = false;
	bool n1 = true;

private:
	void throwItem();
	void resetRotationMatrix();
	void handleItems();
	bool movingCross(const DirectX::XMVECTOR& cameraForward, float deltaTime);
	bool moveCrossController(const DirectX::XMVECTOR& cameraForward, float deltaTime);

public:
	Player(Mesh* useMesh, const AnimationData& data, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, const int& id, const int& onlineId, Client* client, const int& team,
		ID3D11ShaderResourceView* redTeamColor, ID3D11ShaderResourceView* blueTeamColor, GravityField* field = nullptr);
	~Player();

	//Move Functions
	void rotate(const DirectX::XMFLOAT3& grav, const bool& testingVec, const bool& changedPlanet);
	void move(const DirectX::XMVECTOR& cameraForward, const DirectX::XMVECTOR& cameraRight, const float& deltaTime);
	void moveController(const DirectX::XMVECTOR& cameraForward, const DirectX::XMVECTOR& cameraRight, const float& deltaTime);
	void updateVelocity(const DirectX::SimpleMath::Vector3& gravityVec);
	void resetVelocity();
	void velocityMove(const float& dt);

	//Set Functions
	void setSpeed(float speed);
	void setOnlineID(const int& id);
	void setTeam(const int& team);
	void setVibration(float vibration1, float vibration2);
	void setGamePad(DirectX::GamePad* gamePad);

	void isReady(bool ready);
	bool readyToPlay() const;
	void setStartPosition(DirectX::SimpleMath::Vector3 startPosition);

	//Get Functions
	reactphysics3d::Vector3 getRayCastPos()const;
	DirectX::XMVECTOR getUpVector() const;
	DirectX::XMVECTOR getForwardVector() const;
	DirectX::XMVECTOR getRightVector() const;
	DirectX::XMMATRIX getRotationMX() const;
	DirectX::SimpleMath::Vector3 getVelocity()const;
	int getTeam() const;
	int getOnlineID()const;
	bool getHitByBat()const;
	float getSpeed()const;
	void getAnimSpeed(float& speed);

	//Item related functions
	bool pickupItem(const std::vector <Item*>& items, const std::vector <Component*>& components);
	Item* getItem()const;
	void addItem(Item* itemToHold);
	int getItemOnlineType()const;
	int getItemOnlineId()const;
	bool isHoldingItem()const;
	void releaseItem();
	bool isHoldingComp();
	bool getFlip() const;

	//Collision and checks
	void hitByBat(const reactphysics3d::Vector3& force);
	bool checkForStaticCollision(const std::vector<Planet*>& gameObjects, const std::vector<SpaceShip*>& spaceShips);
	bool checkSwimStatus(const std::vector<Planet*>& planets);
	bool raycast(const std::vector<SpaceShip*>& gameObjects, const std::vector<Planet*>& planets, DirectX::XMFLOAT3& hitPos, DirectX::XMFLOAT3& hitNormal);
	bool withinRadius(Item* itemToLookWithinRadius, const float& radius) const;
	void colliedWIthComponent(const std::vector<Component*>& components);
	void orbiting();

	void stateMachine(const float dt);
	void giveItemMatrix();

	//Updating and rendering
	void drawIcon();
	void drawParticles();
	void drawFresnel(float interval = 0.2);
	//virtual void draw() override;
	void update();
	void requestingPickUpItem(const std::vector<Item*>& items);
	void updateController();
	playerMovement* getPlayerMovementInfo();

	void pushPlayer(std::vector<Player*>& players);
	void addForceToPlayer(const DirectX::SimpleMath::Vector3& pushForce);
	void itemRecvFromServer(Item* item);
};