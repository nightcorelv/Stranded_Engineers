#include "stdafx.h"
#include "Game.h"
#include "DirectXMathHelper.h"
#include "SendingDataEvent.h"
#include "MemoryLeackChecker.h"
#include "SoundCollection.h"
#include <filesystem>
#include "ErrorLog.h"



Game::Game(ID3D11DeviceContext* immediateContext, ID3D11Device* device, IDXGISwapChain* swapChain, HWND& window, UINT WIDTH, UINT HEIGHT,
	const int NROFPLAYERS, Client* client, int& currentTeam)
	:camera(Camera()), immediateContext(immediateContext), velocity(DirectX::XMFLOAT3(0, 0, 0)), manager(ModelManager(device)),
	currentMinigame(MiniGames::COMPONENTCOLLECTION), NROFPLAYERS(NROFPLAYERS)
{
	srand((UINT)time(0));

	this->HEIGHT = HEIGHT;
	this->WIDTH = WIDTH;
	this->packetEventManager = new PacketEventManager();
	gameMusic.load(L"../Sounds/Gold Rush Final.wav");
	gameMusic.play(true);
	gameMusic.setVolume(0.75f);
	this->client = client;
	//m�ste raderas******************

	if (IFONLINE)
	{
		std::cout << "Game is setup for " << std::to_string(NROFPLAYERS) << std::endl;
		circularBuffer = client->getCircularBuffer();
	}
	else
	{
		this->client = new Client("192.168.43.251");
	}


	//Setup rendering
	basicRenderer.initiateRenderer(immediateContext, device, swapChain, GPU::windowWidth, GPU::windowHeight);
	
	//Setup Lights
	ltHandler.addLight(DirectX::XMFLOAT3(-120, 0, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 1, 0), 1);
	ltHandler.addLight(DirectX::XMFLOAT3(16 + 7, 42 + 17, 12 + 7), DirectX::XMFLOAT3(0, 0.3f, 1.0f), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0), 2);
	ltHandler.addLight(DirectX::XMFLOAT3(-10 - 5, -45 - 17, -10 - 7), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0, 1, 0), 2);	
	
	//Temp? initiation of animated mesh

	//manager.loadMeshAndBoneData("../Meshes/pinto_Run.fbx");
	manager.loadMeshData("../Meshes/goblin2.fbx");
	
	this->manager.loadMeshData("../Meshes/goblin2.fbx");
	this->manager.getMeshData("../Meshes/goblin2.fbx", vBuff, iBuff, subMeshRanges, verticies);
	manager.loadMeshData("../Meshes/Sphere_with_normal.fbx");
	manager.getMeshData("../Meshes/Sphere_with_normal.fbx", vBuff, iBuff, subMeshRanges, verticies);
	manager.addTexture("p6n.png");

	this->tmpMesh2 = new Mesh(vBuff, iBuff,subMeshRanges, verticies);

	manager.loadMeshData("../Meshes/Sphere_with_normal2.fbx");
	manager.getMeshData("../Meshes/Sphere_with_normal2.fbx", vBuff, iBuff, subMeshRanges, verticies);
	this->tmpMesh3 = new Mesh(vBuff, iBuff,subMeshRanges, verticies);

	manager.loadMeshData("../Meshes/Sphere_with_normal3.fbx");
	manager.getMeshData("../Meshes/Sphere_with_normal3.fbx", vBuff, iBuff, subMeshRanges, verticies);
	this->tmpMesh4 = new Mesh(vBuff, iBuff,subMeshRanges, verticies);
	

	manager.loadMeshAndBoneData("../Meshes/anim/character1_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character1_run.fbx", "../Meshes/anim/character1_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character1_run_fast.fbx", "../Meshes/anim/character1_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character1_throw.fbx", "../Meshes/anim/character1_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character1_attack.fbx", "../Meshes/anim/character1_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character1_fly.fbx", "../Meshes/anim/character1_idle.fbx");

	manager.loadMeshAndBoneData("../Meshes/anim/character2_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character2_run.fbx", "../Meshes/anim/character2_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character2_run_fast.fbx", "../Meshes/anim/character2_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character2_throw.fbx", "../Meshes/anim/character2_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character2_attack.fbx", "../Meshes/anim/character2_idle.fbx");
	manager.AdditionalAnimation("../Meshes/anim/character2_fly.fbx", "../Meshes/anim/character2_idle.fbx");

	ID3D11ShaderResourceView* blueTeamColour = this->manager.getSrv("../Textures/Kosmonaut_K1SG_Diffuse.png");
	ID3D11ShaderResourceView* redTeamColour = this->manager.getSrv("../Textures/Kosmonaut_K1SG_Diffuse.png");
	AnimationData doNotUseT;
	this->manager.getAnimData("../Meshes/anim/character2_idle.fbx", vBuff, iBuff, subMeshRanges, verticies, doNotUseT);
	tmpMesh = new Mesh(vBuff, iBuff, subMeshRanges, verticies);
	AnimationData doNotUse;
	this->manager.getAnimData("../Meshes/anim/character1_idle.fbx", vBuff, iBuff, subMeshRanges, verticies, doNotUse);
	otherTmpMesh = new Mesh(vBuff, iBuff, subMeshRanges, verticies);
	

	animData = doNotUseT;
	//Load game objects
	this->loadObjects();

	//Setup players
	if (IFONLINE)
	{
		int thingstoLoad = 0;
		//client->connectToServer();
		int playerId = client->getPlayerId();
		/*while (playerId <= -1 || playerId >= 9)
		{*/
			//playerId = packetEventManager->handleId(client->getCircularBuffer(), this->planetVector, physWorld, meshes, spaceShips, gameObjects,this->field, thingstoLoad);
			//std::cout << "Game.cpp, playerId: " << std::to_string(playerId) << std::endl;
		//}
		//int playerid = client->initTEMPPLAYERS();
		std::cout << "The recv player id from client: " << std::to_string(playerId) << std::endl;
		this->client->setClientId(playerId);
		int offset = 10;
		int dude = (NROFPLAYERS) / 2;
		Mesh* useMeshForPlayer = nullptr;
		for (int i = 0; i < NROFPLAYERS; i++)//initialize players 
		{
			Player* tmpPlayer = nullptr;
			if (dude < i + 1)
			{
				useMeshForPlayer = tmpMesh;
				//change anim
				tmpPlayer = new Player(useMeshForPlayer, doNotUseT, DirectX::SimpleMath::Vector3(7, 42, 12), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
					0, i, client, (int)(dude < i + 1), redTeamColour, blueTeamColour, planetGravityField);
				std::vector<ID3D11ShaderResourceView*> allPlayerTextures;
				this->manager.getTextureMaps("../Meshes/anim/character2_idle.fbx", allPlayerTextures);
				tmpPlayer->setTextures(allPlayerTextures);
			}
			else
			{
				useMeshForPlayer = otherTmpMesh;
				tmpPlayer = new Player(useMeshForPlayer, doNotUse, DirectX::SimpleMath::Vector3(-4, -42, -10), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
					0, i, client, (int)(dude < i + 1), redTeamColour, blueTeamColour, planetGravityField);
				std::vector<ID3D11ShaderResourceView*> allPlayerTextures;
				this->manager.getTextureMaps("../Meshes/anim/character1_idle.fbx", allPlayerTextures);
				tmpPlayer->setTextures(allPlayerTextures);
			}
			
			tmpPlayer->setOnlineID(i);

			if (playerId != i)
			{
				physWorld.addPhysComponent(tmpPlayer, reactphysics3d::CollisionShapeName::BOX);
				players.push_back(tmpPlayer);
			}
			else
			{
				std::cout << "Player online id: " << std::to_string(i) << " \n";
				currentPlayer = tmpPlayer;

				/*if (i == 0 || i == 1)
				{
					currentPlayer = new Player(otherTmpMesh, doNotUse, DirectX::SimpleMath::Vector3(-4, -42, -10), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
						1, playerId, client, (int)(dude < i + 1), redTeamColour, blueTeamColour, planetGravityField);
					std::vector<ID3D11ShaderResourceView*> allPlayerTextures;
					this->manager.getTextureMaps("../Meshes/anim/character2_idle.fbx", allPlayerTextures);
					currentPlayer->setTextures(allPlayerTextures);
				}
				else
				{
					currentPlayer = new Player(tmpMesh, doNotUseT, DirectX::SimpleMath::Vector3(7, 42, 12), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
						1, playerId, client, (int)(dude < i + 1), redTeamColour, blueTeamColour, planetGravityField);
					std::vector<ID3D11ShaderResourceView*> allPlayerTextures;
					this->manager.getTextureMaps("../Meshes/anim/character1_idle.fbx", allPlayerTextures);
					currentPlayer->setTextures(allPlayerTextures);
				}*/

				//currentPlayer->addData(animData);
				currentPlayer->setOnlineID(i);
				players.push_back(currentPlayer);
			}
			std::cout << "Dude: " << (int)(dude < i + 1) << "\n";
		}

		std::cout << "num players" << this->players.size() << "\n";

		gamePad = new DirectX::GamePad();
		currentPlayer->setGamePad(gamePad);
		std::cout << "clear buffer before waiting for data from server\n";
		circularBuffer->clearBuffer();
		while (thingstoLoad != 5)
		{
			std::cout << "thingstoLoad: " << thingstoLoad << std::endl;
			packetEventManager->handleId(client->getCircularBuffer(), this->planetVector, physWorld, meshes, spaceShips, gameObjects, field, thingstoLoad);
		}
		
		for (int i = 0; i < spaceShips.size(); i++)
		{
			spaceShips[i]->setSpaceShipRotationRelativePlanet(planetVector[0]->getGravityField());	
			spaceShips[i]->resetIcon();
		}
		
		for (int i = 0; i < players.size(); i++)
		{
			players[i]->setGravityField(planetVector[0]->getGravityField());
		}
		
		DoneLoading sendingConfirm;
		sendingConfirm.packetId = PacketType::DONELOADING;
		client->sendStuff<DoneLoading>(sendingConfirm);
		
		currentTeam = currentPlayer->getTeam();

	}
	landingUi.makeGamePad(gamePad);

	currentPlayer->setPhysComp(physWorld.getPlayerBox());
	currentPlayer->getPhysComp()->setParent(currentPlayer);
	currentPlayer->setScale(0.85f);
	for (int i = 0; i < players.size(); i++)
	{
		players[i]->setGravityField(planetGravityField);
		players[i]->setScale(0.85f);
	}
	
	//check the handle id for data ex(Planets, SpaceShips)
	field = nullptr;
	oldField = field;
	TimeStruct UwuYouSowarm;
	UwuYouSowarm.resetStartTime();
	while (UwuYouSowarm.getTimePassed(1.0f));

	//Init delta time
	currentTime = std::chrono::system_clock::now();
	lastUpdate = currentTime;
	//gamePad = std::make_unique<DirectX::GamePad>();
	playerVecRenderer.setPlayer(currentPlayer, arrow);
	currentTime = std::chrono::system_clock::now();
	dt = ((std::chrono::duration<float>)(currentTime - lastUpdate)).count();
	serverStart = std::chrono::system_clock::now();
	this->window = &window;
	HudUI::SetGamePad(gamePad);
	
	//Set gamepad for item
	for (int i = 0; i < items.size(); i++)
	{
		items[i]->setGamepad(this->gamePad);
	}
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->setGamepad(this->gamePad);
	}
	
	
	//Setup fade in and delta time
	ui.count = 1.0f;
	ui.setOpacity(false);
	Time::Start();
	Time::Reset();
}

//-------------------------------------------------------------------- DESTRUCTOR --------------------------------------------------------------------//


Game::~Game()
{
	delete packetEventManager;
	for (int i = 0; i < players.size(); i++)
	{
		delete players[i];
	}
	for (int i = 0; i < this->gameObjects.size(); i++)
	{
		if (this->gameObjects.at(i) != nullptr)
		{
			delete this->gameObjects.at(i);
		}
	}
	for (int i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
	}
	for (int i = 0; i < planetVector.size(); i++)
	{
		delete planetVector[i];
	}

	if (captureZone != nullptr) delete captureZone;
	if (gamePad != nullptr) delete gamePad;
	if (client != nullptr)
	{
		std::cout << "Deleted clients\n";
		delete client;
	}
	delete tmpMesh;
	delete tmpMesh2;
	delete tmpMesh3;
	delete tmpMesh4;
	delete otherTmpMesh;
	delete asteroids;
	delete arrow;
	delete planetGravityField;
	
	
}

//-------------------------------------------------------------------- SCENE SETUP --------------------------------------------------------------------//

void Game::loadObjects()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//Variables 
	SpaceShip* spaceShipRed;
	SpaceShip* spaceShipBlue;
	Component* component;
	Potion* potion;
	//Grenade* grenade2;


	//Load extra textures	
	MaterialLibrary::LoadDefault();
	MaterialLibrary::LoadMaterial("spaceshipTexture1.jpg");
	MaterialLibrary::LoadMaterial("spaceshipTexture2.jpg");
	MaterialLibrary::LoadMaterial("pintoRed.png");
	MaterialLibrary::LoadMaterial("pintoBlue.png");
	MaterialLibrary::LoadMaterial("Red.png");
	MaterialLibrary::LoadMaterial("olive.jpg");

	//Meshes vector contents
	meshes.push_back(new Mesh("../Meshes/Sphere"));
	meshes.push_back(new Mesh("../Meshes/reverseSphere"));
	meshes.push_back(new Mesh("../Meshes/pinto"));
	meshes.push_back(new Mesh("../Meshes/potion"));
	meshes.push_back(new Mesh("../Meshes/rocket"));
	meshes.push_back(new Mesh("../Meshes/bat"));
	meshes.push_back(new Mesh("../Meshes/component"));
	meshes.push_back(new Mesh("../Meshes/grenade"));
	meshes.push_back(new Mesh("../Meshes/arrow"));
	meshes.push_back(new Mesh("../Meshes/zone"));
	meshes.push_back(new Mesh("../Meshes/saturn"));
	meshes.push_back(new Mesh("../Meshes/N1"));
	meshes.push_back(new Mesh("../Meshes/grenade"));
	meshes.push_back(new Mesh("../Meshes/hazard"));


	//SOLAR SYSTEM SETUP
	if (!IFONLINE)
	{
		float planetSize = 40.f; // SET DIFFERENT GRAV-FACTORS FOR THE PLANETS AND DIFFERENT TEXTURES!
		planetVector.emplace_back(new Planet(tmpMesh2, DirectX::XMFLOAT3(planetSize, planetSize, planetSize), DirectX::XMFLOAT3(0.f, 0.f, 0.f), (4.0f * 9.82f), meshes[1]));
		planetVector.back()->setPlanetShape(&physWorld);
		planetVector.emplace_back(new Planet(tmpMesh3, DirectX::XMFLOAT3(planetSize , planetSize , planetSize ), DirectX::XMFLOAT3(60.f, 60.f, 60.f), (4.0f * 9.82f), meshes[1]));
		planetVector.back()->setPlanetShape(&physWorld);
		planetVector.back()->setSrv(this->manager.getSrv("p6.png"));
		planetVector.back()->setNormalMap(this->manager.getSrv("p6n.png"));
		planetVector.emplace_back(new Planet(tmpMesh4, DirectX::XMFLOAT3(planetSize * 0.6f, planetSize * 0.6f, planetSize * 0.6f), DirectX::XMFLOAT3(-130.f, -130.f, 130.f), (4.0f * 9.82f), meshes[1]));
		planetVector.back()->setPlanetShape(&physWorld);
		physWorld.setPlanets(planetVector);

		for (auto& planet :planetVector)
		{
			int randomPlanetIndex = rand() % 11; //range 0 to 12

			const std::string path_c = std::string("p") + std::to_string(randomPlanetIndex) + std::string(".png");
			const std::string path_n = std::string("p") + std::to_string(randomPlanetIndex) + std::string("n.png");

			std::cout << path_c << std::endl;
			std::cout << path_n << std::endl;
			auto colorSRV = loadTexture(path_c);
			planet->setSrv(colorSRV);

			auto normalSRV = loadTexture(path_n);
			planet->setNormalMap(normalSRV);
			
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			int randomPlanetIndex = rand() % 11; //range 0 to 12

			const std::string path_c = std::string("p") + std::to_string(randomPlanetIndex) + std::string(".png");
			const std::string path_n = std::string("p") + std::to_string(randomPlanetIndex) + std::string("n.png");

			std::cout << path_c << std::endl;
			std::cout << path_n << std::endl;
			planetSRV_online.emplace_back(loadTexture(path_c));
			planetSRV_online.emplace_back(loadTexture(path_n));

		}
	}

	for (int i = 0; i < 3; i++)
	{
		int randomPlanetIndex = rand() % 11; //range 0 to 12

		const std::string path_c = std::string("p") + std::to_string(randomPlanetIndex) + std::string(".png");
		const std::string path_n = std::string("p") + std::to_string(randomPlanetIndex) + std::string("n.png");

		std::cout << path_c << std::endl;
		std::cout << path_n << std::endl;
		planetSRV_online.emplace_back(loadTexture(path_c));
		planetSRV_online.emplace_back(loadTexture(path_n));

	}

	asteroids = new AsteroidHandler(meshes[0]);
	planetGravityField = new GravityField(4.f * 9.82f, DirectX::XMFLOAT3(0.f, 0.f, 0.f), 40.f);

	//Make sure the physics world has access to the planets

	//CREATE ITEMS 	//Sphere, reverseSphere, pinto, potion, rocket, bat, component, grenade, arrow
	if (!IFONLINE)
	{
		potion = new Potion(meshes[3], Vector3(0, 0, -42), Vector3(0.0f, 0.0f, 0.0f), POTION, 0, planetGravityField);
		baseballBat = new BaseballBat(meshes[5], Vector3(0, 0, 42), Vector3(0.0f, 0.0f, 0.0f), BAT, 0, planetGravityField);
		grenade = new Grenade(meshes[7], meshes[12], DirectX::SimpleMath::Vector3(42, 0, 0), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), GRENADE, 0, planetGravityField);
		//grenade2 = new Grenade(meshes[7], meshes[12], DirectX::SimpleMath::Vector3(45, 0, 0), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), GRENADE, 0, planetGravityField);

		//EMPLACE ITEMS
		items.emplace_back(potion);
		items.emplace_back(baseballBat);
		items.emplace_back(grenade);
		//items.emplace_back(grenade2);
	}
	arrow = new Arrow(meshes[8], DirectX::SimpleMath::Vector3(6969.f, 6969.f, 6969.f));

	for (int i = 0; i < items.size(); i++)
	{
		gameObjects.emplace_back(items[i]);
	}



	//Add phys components
	for (int i = 0; i < gameObjects.size(); i++)
	{
		physWorld.addPhysComponent(gameObjects[i], reactphysics3d::CollisionShapeName::BOX);
	}
	//SPACE SHIPS
	if (!IFONLINE)
	{
		spaceShipRed = new SpaceShip(meshes[11], Vector3(-7.81178f, -37.8586f, -8.50119f), ROCKET, 0, planetGravityField, meshes[9], { 2.5, 2.5, 2.5 });
		spaceShipBlue = new SpaceShip(meshes[10], Vector3(13.5817f, 35.9383f, 9.91351f), ROCKET, 1, planetGravityField, meshes[9], { 2.5, 2.5, 2.5 });
		spaceShips.emplace_back(spaceShipRed);
		spaceShips.emplace_back(spaceShipBlue);

		//Offline comonent
		component = new Component(meshes[6], DirectX::SimpleMath::Vector3(0, -42, 0), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f), COMPONENT, 0, planetGravityField);
		component->setScale(2.0f);
		components.emplace_back(component);
		gameObjects.emplace_back(component);
		physWorld.addPhysComponent(component, reactphysics3d::CollisionShapeName::BOX);
	}
	for (int i = 0; i < spaceShips.size(); i++)
	{
		physWorld.addPhysComponent(spaceShips[i], reactphysics3d::CollisionShapeName::BOX, DirectX::XMFLOAT3(0.75f, 4 * 0.75f, 0.75f));
		spaceShips[i]->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
		gameObjects.emplace_back(spaceShips[i]);
	}

	//Hazards
	if (!IFONLINE)
	{
		testBox = new HazardBox(meshes[13], DirectX::SimpleMath::Vector3(42, 0, 0), DirectX::SimpleMath::Vector3(0, 0, 0), planetGravityField, { 4,4,4 });
		gameObjects.emplace_back(testBox);
		physWorld.addPhysComponent(gameObjects.back(), reactphysics3d::CollisionShapeName::BOX);
		gameObjects.back()->getPhysComp()->setType(reactphysics3d::BodyType::STATIC);
	}

	//Initilize player
	if (!currentPlayer && !IFONLINE)
	{
		ID3D11ShaderResourceView* blueTeamColour = nullptr;
		ID3D11ShaderResourceView* redTeamColour = nullptr;
		currentPlayer = new Player(tmpMesh, animData, DirectX::SimpleMath::Vector3(7, 42, 12), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
			1, client->getPlayerId(), client, 0, redTeamColour, blueTeamColour, planetGravityField);
		std::vector<ID3D11ShaderResourceView*> allPlayerTextures;
		this->manager.getTextureMaps("../Meshes/anim/character2_idle.fbx", allPlayerTextures);
		
		currentPlayer->setTextures(allPlayerTextures);

		players.emplace_back(currentPlayer);
		gamePad = new GamePad();
		currentPlayer->setGamePad(gamePad);
	}

	//Set items baseball bat
	if (!IFONLINE)
	{
		baseballBat->setPlayer(currentPlayer);
		baseballBat->setGameObjects(gameObjects);

		//Set items grenade
		grenade->setGameObjects(gameObjects);
	}
}

//-------------------------------------------------------------------- DRAW FUNCTIONS --------------------------------------------------------------------//

void Game::drawShadows()
{
	int nrofLights = ltHandler.getNrOfLights();
	//Draw object shadow
	for (int i = 0; i < nrofLights; i++)
	{
		ltHandler.drawShadows(i, gameObjects);
	}
	//Draw planet shadow
	basicRenderer.normaltasseletion(camera, true);
	for (int i = 0; i < nrofLights; i++)
	{
		ltHandler.drawShadows(i, planetVector);
	}
	basicRenderer.resetTopology();
	//For animated meshes
	basicRenderer.bindAnimVs();
	for (int i = 0; i < nrofLights; i++)
	{
		ltHandler.drawShadows(i, players);
	}

	//Draw depth stencil
	//basicRenderer.depthPrePass();
	//ltHandler.drawShadows(0, gameObjects, &camera);
	//GPU::immediateContext->OMSetDepthStencilState(nullptr, 0);
}

void Game::drawObjects(bool drawDebug)
{
	//Bind light
	ltHandler.bindLightBuffers();
	
	//Draw Game objects
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->draw();
	}

	//Nödvändig??????????
	for (int i = 0; i < onlineItems.size(); i++)
	{
		onlineItems[i]->draw();
	}
	
	//Draw planets
	basicRenderer.normaltasseletion(camera);
	for (int i = 0; i < planetVector.size(); i++)
	{
		if (i == camera.getCollidedWith()) continue;
		planetVector[i]->drawObjectWithNormalMap();
	}
	basicRenderer.resetTopology();
	asteroids->drawAsteroids();


	//Draw with Ambient only shader
	basicRenderer.bindAmbientShader();
	arrow->draw();

	//Draw light debug meshes
	if (drawDebug)
	{
		ltHandler.drawDebugMesh();
	}
	
	
	//Animated meshes
	basicRenderer.changeToAnimation();
	
	for (int i = 0; i < players.size(); i++)
	{
			players[i]->drawSubMeshesWithTexture();
	}
	
}

void Game::drawIcons()
{
	for (int i = 0; i < items.size(); i++)
	{
		this->items[i]->drawIcon();
	}
	for (int i = 0; i < components.size(); i++)
	{
		this->components[i]->drawIcon();
	}
	for (int i = 0; i < onlineItems.size(); i++)
	{
		this->onlineItems[i]->drawIcon();
	}
	for (int i = 0; i < players.size(); i++)
	{
		players[i]->drawIcon();
	}
	if (currentMinigame == COMPONENTCOLLECTION)
	{
		for (int i = 0; i < spaceShips.size(); i++)
		{
			spaceShips[i]->drawQuad();
		}
	}
}

void Game::drawParticles()
{
	for (int i = 0; i < items.size(); i++)
	{
		this->items[i]->drawParticles();
	}
	//Used??
	for (int i = 0; i < onlineItems.size(); i++)
	{
		this->onlineItems[i]->drawParticles();
	}

	for (int i = 0; i < components.size(); i++)
	{
		this->components[i]->drawParticles();
	}
	for (int i = 0; i < spaceShips.size(); i++)
	{
		spaceShips[i]->drawParticles();
	}
	//test
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->getId() == ObjID::HAZARD)
		{
			HazardBox* hazard = (HazardBox*)gameObjects[i];
			hazard->drawParticles();
		}
	}

	//Bind special shader for player
	basicRenderer.playerParticlePass();
	for (int i = 0; i < players.size(); i++)
	{
		players[i]->drawParticles();
	}
}


void Game::drawNormalObjects()
{
	//Draw normal mapped objects here

	//
	// 	basicRenderer.setUpSceneNormalMap(this->camera);

	//Animated meshes
	//basicRenderer.changeToAnimation();
	//for (int i = 0; i < players.size(); i++)
	//{
	//	//players[i]->draw();
	//	if (i == 0)
	//	{
	//		players[i]->drawSubMeshesWithTexture();
	//	}
	//}

}

void Game::drawFresnel()
{
	//Regular
	for (int i = 0; i < spaceShips.size(); i++)
	{
		spaceShips[i]->drawFresnel();
	}
	if (captureZone != nullptr) captureZone->drawFresnel();
	for (int i = 0; i < items.size(); i++)
	{
		items[i]->drawFresnel();
	}

	for (int i = 0; i < components.size(); i++)
	{
		components[i]->drawFresnel();
	}
	
	asteroids->drawExplosions();

	for (int i = 0; i < onlineItems.size(); i++)
	{
		if (onlineItems[i]->getId() == ObjID::GRENADE)
		{
			Grenade* tempNade = (Grenade*)onlineItems[i];
			tempNade->drawFresnel();
		}
	}

	//!!Test!!
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->getId() == ObjID::HAZARD)
		{
			HazardBox* hazard = (HazardBox*)gameObjects[i];
			hazard->drawFresnel();
		}
	}

	//Inverse
	basicRenderer.invFresnelPrePass();
	for (int i = 0; i < planetVector.size(); i++)
	{
		if (i == camera.getCollidedWith()) continue;
		planetVector[i]->drawAtmosphere();
	}
}


//-------------------------------------------------------------------- MISC FUNCTIONS --------------------------------------------------------------------//


ID3D11ShaderResourceView* Game::loadTexture(const std::string& fileName)
{
	manager.addTexture(fileName);
	return manager.getSrv(fileName);
}

void Game::randomizeObjectPos(GameObject* object)
{
	DirectX::SimpleMath::Vector3 randomPos = {};

	int xPos = rand() % 201 - 100;
	int yPos = rand() % 201 - 100;
	int zPos = rand() % 201 - 100;

	randomPos.x = (float)xPos;
	randomPos.y = (float)yPos;
	randomPos.z = (float)zPos;

	randomPos.Normalize();
	randomPos *= 100;

	object->setPos(randomPos);
}

//-------------------------------------------------------------------- GAMESTATE FUNCTIONS --------------------------------------------------------------------//


GAMESTATE Game::updateComponentGame()
{
	currentPlayer->stateMachine(dt);

	//if (asteroids->ifTimeToSpawnAsteroids()) asteroids->spawnAsteroids(planetVector[0]);
	//asteroids->updateAsteroids(dt, planetVector, gameObjects);
	
	//Calculate gravity factor
	if (planetVector.size() > 0) field = planetVector[0]->getClosestField(planetVector, currentPlayer->getPosV3());
	if (field != oldField) { changedPlanet = true; currentPlayer->setGravityField(this->field); }
	else changedPlanet = false;
	oldField = field;

	if (planetVector.size() > 0)
	{
		for (int i = 0; i < gameObjects.size(); i++) gameObjects[i]->setGravityField(planetVector[0]->getClosestField(planetVector, gameObjects[i]->getPosV3()));
		for (int i = 0; i < onlineItems.size(); i++) onlineItems[i]->setGravityField(planetVector[0]->getClosestField(planetVector, onlineItems[i]->getPosV3()));
	}


	//Raycasting
	static DirectX::XMFLOAT3 hitPos;
	static DirectX::XMFLOAT3 hitNormal;
	hitPos = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	hitNormal = DirectX::XMFLOAT3(grav.x, grav.y, grav.z);
	bool testingVec;
	testingVec = this->currentPlayer->raycast(spaceShips, planetVector, hitPos, hitNormal);
	if (testingVec || currentPlayer->getHitByBat()) 
		currentPlayer->resetVelocity();
	else
	{
		grav = field->calcGravFactor(currentPlayer->getPosV3());
		currentPlayer->updateVelocity(getScalarMultiplicationXMFLOAT3(dt, grav));
	}

	//Player functions
	currentPlayer->pushPlayer(players);
	//currentPlayer->velocityMove(dt);
	//if (planetVector.size() > 0 && !testingVec) 
	currentPlayer->move(DirectX::XMVector3Normalize(camera.getForwardVector()), DirectX::XMVector3Normalize(camera.getRightVector()), dt);

	currentPlayer->rotate(hitNormal, testingVec, changedPlanet);
	currentPlayer->move(DirectX::XMVector3Normalize(camera.getForwardVector()), DirectX::XMVector3Normalize(camera.getRightVector()), dt);
	currentPlayer->moveController(DirectX::XMVector3Normalize(camera.getForwardVector()), DirectX::XMVector3Normalize(camera.getRightVector()), dt);
	currentPlayer->checkForStaticCollision(planetVector, spaceShips);
	currentPlayer->checkSwimStatus(planetVector);
	currentPlayer->orbiting();
	currentPlayer->velocityMove(dt);
	currentPlayer->setSpeed(25.f);

	if (!IFONLINE) currentPlayer->pickupItem(items, components);
	else currentPlayer->requestingPickUpItem(onlineItems);

	//Update item checks
	for (int i = 0; i < onlineItems.size(); i++)
	{
		int id = onlineItems[i]->getId();
		switch (id)
		{
		case ObjID::GRENADE:
		{
			Grenade* tempNade = (Grenade*)onlineItems[i];
			tempNade->updateExplosionCheck();
			tempNade->applyExplosionForce();
		}	
		break;
		case ObjID::POTION:
		{
			Potion* tempPotion = (Potion*)onlineItems[i];
			if (tempPotion->timerGoing()) currentPlayer->setSpeed(1.f);
		}	
		break;
		}
	}


	//Update item checks
	for (int i = 0; i < items.size(); i++)
	{
		id = items[i]->getId();
		switch (id)
		{
		case ObjID::GRENADE:
		{
			Grenade* tempNade = (Grenade*)items[i];
			tempNade->updateExplosionCheck();
			tempNade->applyExplosionForce();
			if (tempNade->getExploded() == true)
			{
				randomizeObjectPos(tempNade);
				tempNade->setExploded(false);
			}

		} break;

		case ObjID::POTION:
		{
			Potion* tempPotion = (Potion*)items[i];
			if (tempPotion->timerGoing())
			{
				currentPlayer->setSpeed(1.f);
			}
		}	break;
		}
	}

	//sending data to server
	if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - serverStart)).count() > serverTimerLength && client->getIfConnected())
	{
		SendingDataEvent(client, currentPlayer, players);
		serverStart = std::chrono::system_clock::now();
	}

	//Physics related functions
	if (!IFONLINE) physWorld.update(dt);
	for (int i = 0; i < players.size(); i++)
	{
		players[i]->updateMatrixOnline();
		players[i]->update();
	}


	//Updates gameObject physics components
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->update();
	}

	//Setting the camera at position
	if (!velocityCamera) camera.moveVelocity(currentPlayer, dt);
	else camera.collisionMove(currentPlayer, planetVector, currentPlayer->getFlip(), dt);

	//Check Spaceship completion
	for (int i = 0; i < spaceShips.size(); i++)
	{
		if (spaceShips[i]->getCompletion())
		{
			if (currentPlayer->getTeam() == i) { camera.winScene(spaceShips[i]->getPosV3(), spaceShips[i]->getRot()); currentPlayer->setVibration(0.1f, 0.1f); }
			this->spaceShips[i]->flyAway(dt);
			
			endTimer += dt;
			camera.setHaveWon(true);
			arrow->removeArrow();
			if (currentPlayer->getTeam() == i) this->currentPlayer->setPos(DirectX::XMFLOAT3(6969, 6969, 6969));
		}
	}

	//Arrow pointing to spaceship
	if (currentPlayer->isHoldingComp())
	{
		for (int i = 0; i < spaceShips.size(); i++)
		{
			if (currentPlayer->getTeam() == i)
			{
				arrow->moveWithCamera(currentPlayer->getPosV3(), DirectX::XMVector3Normalize(camera.getForwardVector()), currentPlayer->getUpVector(), currentPlayer->getRotationMX());
				this->arrow->showDirection(spaceShips[i]->getPosV3(), planetGravityField->calcGravFactor(arrow->getPosition()));
			}
		}
	}
	//Arrow pointing to component
	else if (onlineItems.size() > 0)
	{
		for (int i = 0; i < onlineItems.size(); i++)
		{
			if (onlineItems[i]->getId() == ObjID::COMPONENT)
			{
				arrow->moveWithCamera(currentPlayer->getPosV3(), DirectX::XMVector3Normalize(camera.getForwardVector()), currentPlayer->getUpVector(), currentPlayer->getRotationMX());
				grav = field->calcGravFactor(currentPlayer->getPosV3());
				this->arrow->showDirection(onlineItems[i]->getPosV3(), grav);
				break;
			}
		}
	}
	else if (components.size() > 0)
	{
		arrow->moveWithCamera(currentPlayer->getPosV3(), DirectX::XMVector3Normalize(camera.getForwardVector()), currentPlayer->getUpVector(), currentPlayer->getRotationMX());
		grav = field->calcGravFactor(currentPlayer->getPosV3());
		this->arrow->showDirection(components[0]->getPosV3(), grav);
	}
	currentPlayer->colliedWIthComponent(components);

	if (!IFONLINE)
	{
		for (int i = 0; i < spaceShips.size(); i++)
		{
			for (int j = 0; j < components.size(); j++)
			{
				if (spaceShips[i]->detectedComponent(components[j]))
				{
					if (currentPlayer->getItem() != nullptr)
					{
						if (currentPlayer->getItem()->getId() == ObjID::COMPONENT)
						{
							currentPlayer->releaseItem();
						}
					}
					randomizeObjectPos(components[j]);
					spaceShips[i]->addComponent();
					spaceShips[i]->setAnimate(true);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < spaceShips.size(); i++)
		{
			for (int j = 0; j < onlineItems.size(); j++)
			{
				spaceShips[i]->detectedComponent(onlineItems[j]);
			}
		}
	}
	//Check winstate
	if (endTimer > 0.4)///change
	{
		ui.moveSprite();
		this->currentPlayer->setVibration(0.f, 0.f);
	}
	if (endTimer > 6.7f)
	{
		ui.fadeOut();
	}
	if (endTimer > 8)
	{
		for (int i = 0; i < spaceShips.size(); i++)
		{
			if (spaceShips[i]->isFinished())
			{
				if (!IFONLINE)
				{
					//Complete both ships
					for (int j = 0; j < spaceShips.size(); j++)
					{
						spaceShips[j]->completeShip();
					}
					//Change mode
					this->Game::startIntermission();
				}
			}
		}
	}

	//Play pickup animation
	for (int i = 0; i < spaceShips.size(); i++)
	{
		spaceShips[i]->animateOnPickup();
	}
	//Check if item icon should change to pickup icon 
	for (int i = 0; i < items.size(); i++)
	{
		this->items[i]->checkDistance((GameObject*)(currentPlayer));
	}
	for (int i = 0; i < onlineItems.size(); i++)
	{
		this->onlineItems[i]->checkDistance((GameObject*)(currentPlayer));
	}
	for (int i = 0; i < components.size(); i++)
	{
		this->components[i]->checkDistance((GameObject*)(currentPlayer));
	}

	return currentGameState;
}

GAMESTATE Game::startLanding()
{
	currentMinigame = LANDINGSPACESHIP;
	currentPlayer->setPos(DirectX::SimpleMath::Vector3(0.f, 62.f, 0.f));

	int index = currentPlayer->getTeam() + 1;
	int team = currentPlayer->getTeam();

	for (int i = 0; i < spaceShips.size(); i++)
	{
		if (i == currentPlayer->getTeam()) { index = currentPlayer->getTeam() + 1; team = currentPlayer->getTeam(); }
		else { index = (int)!currentPlayer->getTeam() + 1; team = (int)!currentPlayer->getTeam(); }
		DirectX::SimpleMath::Vector3 planetOffset = planetVector[index]->getPlanetPosition();
		planetOffset.Normalize();
		planetOffset *= 150.f;
		DirectX::SimpleMath::Vector3 newPos = planetVector[index]->getPlanetPosition() + planetOffset;
		spaceShips[team]->setPos(newPos);
		spaceShips[team]->setGravityField(planetVector[index]->getGravityField());
		spaceShips[team]->setRot(spaceShips[team]->getRotOrientedToGrav());
	}

	
	DirectX::SimpleMath::Vector3 lightDir = -DirectX::SimpleMath::Vector3(-80, -83, -89);
	lightDir.Normalize();


	ltHandler.setPosition(DirectX::XMFLOAT3(-80, -83, -89), 0);
	ltHandler.setDirection({lightDir}, 0);
	ltHandler.updateBuffers();

	ui.count = 1.0f;
	ui.setOpacity(false);
	fadedIn = false;

	landingMiniGamePoints = 0.f;
	return GAMESTATE::NOCHANGE;
}

GAMESTATE Game::updateLandingGame()
{
	if (!fadedIn)// fade in condition
	{
		if (!this->ui.fadeIn()) // is fading
		{
		}
		else // fade in complete
		{
			fadedIn = true;
		}
	}

	landingUi.setLandingScore((int)landingMiniGamePoints);

	//Here yo type the function below but replace testObject with your space ship
	camera.landingMinigameScene(planetVector[currentPlayer->getTeam() + 1], spaceShips[currentPlayer->getTeam()]->getPosV3(), spaceShips[currentPlayer->getTeam()]->getRot());
	
	//Moves the spaceShips
	spaceShips[currentPlayer->getTeam()]->fly(spaceShips[currentPlayer->getTeam()]->getUpDirection(), 0.5f * dt);
	spaceShips[!currentPlayer->getTeam()]->fly(spaceShips[!currentPlayer->getTeam()]->getUpDirection(), 0.5f * dt);

	if (landingUi.handleInputs(dt)) landingMiniGamePoints += 5.f*dt;
	if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - serverStart)).count() > serverTimerLength && client->getIfConnected())
	{
		std::cout << "Team score: " << teamScoreLandingMiniGame << "\nEnemy Team score: " << enemyTeamScoreLandingMiniGame << "\n";

		//Send data to server
		LandingMiniSendScoreToServer totalPoints = {};
		totalPoints.packetId = PacketType::LANDINGMINIGAMESENDSCORETOSERVER;
		totalPoints.playerId = currentPlayer->getOnlineID();
		totalPoints.scoreToServer = landingMiniGamePoints;
		client->sendStuff<LandingMiniSendScoreToServer>(totalPoints);
		serverStart = std::chrono::system_clock::now();
	}

	DirectX::SimpleMath::Vector3 vecToPlanet = spaceShips[currentPlayer->getTeam()]->getPosV3() - planetVector[currentPlayer->getTeam() + 1]->getPlanetPosition();

	if (getLength(vecToPlanet) <= planetVector[currentPlayer->getTeam() + 1]->getSize())
	{
		ui.count = 1.0f;
		ui.setOpacity(false);
		fadedIn = false;
		for (int i = 0; i < spaceShips.size(); i++)
		{
			DirectX::SimpleMath::Quaternion dx11Quaternion = DirectX::XMQuaternionRotationMatrix(spaceShips[i]->getRot());
			reactphysics3d::Quaternion reactQuaternion = reactphysics3d::Quaternion(dx11Quaternion.x, dx11Quaternion.y, dx11Quaternion.z, dx11Quaternion.w);
			spaceShips[i]->getPhysComp()->setPosition(reactphysics3d::Vector3(spaceShips[i]->getPosV3().x, spaceShips[i]->getPosV3().y, spaceShips[i]->getPosV3().z));
			spaceShips[i]->getPhysComp()->setRotation(reactQuaternion);

			if (currentPlayer->getTeam() == i)
			{
				currentPlayer->setPos(DirectX::XMFLOAT3(spaceShips[i]->getPos().x - 10.f, spaceShips[i]->getPos().y, spaceShips[i]->getPos().z));
				currentPlayer->setStartPosition(DirectX::XMFLOAT3(spaceShips[i]->getPos().x - 10.f, spaceShips[i]->getPos().y, spaceShips[i]->getPos().z));
			}
		}
		std::cout << "\nLANDING MINIGAME OVER!\nTOTAL SCORE:\nTeam score: " << teamScoreLandingMiniGame << "\nEnemy Team score: " << enemyTeamScoreLandingMiniGame << "\n";
		enemyTeamScoreLandingMiniGame = 0;
		teamScoreLandingMiniGame = 0;
		//Send data to server
		DoneWithGame requestStart;
		requestStart.packetId = PacketType::DONEWITHGAME;
		requestStart.playerID = currentPlayer->getOnlineID();
		requestStart.formerGame = MiniGames::LANDINGSPACESHIP;
		client->sendStuff<DoneWithGame>(requestStart);
		currentMinigame = MiniGames::DEFAULT;
		camera.setHaveWon(false);
		if (!IFONLINE)
		{
			currentGameState = this->updateKingOfTheHillGame();
		}
		ltHandler.setPosition({ -200,0,0 }, 0);
		ltHandler.updateBuffers();
	}
	return NOCHANGE;
}

GAMESTATE Game::updateKingOfTheHillGame()
{
	if (IFONLINE)
	{
		packetEventManager->PacketHandleEvents(circularBuffer, NROFPLAYERS, players, client->getPlayerId(), components, physWorld, gameObjects, planetGravityField, spaceShips, onlineItems, meshes, planetVector, captureZone, currentMinigame,
			teamScoreLandingMiniGame, enemyTeamScoreLandingMiniGame, client, dt, currentGameState, gamePad, tmpMesh2, tmpMesh3, tmpMesh4, planetSRV_online, teamScores);
	}

	if (!firstFrame)
	{
		Time::Start();
		Time::Reset();
		firstFrame = true;
		ui.count = 1.0f;
		ui.setOpacity(false);
		fadedIn = false;
		ui.resetReadySetGo();
	}
	
	if (!fadedIn)// fade in condition
	{
		if (!this->ui.fadeIn()) // is fading
		{
		}
		else // fade in complete
		{
			fadedIn = true;
		}
		landingUi.drawPointsForOtherGameModes(teamScores);
	}

	//Calculate gravity factor
	if (planetVector.size() > 0) field = planetVector[0]->getClosestField(planetVector, currentPlayer->getPosV3());
	if (field != oldField) { changedPlanet = true; currentPlayer->setGravityField(this->field); }
	else changedPlanet = false;
	oldField = field;
	if (planetVector.size() > 0) grav = field->calcGravFactor(currentPlayer->getPosV3());
	currentPlayer->updateVelocity(getScalarMultiplicationXMFLOAT3(dt, grav));

	if (planetVector.size() > 0)
	{
		for (int i = 0; i < gameObjects.size(); i++) gameObjects[i]->setGravityField(planetVector[0]->getClosestField(planetVector, gameObjects[i]->getPosV3()));
	}
	currentPlayer->stateMachine(dt);

	//Raycasting
	static DirectX::XMFLOAT3 hitPos;
	static DirectX::XMFLOAT3 hitNormal;
	hitPos = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	hitNormal = DirectX::XMFLOAT3(grav.x, grav.y, grav.z);
	bool testingVec;
	testingVec = this->currentPlayer->raycast(spaceShips, planetVector, hitPos, hitNormal);
	if (testingVec || currentPlayer->getHitByBat())
		currentPlayer->resetVelocity();
	else
	{
		grav = field->calcGravFactor(currentPlayer->getPosV3());
		currentPlayer->updateVelocity(getScalarMultiplicationXMFLOAT3(dt, grav));
	}

	//Player functions
	currentPlayer->pushPlayer(players);
	//currentPlayer->velocityMove(dt);
	//if (planetVector.size() > 0 && !testingVec) 
	currentPlayer->move(DirectX::XMVector3Normalize(camera.getForwardVector()), DirectX::XMVector3Normalize(camera.getRightVector()), dt);

	currentPlayer->rotate(hitNormal, testingVec, changedPlanet);
	currentPlayer->move(DirectX::XMVector3Normalize(camera.getForwardVector()), DirectX::XMVector3Normalize(camera.getRightVector()), dt);
	currentPlayer->moveController(DirectX::XMVector3Normalize(camera.getForwardVector()), DirectX::XMVector3Normalize(camera.getRightVector()), dt);
	currentPlayer->checkForStaticCollision(planetVector, spaceShips);
	currentPlayer->checkSwimStatus(planetVector);
	currentPlayer->orbiting();
	currentPlayer->velocityMove(dt);
	currentPlayer->setSpeed(25.f);

	//Check component pickup
	if (!IFONLINE) currentPlayer->pickupItem(items, components);
	else currentPlayer->requestingPickUpItem(onlineItems);

	//Update item checks
	for (int i = 0; i < items.size(); i++)
	{
		int id = items[i]->getId();
		switch (id)
		{
		case ObjID::GRENADE:
		{
			Grenade* tempNade = (Grenade*)items[i];
			tempNade->updateExplosionCheck();
			if (tempNade->getExploded() == true)
			{
				randomizeObjectPos(tempNade);
				tempNade->setExploded(false);
			}
		}	break;
		case ObjID::POTION:
		{
			Potion* tempPotion = (Potion*)items[i];
			if (tempPotion->timerGoing()) currentPlayer->setSpeed(50.f);
		}	break;
		}

	}

	for (int i = 0; i < onlineItems.size(); i++)
	{
		int id = onlineItems[i]->getId();
		switch (id)
		{
		case ObjID::GRENADE:
		{
			Grenade* tempNade = (Grenade*)onlineItems[i];
			tempNade->updateExplosionCheck();
			if (tempNade->getExploded() == true)
			{
				randomizeObjectPos(tempNade);
				tempNade->setExploded(false);
			}
		}	break;
		case ObjID::POTION:
		{
			Potion* tempPotion = (Potion*)onlineItems[i];
			if (tempPotion->timerGoing()) currentPlayer->setSpeed(50.f);
		}	break;
		}
	}

	//sending data to server
	if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - serverStart)).count() > serverTimerLength && client->getIfConnected())
	{
		SendingDataEvent(client, currentPlayer, players);
		serverStart = std::chrono::system_clock::now();
	}

	//Physics related functions
	for (int i = 0; i < players.size(); i++)
	{
		players[i]->updateMatrixOnline();
		players[i]->update();
	}

	//Updates gameObject physics components
	for (int i = 0; i < gameObjects.size(); i++) gameObjects[i]->update();

	//Setting the camera at position
	if (!velocityCamera) camera.moveVelocity(currentPlayer, dt);
	else camera.collisionMove(currentPlayer, planetVector, currentPlayer->getFlip(), dt);
	arrow->moveWithCamera(currentPlayer->getPosV3(), DirectX::XMVector3Normalize(camera.getForwardVector()), currentPlayer->getUpVector(), currentPlayer->getRotationMX());


	//Arrow pointing to capture zone or is removed
	if (captureZone)
	{
		if (captureZone->detectedObject(currentPlayer)) this->arrow->removeArrow();
		else this->arrow->showDirection(captureZone->getPosition(), grav);
	}
	currentPlayer->colliedWIthComponent(components);

	//Play pickup animation
	for (int i = 0; i < spaceShips.size(); i++) spaceShips[i]->animateOnPickup();

	//Check if item icon should change to pickup icon 
	for (int i = 0; i < items.size(); i++)
	{
		this->items[i]->checkDistance((GameObject*)(currentPlayer));
	}
	for (int i = 0; i < onlineItems.size(); i++)
	{
		this->onlineItems[i]->checkDistance((GameObject*)(currentPlayer));
	}
	for (int i = 0; i < components.size(); i++)
	{
		this->components[i]->checkDistance((GameObject*)(currentPlayer));
	}

	return currentGameState;
}

GAMESTATE Game::startIntermission()
{
	//Complete both ships
	for (int j = 0; j < spaceShips.size(); j++)
	{
		spaceShips[j]->completeShip();
	}

	//Cam
	this->camera.setPosition(DirectX::XMFLOAT3(100.f, 0.f, 300.f));
	this->camera.setRotToStart();
	this->camera.setCameraLookAt(DirectX::XMFLOAT3(150.f, 0, 300.f));

	//Light
	ltHandler.setPosition(DirectX::XMFLOAT3(100, 0, 290), 0);
	ltHandler.updateBuffers();

	//Ship
	this->spaceShips[0]->setRot(DirectX::XMFLOAT3(-DirectX::XM_PI * 0.5, 0, 0));
	this->spaceShips[1]->setRot(DirectX::XMFLOAT3(-DirectX::XM_PI * 0.5, 0, 0));

	if (teamScores[0] > teamScores[1]) this->offset = DirectX::XMFLOAT2(-15, 7);
	else this->offset = DirectX::XMFLOAT2(15, 7);

	this->centerPos = DirectX::XMFLOAT3(150, 0, 400);
	this->Stage = 0;
	ui.count = 1.0f;
	ui.setOpacity(false);
	fadedIn = false;

	miniGameUI.pointBlue = (float) teamScores[1];
	miniGameUI.pointRed = (float) teamScores[0];

	currentMinigame = INTERMISSION;
	return NOCHANGE;
}

GAMESTATE Game::updateIntermission()
{
	float camSpeed = 10 * dt;
	totalTime += dt;
	DirectX::XMFLOAT3 camPos;
	DirectX::XMStoreFloat3(&camPos, camera.getRealPosition());
	camPos.z -= camSpeed;

	if (!fadedIn)// fade in condition
	{
		if (!this->ui.fadeIn()) // is fading
		{
		}
		else // fade in complete
		{
			fadedIn = true;
		}
	}

	//DirectX::XMFLOAT3 midPos = camPos;
	//midPos.z += -20;
	this->camera.setPosition(camPos);
	camPos.x += 50;
	this->camera.setCameraLookAt(camPos);

	if (this->Stage == 0)
	{
		if (this->centerPos.z - camSpeed <= camPos.z)
		{
			this->Stage = 1;
			this->timer = 0;
		}
		else
		{
			camSpeed *= 2;
		}
	}
	else if (this->Stage == 1)
	{
		this->timer += dt;
		if (timer >= 5.0f)
		{
			this->Stage = 3;
		}
	}
	else if (Stage == 3)
	{
		
		camSpeed *= 2;
		if (this->centerPos.z + 60 <= camPos.z)
		{
			ui.fadeOut();

		}
		if (this->centerPos.z + 80 <= camPos.z)
		{
			this->spaceShips[0]->setRot(spaceShips[0]->getRotOrientedToGrav());
			this->spaceShips[0]->getPhysComp()->setRotation(reactphysics3d::Quaternion(spaceShips[0]->getRotXM().x, spaceShips[0]->getRotXM().y, spaceShips[0]->getRotXM().z, 1.0));
			this->spaceShips[1]->setRot(spaceShips[1]->getRotOrientedToGrav());
			this->spaceShips[1]->getPhysComp()->setRotation(reactphysics3d::Quaternion(spaceShips[1]->getRotXM().x, spaceShips[1]->getRotXM().y, spaceShips[1]->getRotXM().z, 1.0));


			//Send data to server
			DoneWithGame requestStart;
			requestStart.packetId = PacketType::DONEWITHGAME;
			requestStart.playerID = currentPlayer->getOnlineID();
			requestStart.formerGame = MiniGames::INTERMISSION;
			client->sendStuff<DoneWithGame>(requestStart);
			currentMinigame = MiniGames::DEFAULT;
			if (!IFONLINE)
			{
				this->startLanding();
			}
			return GAMESTATE::NOCHANGE;
		}
	}

	this->centerPos.z -= camSpeed;
	DirectX::XMFLOAT3 spacePos = centerPos;
	spacePos.z += offset.x;
	spacePos.y += offset.y;
	spacePos.y += sin(totalTime + offset.x);
	this->spaceShips[0]->setPos(spacePos);

	spacePos = centerPos;
	spacePos.z -= offset.x;
	spacePos.y -= offset.y;
	spacePos.y += sin(totalTime);
	this->spaceShips[1]->setPos(spacePos);
	return NOCHANGE;
}

GAMESTATE Game::Update()
{
	Input::Update();
	if (ui.isDone()) currentPlayer->isReady(true);

	//If someone for some reason want to add physics boxes to the world, SHALL BE REMOVED
	if (GetAsyncKeyState('C')) physWorld.addBoxToWorld();
	currentGameState = NOCHANGE;

	if (IFONLINE)
	{
		//read the packets received from the server
		packetEventManager->PacketHandleEvents(circularBuffer, NROFPLAYERS, players, client->getPlayerId(), components, physWorld, gameObjects, planetGravityField, spaceShips, onlineItems, meshes, planetVector, captureZone, currentMinigame,
			teamScoreLandingMiniGame, enemyTeamScoreLandingMiniGame, client, dt, currentGameState, gamePad, tmpMesh2, tmpMesh3, tmpMesh4, planetSRV_online, teamScores);
	}

	//Test player & hazard collision
	for (int i = 0; i < players.size(); i++)
	{
		for (int j = 0; j < gameObjects.size(); j++)
		{
			if (gameObjects[j]->getId() == ObjID::HAZARD)
			{
				HazardBox* hazard = (HazardBox*)gameObjects[j];
				hazard->CheckZap(players[i]);
			}
		}
	}

	lastUpdate = currentTime;
	this->currentPlayer->updateController();
	currentTime = std::chrono::system_clock::now();
	dt = ((std::chrono::duration<float>)(currentTime - lastUpdate)).count();

	//if (Input::KeyPress(KeyCode::P)) //THIS WILL BE REMOVED WHEN SWITCHING BETWEEN STATES IS DONE
	//{ 
	//	currentMinigame = MiniGames::LANDINGSPACESHIP;
	//	
	//	int index = currentPlayer->getTeam() + 1;
	//	int team = currentPlayer->getTeam();
		//
	//	for (int i = 0; i < spaceShips.size(); i++)
	//	{
	//		if (i == currentPlayer->getTeam()) { index = currentPlayer->getTeam() + 1; team = currentPlayer->getTeam(); }
	//		else { index = (int)!currentPlayer->getTeam() + 1; team = (int)!currentPlayer->getTeam(); }
	//		DirectX::SimpleMath::Vector3 planetOffset = planetVector[index]->getPlanetPosition();
	//		planetOffset.Normalize();
	//		planetOffset *= 150.f;
	//		DirectX::SimpleMath::Vector3 newPos = planetVector[index]->getPlanetPosition() + planetOffset;
	//		spaceShips[team]->setPos(newPos);
	//		spaceShips[team]->setGravityField(planetVector[index]->getGravityField());
	//		spaceShips[team]->setRot(spaceShips[team]->getRotOrientedToGrav());
	//	}
		//
	//	landingMiniGamePoints = 0.f;
	//}

	//if (Input::KeyPress(KeyCode::L))
	//{
	//	currentMinigame = MiniGames::INTERMISSION;
	//	this->camera.setPosition(DirectX::XMFLOAT3(100.f, 0.f, 300.f));
	//	this->camera.setRotToStart();
	//	this->camera.setCameraLookAt(DirectX::XMFLOAT3(150.f, 0, 300.f));

	//	this->spaceShips[0]->setRot(DirectX::XMFLOAT3(-DirectX::XM_PI * 0.5, 0, 0));
	//	this->spaceShips[1]->setRot(DirectX::XMFLOAT3(-DirectX::XM_PI * 0.5, 0, 0));

	//	this->centerPos = DirectX::XMFLOAT3(150, 0, 400);
	//	this->offset = DirectX::XMFLOAT2(15, 7);
	//	//this->spaceShips[0]->setPos(DirectX::XMFLOAT3(150, 7, 95));
	//	//this->spaceShips[1]->setPos(DirectX::XMFLOAT3(150, -7, 290));
	//	this->Stage = 0;
	//}

	//if (Input::KeyPress(KeyCode::MOUSE_R))
	//{
	//	currentMinigame = STARTOFINTERMISSION;
	//}
	//
	//if (Input::KeyPress(KeyCode::MOUSE_M))
	//{
	//	currentMinigame = STARTLANDING;
	//}
	//
	//if (Input::KeyPress(KeyCode::Y))
	//{
	//	currentMinigame = KINGOFTHEHILL;
	//}
	//
	//if (Input::KeyPress(KeyCode::P))
	//{
	//	return WIN;
	//}
	//if (Input::KeyPress(KeyCode::O))
	//{
	//	return LOSE;
	//}

	//Simulate the current minigame on client side
	switch (currentMinigame)
	{
	case COMPONENTCOLLECTION:
		currentGameState = this->updateComponentGame();
		break;
	case LANDINGSPACESHIP:
		currentGameState = this->updateLandingGame();
		break;
	case KINGOFTHEHILL:
		currentGameState = this->updateKingOfTheHillGame();
		break;
	case INTERMISSION:
		currentGameState = this->updateIntermission();
		break;
	case STARTOFINTERMISSION:
		currentGameState = this->startIntermission();
		break;
	case STARTLANDING:
		currentGameState = this->startLanding();
		break;
	default:
		break;
	}

	//animations
	//this->currentPlayer->updateAnim(dt, 0, 1);

	/*if (Input::KeyPress(KeyCode::L))
	{
		return WIN;
	}
	if (Input::KeyPress(KeyCode::P))
	{
		return LOSE;
	}*/

	return currentGameState;
}

//-------------------------------------------------------------------- RENDER PASS --------------------------------------------------------------------//


void Game::Render()
{
	
	if (!fadedIn)// fade in condition
	{
		if (!this->ui.fadeIn()) {}	// is fading
		else {fadedIn = true;}		// fade in complete
	}
	
	//Render shadow maps
	basicRenderer.lightPrePass();
	drawShadows();
	
	//Render Scene
	basicRenderer.setUpScene(this->camera);
	if (objectDraw) drawObjects(drawDebug);
	

	basicRenderer.setUpSceneNormalMap(this->camera);
	ltHandler.bindLightBuffers();
	//testCube->drawObjectWithNormalMap();

	if (IFONLINE)
	{
		planetVector[0]->setMesh(tmpMesh4);
		planetVector[0]->setSrv(planetSRV_online[0]);
		planetVector[0]->setNormalMap(planetSRV_online[1]);

		planetVector[1]->setMesh(tmpMesh3);
		planetVector[1]->setSrv(planetSRV_online[2]);
		planetVector[1]->setNormalMap(planetSRV_online[3]);

		planetVector[2]->setMesh(tmpMesh2);
		planetVector[2]->setSrv(planetSRV_online[4]);
		planetVector[2]->setNormalMap(planetSRV_online[5]);
	}

	//Unbind light
	ltHandler.unbindSrv();

	//Render fresnel objects
	basicRenderer.fresnelPrePass(this->camera);
	this->drawFresnel();

	//Animated meshes
	basicRenderer.fresnelAnimPrePass(this->camera);
	for (int i = 0; i < players.size(); i++)
	{
		players[i]->drawFresnel(0.4f);
	}

	//Render Skybox
	basicRenderer.skyboxPrePass();
	skybox.draw();
	basicRenderer.depthUnbind();

	//Render imgui & wireframe
	//imGui.react3D(wireframe, objectDraw, landingMinigame, dt, velocityCamera, currentPlayer);
	if (wireframe) { physWorld.renderReact3D(); playerVecRenderer.drawLines(); }
	
	//render billboard objects
	basicRenderer.bilboardPrePass(this->camera);
	drawIcons();

	//Render Particles
	basicRenderer.geometryPass(this->camera);
	drawParticles();
	basicRenderer.geometryUnbind();
	
	basicRenderer.postProcessPass();

	//Render UI (needs to render last)
	switch (currentMinigame)
	{

	case COMPONENTCOLLECTION:
		if (!ui.readySetGo()){}
		ui.Draw();
		break;

	case LANDINGSPACESHIP:
		landingUi.draw();
		ui.DrawFade();
		break;

	case KINGOFTHEHILL:
		if (!ui.readySetGo()) { currentPlayer->isReady(false);  }
		else { currentPlayer->isReady(true); }
		ui.setKTH(true);
		ui.DrawFade(this->teamScoreLandingMiniGame, this->enemyTeamScoreLandingMiniGame);
		break;

	case INTERMISSION:
		miniGameUI.Draw();
		landingUi.drawPointsForOtherGameModes(teamScores);
		ui.DrawFade();
		break;

	default:
		break;
	}
	
}