#include "PhysicsWorld.h"
#include "GameObject.h"
#include "ServerPlanet.h"

#include <iostream>
#include <string>
#include <mutex>

#include <SFML/Network.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include "Player.h"
#include "PacketsDataTypes.h"
#include "Packethandler.h"
#include "CircularBuffer.h"
#include "Component.h"
#include "SpawnComponent.h"
#include "RandomizeSpawn.h"
#include "DirectXMathHelper.h"
#include "TimeStruct.h"
#include "ObjectId.h"
#include "BaseBallBat.h"
#include "Grenade.h"
#include "GetPlayerData.h"

#include "TestObj.h"

#include "KingOfTheHillMiniGame.h"
#include "Time.h"
#include <d3d11_4.h>
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <psapi.h>

std::mutex mutex;
bool GAMEONGOING = true;

struct threadInfo
{
	userData* user;
	float pos[3];
	bool ifAccepted;//used for lobby System
	CircularBuffer* circBuffer;
};

void setupTcp(serverData& data)
{
	unsigned short listenport = 2001;
	data.tcpListener.listen(listenport);
	//std::cout << "TCP listening on port: " << std::to_string((int)listenport) << std::endl;
};

void acceptPlayers(serverData& data)
{
	std::cout << "Accepting players TCP socket\n";

	for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
	{
		if (data.tcpListener.accept(data.users[i].tcpSocket) == sf::TcpListener::Done)
		{
			//std::cout << "TCP Accepted new player ipAdress: " << data.users[i].tcpSocket.getRemoteAddress().toString() << std::endl;
			data.users[i].ipAdress = data.users[i].tcpSocket.getRemoteAddress().toString();
			data.users[i].userName = "fixa username " + std::to_string(i + 1);
			data.users[i].playerId = i;
		}

	}
};

void recvData(void* param, userData* user)//thread to recv data
{
	threadInfo* data = (threadInfo*)param;
	int testStore = -2;

	//std::cout << "ip from socket in thread: " << user->tcpSocket.getRemoteAddress().toString() << std::endl;
	while (GAMEONGOING)
	{
		std::size_t recv;
		char datapointer[256];

		if (user->tcpSocket.receive(&datapointer, 256, recv) != sf::Socket::Done)
		{
			//error
		}
		else
		{
			//USE TO CHECK IF THE PACKET ID IS CORRECT (MAYBE=
			memcpy(&testStore, datapointer, sizeof(int));

			//if(user->playerId == 0)std::cout << "testStore: " << testStore << std::endl;

			if (testStore > 0 && testStore < 80)
			{
				mutex.lock();
				data->circBuffer->addData(datapointer, recv);
				mutex.unlock();
			}

		}

	}
	std::cout << "ENDED RECVDATA THREAD\n";
};

void acceptPlayersLobbyThread(void* param)//thread for lobby system to receive players
{
	serverData* data = (serverData*)param;
	std::cout << "Started lobby accept thread\n";
	int i = 0;
	while (1)
	{
		if (i == MAXNUMBEROFPLAYERS)break;
		if (data->tcpListener.accept(data->users[i].tcpSocket) == sf::TcpListener::Done)
		{
			//std::cout << "TCP Accepted new player ipAdress: " << data->users[i].tcpSocket.getRemoteAddress().toString() << ", Player id: " << i << std::endl;
			data->users[i].ipAdress = data->users[i].tcpSocket.getRemoteAddress().toString();
			data->users[i].userName = "fixa username " + std::to_string(i + 1);
			data->users[i].playerId = i;
			data->users[i].playa.setOnlineId(i);
			data->users[i].playa.setReadyStatus(-1);

			idProtocol pId;
			pId.packetId = PacketType::PACKETID;
			pId.assignedPlayerId = i;

			data->users[i].ifAccepted = true;
			
			sendBinaryDataOnePlayer<idProtocol>(pId, data->users[i]);
			i++;
		}
	}
	std::cout << "Ended lobby accept thread\n";
};

void sendIdToAllPlayers(serverData& data)
{
	int packetid = 10;

	for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
	{
		idProtocol protocol;
		protocol.packetId = PacketType::PACKETID;
		protocol.assignedPlayerId = data.users[i].playerId;


		std::size_t sentSize;

		if (data.users[i].tcpSocket.send(&protocol, sizeof(protocol), sentSize) != sf::Socket::Done)
		{
			//error
			//std::cout << "sendIdToAllPlayers(), couldnt send id to currentPlayer: " << std::to_string(i) << std::endl;
		}
		else
		{
			//std::cout << "sendIdToAllPlayers() sent to currentPlayer nr: " << std::to_string(data.users[i].playerId) << std::endl;
		}
	}
};

void lobby(serverData& data, CircularBuffer & circBuffer, std::thread* recvThread[],PhysicsWorld &physWorld, threadInfo threadData[])
{
	TimeStruct tempTime;
	bool ifThreaded[MAXNUMBEROFPLAYERS]{ false };
	bool loop = true;
	while (loop)
	{

		//kollar packet så att det tas emot och slänger packet vi inte vill ha
		while (circBuffer.getIfPacketsLeftToRead())
		{
			int packetId = circBuffer.peekPacketId();

			if (packetId == PacketType::PLAYERDATALOBBY)
			{
				PlayerData* plData = circBuffer.readData<PlayerData>();
				//std::cout << "Got Ready Status from player Id: " << plData->playerId << ", readyStatus: " << plData->playerReady << std::endl;
				data.users[plData->playerId].playa.setReadyStatus(plData->playerReady);
				
			}
			else
			{
				std::cout << "Clearing circle Buffer\n";
				circBuffer.clearBuffer();
			}
		}

		
		for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		{
			//om spelaren inte har en tråd så skapa en tråd så att vi kan ta emot data
			//detta händer efter den blivit accepted
			if (!ifThreaded[i] && data.users[i].ifAccepted)
			{
				int offset = 25; PhysicsComponent* newComp = new PhysicsComponent();
				physWorld.addPhysComponent(newComp);
				data.users[i].playa.setPhysicsComponent(newComp);
				newComp->setType(reactphysics3d::BodyType::KINEMATIC);
				threadData[i].pos[0] = 102.0f + (offset * i);
				threadData[i].pos[1] = 12.0f;
				threadData[i].pos[2] = -22.0f;
				threadData[i].circBuffer = &circBuffer;
				std::thread(recvData, &threadData[i], &data.users[i]).detach();

				std::cout << "startade thread : " << i << std::endl;

				ifThreaded[i] = true;
			}
		}
		
		bool allPlayersReadyForGame = true;

		if (tempTime.getTimePassed(1.0f))
		{
			//******************************************************************SKICKAR AV NÅGON KONSTIG ANLEDNING
			//checking if all the players are ready for the game
			for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
			{
				//std::cout << "readystatus: " << data.users[i].playa.getReadyStatus() << std::endl;
				if (ifThreaded[i] && data.users[i].playa.getReadyStatus() == 0)
				{
					allPlayersReadyForGame = true;
					
				}
				else
				{
					allPlayersReadyForGame = false;
					break;
				}
			}
			if (allPlayersReadyForGame)
			{
				LobbyStartGame lbyStartGame;
				lbyStartGame.packetId = PacketType::LOBBYSTARTGAME;

				std::cout << "Sending Ready to start game\n";
				sendBinaryDataAllPlayers<LobbyStartGame>(lbyStartGame, data);
				loop = false;
			}

			for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
			{
				//if it isnt -1, the player has been accepted
				if (data.users[i].playerId != -1)
				{
					//send data to all the players that this player is ready
					for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)
					{
						//only send if the recipient is initialized

						if (data.users[j].playerId != -1)
						{
							//sending data about the player to other players
							PlayerData playerData;

							playerData.packetId = PacketType::PLAYERDATALOBBY;
							playerData.playerId = data.users[i].playa.getOnlineID();
							playerData.playerReady = data.users[i].playa.getReadyStatus();
							playerData.playerIsConnected = ifThreaded[i];

							//std::cout << " Player Data id:" << playerData.playerId << ", isReady: " << playerData.playerReady << std::endl;
							sendBinaryDataOnePlayer<PlayerData>(playerData, data.users[j]);
						}
					}
				}
			}
			tempTime.resetStartTime();
		}
	}
};

int main()
{

	CircularBuffer* circBuffer = new CircularBuffer();

	srand((unsigned)(time(0)));
	Time::Start();
	Time::Reset();

	while (1)
	{
		//std::cout << "UwU begin 1\n";
		GAMEONGOING = true;
		PhysicsWorld physWorld;
		std::thread* recvThread[MAXNUMBEROFPLAYERS];
		threadInfo threadData[MAXNUMBEROFPLAYERS];
		
		Component planetComp;

		std::vector<int> playersSent;

		TimeStruct physicsTimer;

		std::vector<Planet*> planetVector;
		std::vector<Item*> onlineItems;
		std::vector<DirectX::XMFLOAT3> spaceShipPos;

		int requests = 0;
		int itemid = 0;
		int componentIdCounter = 0;
		bool once = false;
		int itemLimit = 10;
		int grenadeCount = 0;
		float flyTime = 0.f;
		float landingPoints[MAXNUMBEROFPLAYERS]{ 0.f };
		float totalTeamScores[2]{ 0.f };
		int shipComponentCounter[2]{ 0 };
		bool timeToFly = false;
		int progress[2] = { 0, 0 };

		//std::cout << "UwU begin 1.5\n";
		MiniGames currentMinigame = MiniGames::COMPONENTCOLLECTION;
		physWorld.addPhysComponent(planetComp, reactphysics3d::CollisionShapeName::SPHERE, DirectX::XMFLOAT3(40, 40, 40));
		planetComp.getPhysicsComponent()->setType(reactphysics3d::BodyType::STATIC);

		//std::cout << "UwU begin 2\n";
	

		std::string identifier;
		std::string s = "empty";
		// Group the variables to send into a packet

		for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		{
			landingPoints[i] = 0.f;

		}

		//std::cout << "UwU begin 3\n";


		spaceShipPos.emplace_back(DirectX::XMFLOAT3(-7.81178f, -37.8586f, -8.50119f));
		spaceShipPos.emplace_back(DirectX::XMFLOAT3(13.5817f, 35.9383f, 9.91351f));

		//std::cout << "Nr of players for the game: " << std::to_string(MAXNUMBEROFPLAYERS) << std::endl;
		KingOfTheHillMiniGame miniGameKTH(MAXNUMBEROFPLAYERS);

		//std::vector<player> players;
		//std::vector<Component> components;
		//std::vector<Component> items;

		//ANVÄND DENNA FLR ALLA ITEMS OCH COMPONENTS

		

		//sf::UdpSocket socket;
		std::string connectionType, mode;

		serverData data;
		
		sf::IpAddress remoteAddress;

		sf::IpAddress ip = sf::IpAddress::getLocalAddress();

		data.port = 2001;
		
		//std::cout << ip.toString() << "\n" << "port: " << data.port << std::endl;
		std::cout << "Ip adress: " << ip.toString() << "\n";

		for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		{
			data.users[i].playa.setOnlineId(-1);
		}

		//serverData* serverData;
		//std::cout << "Starting handleReceiveData thread!\n";
		//std::thread* serverThread = new std::thread(handleReceivedData, &serverData);

		//std::cout << "UWU 5\n";

		if (data.socket.bind(data.port) != sf::Socket::Done)
		{
			std::cout << "UDP Failed to bind UDP servern\n";
			return -1;
		}
		else
		{
			//std::cout << "UDP Successfully bound socket\n";
		}

		std::chrono::time_point<std::chrono::system_clock> start, startComponentTimer, itemSpawnTimer, startFly;
		start = std::chrono::system_clock::now();

		float timerLength = 1.f / 60.0f;
		float timerComponentLength = 10.0f;
		float itemSpawnTimerLength = 5.0f;

		setupTcp(data);
		//acceptPlayers(data);

		//std::cout << "UWU 6\n";
		circBuffer->clearBuffer();
		std::thread(acceptPlayersLobbyThread, &data).detach();
		lobby(data, *circBuffer, recvThread, physWorld, threadData);
		circBuffer->clearBuffer();

		//std::cout << "UWU 7\n";

		physicsTimer.resetStartTime();
		while (!physicsTimer.getTimePassed(2.0f)) continue;
		//sendIdToAllPlayers(data);

		//Wait 3 seconds since we can lose some data if we directly send information about space ships
		physicsTimer.resetStartTime();
		while (!physicsTimer.getTimePassed(7.0f)) continue;

		//Sends information about the space ships to the clients
		for (int i = 0; i < spaceShipPos.size(); i++)
		{
			SpaceShipPosition spaceShipData;
			spaceShipData.packetId = PacketType::SPACESHIPPOSITION;
			spaceShipData.spaceShipTeam = i;
			spaceShipData.x = spaceShipPos[i].x;
			spaceShipData.y = spaceShipPos[i].y;
			spaceShipData.z = spaceShipPos[i].z;
			sendBinaryDataAllPlayers<SpaceShipPosition>(spaceShipData, data);
			//std::cout << "Sent a spaceship\n";
		}

		//Waiting a bit
		physicsTimer.resetStartTime();
		while (!physicsTimer.getTimePassed(7.0f)) continue;

		//Spawning planets
		float planetSize = 40.f;
		planetVector.emplace_back(new Planet(DirectX::XMFLOAT3(planetSize, planetSize, planetSize), DirectX::XMFLOAT3(0.f, 0.f, 0.f)));
		planetVector.back()->setPlanetShape(&physWorld);
		planetVector.emplace_back(new Planet(DirectX::XMFLOAT3(planetSize * 0.8f, planetSize * 0.8f, planetSize * 0.8f), DirectX::XMFLOAT3(60.f, 60.f, 60.f)));
		planetVector.back()->setPlanetShape(&physWorld);
		planetVector.emplace_back(new Planet(DirectX::XMFLOAT3(planetSize * 0.6f, planetSize * 0.6f, planetSize * 0.6f), DirectX::XMFLOAT3(55.f, -55.f, -55.f)));
		planetVector.back()->setPlanetShape(&physWorld);
		physWorld.setPlanets(planetVector);

		for (int i = 0; i < planetVector.size(); i++)
		{
			SpawnPlanets planetData;
			planetData.packetId = PacketType::SPAWNPLANETS;
			planetData.xPos = planetVector[i]->getPlanetPosition().x;
			planetData.yPos = planetVector[i]->getPlanetPosition().y;
			planetData.zPos = planetVector[i]->getPlanetPosition().z;
			planetData.size = planetVector[i]->getSize();
			sendBinaryDataAllPlayers<SpawnPlanets>(planetData, data);
			//std::cout << "Sent a planet\n";
		}



		//int offset = 25;
		//for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		//{
		//	PhysicsComponent* newComp = new PhysicsComponent();
		//	physWorld.addPhysComponent(newComp);
		//	data.users[i].playa.setPhysicsComponent(newComp);
		//	newComp->setType(reactphysics3d::BodyType::KINEMATIC);
		//	threadData[i].pos[0] = 102.0f + (offset * i);
		//	threadData[i].pos[1] = 12.0f;
		//	threadData[i].pos[2] = -22.0f;
		//	threadData[i].circBuffer = circBuffer;
		//	recvThread[i] = new std::thread(recvData, &threadData[i], &data.users[i]);
		//}

		int temp = 0;
		while (1)
		{
			//std::cout << "damn: " << circBuffer->peekPacketId() << std::endl;
			if (circBuffer->getIfPacketsLeftToRead())
			{
				int packetId = circBuffer->peekPacketId();

				if (packetId == PacketType::DONELOADING)
				{
					DoneLoading* los = circBuffer->readData<DoneLoading>();
					std::cout << "DONE LOADING\n";
					temp++;
					if (temp == MAXNUMBEROFPLAYERS) break;
				}
				else if (packetId == PacketType::POSITION)
				{
					circBuffer->readData<testPosition>();
					std::cout << "position while Loading\n";
				}
				else if (packetId == PacketType::POSITIONROTATION)
				{
					circBuffer->readData<PositionRotation>();
					std::cout << "prRotation while Loading\n";
				}
				else
				{
					std::cout << "Clearing buffer in loading loop\n";
					circBuffer->clearBuffer();
					break;
				}
			}
		}


		int sizeOfPackets = 0;
		TimeStruct DebugSizePackets;

		std::cout << "Starting while loop! \n";

		//Starting timer
		start = std::chrono::system_clock::now();
		startComponentTimer = std::chrono::system_clock::now();
		itemSpawnTimer = std::chrono::system_clock::now();


		//clearing data before the game loop
		circBuffer->clearBuffer();
		while (true)
		{

			while (circBuffer->getIfPacketsLeftToRead())
			{
				int packetId = circBuffer->peekPacketId();

				testPosition* tst = nullptr;
				ComponentData* compData = nullptr;
				PositionRotation* prMatrixData = nullptr;
				PlayerHit* playerHit = nullptr;
				itemPosition* itemPos = nullptr;
				ComponentDropped* cmpDropped = nullptr;
				ComponentRequestingPickUp* requestingCmpPickedUp = nullptr;
				LandingMiniSendScoreToServer* scoreFromClient = nullptr;
				MinigameStart* startMinigame = nullptr;
				DoneWithGame* requestStart = nullptr;
				baseballBatSpawn* baseBallBatData = nullptr;
				UseGrenade* grenadeData = nullptr;
				UseBat* useBat = nullptr;
				PlayerPush* playerPush = nullptr;
				ItemThrow* testThrow = nullptr;

				switch (packetId)
				{
				default:
					circBuffer->clearBuffer();
					//std::cout << "BAD PACKET RECEIVED, CLEARING THE CIRCLEBUFFER\n";
					break;

				case PacketType::POSITIONROTATION:
					prMatrixData = circBuffer->readData<PositionRotation>();
					//std::cout << "packet id: " << std::to_string(prMatrixData->playerId) << std::endl;
					for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
					{
						//std::cout << "playerid prMatrixData" << std::to_string(prMatrixData->) << std::endl;
						if (i == prMatrixData->playerId)
						{
							data.users[i].playa.setAnimData(prMatrixData->AnimId, prMatrixData->animSpeed);
							if (!data.users[i].playa.getDeathState())
							{
								data.users[i].playa.setMatrix(prMatrixData->matrix);
								data.users[i].playa.getPhysComp()->setRotation(reactphysics3d::Quaternion(prMatrixData->xRot, prMatrixData->yRot, prMatrixData->zRot, prMatrixData->wRot));
							}
							//std::cout <<"player Id: " << std::to_string(prMatrixData->playerId)<<"pos: " << std::to_string(data.users[i].playa.getMatrix()._14) << std::endl;
							break;
						}
					}
					//sendBinaryDataAllPlayers<PositionRotation>(*prMatrixData, data);
					break;

				case PacketType::POSITION:
					tst = circBuffer->readData<testPosition>();
					for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
					{
						if (i == tst->playerId)
						{
							if (!data.users[i].playa.getDeathState()) data.users[i].playa.setPosition(tst->x, tst->y, tst->z); else
							{
								reactphysics3d::Vector3 tempPos = data.users[i].playa.getPhysComp()->getPosition();
								data.users[i].playa.setPosition(tempPos.x, tempPos.y, tempPos.z);
							}
							break;
						}
					}
					break;

				case PacketType::COMPONENTPOSITION:
					compData = circBuffer->readData<ComponentData>();

					for (int i = 0; i < onlineItems.size(); i++)
					{
						if (onlineItems[i]->getOnlineId() == compData->ComponentId)
						{
							//kanske inte GÖRA NÅGOT EFTERSOM MAN KAN TA POSITION FRÅN DEN SOM ÄR IN USE NÄR MAN FÅR SPELAR POSITION
						}
					}
					//use later

					break;

				case PacketType::COMPONENTDROPPED:
					cmpDropped = circBuffer->readData<ComponentDropped>();
					//std::cout << "dropped package recv compId: " << std::to_string(cmpDropped->componentId) << std::endl;
					//find the component that is dropped
					for (int i = 0; i < onlineItems.size(); i++)
					{
						if (onlineItems[i]->getOnlineId() == cmpDropped->componentId)
						{
							DirectX::XMFLOAT3 newRandomPos = randomizeObjectPos(planetVector);
							onlineItems[i]->setInUseBy(-1);
							//std::cout << std::to_string(onlineItems[i]->getPosXMFLOAT3().x) << ", y: " << std::to_string(onlineItems[i]->getPosXMFLOAT3().y) <<
								//", z" << std::to_string(onlineItems[i]->getPosXMFLOAT3().z) << std::endl;
							if (cmpDropped->randomizePos == 0) onlineItems[i]->setPosition(cmpDropped->xPos, cmpDropped->yPos, cmpDropped->zPos);
							else if (cmpDropped->randomizePos == 1) onlineItems[i]->setPosition(newRandomPos.x, newRandomPos.y, newRandomPos.z);
							onlineItems[i]->getPhysicsComponent()->setType(reactphysics3d::BodyType::DYNAMIC);
							for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)
							{
								//skicka att en spelare har droppat en component till alla spelare förutom spelaren som droppat componenten eftersom den redan är droppad

								ComponentDropped cmpDropData;
								cmpDropData.componentId = cmpDropped->componentId;
								cmpDropData.packetId = cmpDropped->packetId;
								cmpDropData.playerId = cmpDropped->playerId;
								sendBinaryDataOnePlayer<ComponentDropped>(cmpDropData, data.users[j]);

							}
						}
					}
					break;

				case PacketType::COMPONENTREQUESTINGPICKUP:
					requestingCmpPickedUp = circBuffer->readData<ComponentRequestingPickUp>();
					//std::cout << "cmprequestpickup comp id: " << std::to_string(requestingCmpPickedUp->componentId) << std::endl;
					//std::cout << "onlineItems size: " << std::to_string(onlineItems.size()) << std::endl;
					for (int i = 0; i < onlineItems.size(); i++)
					{
						//kollar efter rätt component
						if (onlineItems[i]->getOnlineId() == requestingCmpPickedUp->componentId)
						{
							//kollar om componenten inte används
							if (onlineItems[i]->getInUseById() == -1)
							{
								//std::cout << "picked up componentId: " << std::to_string(requestingCmpPickedUp->componentId)
									//<< ", by player: " << std::to_string(requestingCmpPickedUp->playerId) << std::endl;
								//skickar en bekräftelse till alla spelare att komponenten är upplockad av en spelare
								ConfirmComponentPickedUp sendConfirmComponentData;
								sendConfirmComponentData.playerPickUpId = requestingCmpPickedUp->playerId;
								sendConfirmComponentData.componentId = requestingCmpPickedUp->componentId;
								sendConfirmComponentData.packetId = PacketType::COMPONENTCONFIRMEDPICKUP;
								sendBinaryDataAllPlayers<ConfirmComponentPickedUp>(sendConfirmComponentData, data);
								onlineItems[i]->setInUseBy(requestingCmpPickedUp->playerId);
								onlineItems[i]->getPhysicsComponent()->setType(reactphysics3d::BodyType::STATIC);
							}
							else
							{
								std::cout << "A Player couldnt pick up component: " << std::to_string(requestingCmpPickedUp->componentId) << std::endl;
							}
						}
					}

					break;
				case PacketType::PLAYERHIT:
					playerHit = circBuffer->readData<PlayerHit>();
					sendBinaryDataOnePlayer<PlayerHit>(*playerHit, data.users[playerHit->playerId]);
					data.users[playerHit->playerId].playa.playerGotHit(reactphysics3d::Vector3(playerHit->xForce, playerHit->yForce, playerHit->zForce));
					break;

				case PacketType::ITEMPOSITION:
					itemPos = circBuffer->readData<itemPosition>();
					for (int i = 0; i < onlineItems.size(); i++)
					{
						if (onlineItems[i]->getOnlineId() == itemPos->itemId) //finding the correct item
						{
							//set the data
							break;
						}
					}
					break;

				case PacketType::BASEBALLBATSPAWN://ändras sonlineItemså att servern skickar och client tar emot
					baseBallBatData = circBuffer->readData<baseballBatSpawn>();

					break;

				case PacketType::USEGRENADE:
					grenadeData = circBuffer->readData<UseGrenade>();
					onlineItems[grenadeData->itemId]->use(nullptr);
					onlineItems[grenadeData->itemId]->getPhysicsComponent()->setType(reactphysics3d::BodyType::DYNAMIC);
					onlineItems[grenadeData->itemId]->getPhysicsComponent()->setPosition(reactphysics3d::Vector3(
						onlineItems[grenadeData->itemId]->getPhysicsComponent()->getPosV3().x + grenadeData->xForce * 0.0025f,
						onlineItems[grenadeData->itemId]->getPhysicsComponent()->getPosV3().y + grenadeData->yForce * 0.0025f,
						onlineItems[grenadeData->itemId]->getPhysicsComponent()->getPosV3().z + grenadeData->zForce * 0.0025f));
					onlineItems[grenadeData->itemId]->getPhysicsComponent()->applyForceToCenter(reactphysics3d::Vector3(grenadeData->xForce *2, grenadeData->yForce * 2, grenadeData->zForce * 2));
					sendBinaryDataAllPlayers<UseGrenade>(*grenadeData, data);
					//std::cout << "Player used grenade OwO\n";
					break;

				case PacketType::LANDINGMINIGAMESENDSCORETOSERVER:
					scoreFromClient = circBuffer->readData<LandingMiniSendScoreToServer>();
					landingPoints[scoreFromClient->playerId] = scoreFromClient->scoreToServer;
					break;

				case PacketType::USEBAT:
					useBat = circBuffer->readData<UseBat>();



					//DirectX::SimpleMath::Vector3 posTwo;

					/*for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
					{
						if(data.users[i].)
					}*/

					for (int i = 0; i < onlineItems.size(); i++)
					{

						if (onlineItems[i]->getOnlineId() == useBat->itemId)
						{
							DirectX::SimpleMath::Vector3 pos;
							for (int j = 0; j < onlineItems.size(); j++)
							{
								if (onlineItems[i] == onlineItems[j]) continue;
								pos = (DirectX::SimpleMath::Vector3)onlineItems[j]->getPosXMFLOAT3() - onlineItems[i]->getPosXMFLOAT3();
								if (getLength(pos) <= 20.f)
								{
									//std::cout << "USER ID: " << useBat->playerThatUsedTheItem << "\n";
									pos.x = -data.users[useBat->playerThatUsedTheItem].playa.getposition('x');
									pos.y = -data.users[useBat->playerThatUsedTheItem].playa.getposition('y');
									pos.z = -data.users[useBat->playerThatUsedTheItem].playa.getposition('z');
									pos += onlineItems[j]->getPosXMFLOAT3();
									pos.Normalize();
									onlineItems[j]->getPhysicsComponent()->applyForceToCenter(
										reactphysics3d::Vector3(pos.x * 10000, pos.y * 10000, pos.z * 10000));

								}
							}
							onlineItems[useBat->itemId]->setInUseBy(-1);
							DirectX::XMFLOAT3 newPos = randomizeObjectPos(planetVector);
							onlineItems[useBat->itemId]->setPosition(newPos.x, newPos.y, newPos.z);
						}
					}
					//for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)
					//{
					//	pos.x = data.users[j].playa.getposition('x');
					//	pos.y = data.users[j].playa.getposition('y');
					//	pos.z = data.users[j].playa.getposition('z');


					//	objPos = onlineItems[i]->getPosXMFLOAT3();
					//	subtractionXMFLOAT3(pos, objPos);
					//	if (getLength(pos) <= 25.f)
					//	{
					//		pos.Normalize();
					//		//data.users[j].playa.getPhysComp()->applyForceToCenter();
					//		data.users[j].playa.playerGotHit(reactphysics3d::Vector3(
					//			10000 * pos.x, 10000 * pos.y, 10000 * pos.z));
					//		sendBinaryDataOnePlayer<HitByGrenade>(hitByGrenade, data.users[j]);
					//	}
					//}


					break;

				case PacketType::PLAYERPUSHED:
					playerPush = circBuffer->readData<PlayerPush>();
					sendBinaryDataOnePlayer<PlayerPush>(*playerPush, data.users[playerPush->affectedPlayerId]);
					break;

				case PacketType::ITEMTHROW:
					testThrow = circBuffer->readData<ItemThrow>();
					//std::cout << "test throw server" << std::endl;
					//std::cout << "Force X: " << testThrow->xForce << " Y: " << testThrow->yForce << " Z: " << testThrow->zForce << std::endl;

					//PacketType::ITEMTHROW;
					//PacketType::COMPONENTDROPPED;
					//find the component that is dropped
					for (int i = 0; i < onlineItems.size(); i++)
					{
						if (onlineItems[i]->getOnlineId() == testThrow->itemId)
						{
							//set not used
							onlineItems[i]->setInUseBy(-1);

							//print pos
							//std::cout << std::to_string(onlineItems[i]->getPosXMFLOAT3().x) << ", y: " << std::to_string(onlineItems[i]->getPosXMFLOAT3().y) <<
								//", z" << std::to_string(onlineItems[i]->getPosXMFLOAT3().z) << std::endl;

							//set pos // nessecary?
							onlineItems[i]->setPosition(testThrow->xPos, testThrow->yPos, testThrow->zPos);

							//set hitbox type
							onlineItems[i]->getPhysicsComponent()->setType(reactphysics3d::BodyType::DYNAMIC);

							//Apply the force
							onlineItems[i]->getPhysicsComponent()->applyForceToCenter(reactphysics3d::Vector3(testThrow->xForce * 5, testThrow->yForce * 5, testThrow->zForce * 5));

							for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)
							{
								//skicka att en spelare har droppat en component till alla spelare förutom spelaren som droppat componenten eftersom den redan är droppad
								
								ComponentDropped cmpDropData;
								cmpDropData.componentId = testThrow->itemId;
								cmpDropData.packetId = PacketType::COMPONENTDROPPED;
								cmpDropData.playerId = testThrow->playerId;
								sendBinaryDataOnePlayer<ComponentDropped>(cmpDropData, data.users[j]);

							}
						}
					}


					break;

				case PacketType::DONEWITHGAME:
					requestStart = circBuffer->readData<DoneWithGame>();

					//Checking if the same player have already sent a request
					bool alreadySent = false;
					for (int i = 0; i < playersSent.size(); i++)
					{
						if (requestStart->playerID == playersSent[i]) alreadySent = true;
					}

					//It was not the same player
					if (!alreadySent)
					{
						playersSent.emplace_back(requestStart->playerID);
						requests++;

						//Starting new minigame if everyone is done
						if (requests == MAXNUMBEROFPLAYERS)
						{
							MinigameStart startGame;
							startGame.packetId = PacketType::STARTMINIGAMES;

							//Checks what the former game was
							if (requestStart->formerGame == MiniGames::INTERMISSION)
							{
								//Sending the capturezone
								miniGameKTH.sendKingOfTheHillZone(data);
								//std::cout << "Sent capture zone\n";

								for (int i = 0; i < onlineItems.size(); i++)
								{
									if (onlineItems[i]->getOnlineType() == ObjID::COMPONENT)
									{
										onlineItems[i]->setPosition(420.0f, 420.0f, 420.0f);
										onlineItems[i]->getPhysicsComponent()->setType(reactphysics3d::BodyType::STATIC);
									}
								}
								

								//Sending the planets
								planetVector[0]->setScale(DirectX::XMFLOAT3(60.f, 60.f, 60.f));
								planetVector[2]->setPosition(DirectX::XMFLOAT3(60.f, 60.f, 60.f));
								planetVector[2]->setScale(DirectX::XMFLOAT3(25.f, 25.f, 25.f));
								planetVector[1]->setPosition(DirectX::XMFLOAT3(-60.f, -60.f, -60.f));
								planetVector[1]->setScale(DirectX::XMFLOAT3(25.f, 25.f, 25.f));

								for (int i = 0; i < planetVector.size(); i++)
								{
									SpawnPlanets planetData;
									planetData.packetId = PacketType::SPAWNPLANETS;
									planetData.xPos = planetVector[i]->getPlanetPosition().x;
									planetData.yPos = planetVector[i]->getPlanetPosition().y;
									planetData.zPos = planetVector[i]->getPlanetPosition().z;
									planetData.size = planetVector[i]->getSize();
									sendBinaryDataAllPlayers<SpawnPlanets>(planetData, data);
									//std::cout << "Sent a planet\n";
								}

								//Sending next minigame
								currentMinigame = MiniGames::LANDINGSPACESHIP;
								startGame.minigame = MiniGames::STARTLANDING;

							}
							else if (requestStart->formerGame == MiniGames::LANDINGSPACESHIP)
							{
								int landPointToScore[2]{ 0 };
								if (landingPoints[0] > landingPoints[1])
								{
									landPointToScore[0] = 100;
									landPointToScore[1] = (int)((landingPoints[1] / landingPoints[0]) * 100.f);
								}
								else
								{
									landPointToScore[1] = 100;
									landPointToScore[0] = (int)((landingPoints[0] / landingPoints[1]) * 100.f);
								}
								totalTeamScores[0] += landPointToScore[0];
								totalTeamScores[1] += landPointToScore[1];

								//std::cout << "Red team score : " << (int)totalTeamScores[0] << "\nBlue team score: " << (int)totalTeamScores[1] << "\n";

								//Sending the next minigame
								currentMinigame = MiniGames::KINGOFTHEHILL;
								startGame.minigame = MiniGames::KINGOFTHEHILL;
							}
							startGame.pointsRed = totalTeamScores[0];
							startGame.pointsBlue = totalTeamScores[1];
							sendBinaryDataAllPlayers<MinigameStart>(startGame, data);

							//Resetting
							requests = 0;
							playersSent.clear();
						}
					}
					break;

				
				}
			}

			//checks all components player position
			//for (int i = 0; i < components.size(); i++)
			//{
			//	for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)
			//	{
			//		if (components[i].getInUseById() == data.users[j].playerId)
			//		{
			//			components[i].setPosition(data.users[j].playa.getposition('x'), data.users[j].playa.getposition('y'), data.users[j].playa.getposition('z'));
			//		}
			//	}
			//	//std::cout << "component in useBy: " << std::to_string(components[i].getInUseById()) << std::endl;
			//	
			//	//std::cout << "posX: " << std::to_string(components[i].getposition('x')) << "posY: " << std::to_string(components[i].getposition('y')) << std::endl;
			//}

			for (int i = 0; i < onlineItems.size(); i++)
			{
				for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)
				{
					if (onlineItems[i]->getInUseById() == data.users[j].playerId)
					{
						onlineItems[i]->setPosition(data.users[j].playa.getposition('x'), data.users[j].playa.getposition('y'), data.users[j].playa.getposition('z'));
					}
				}
			}

			if (currentMinigame == MiniGames::COMPONENTCOLLECTION)
			{
				//Spawns a component
				if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - startComponentTimer)).count() > timerComponentLength && !once)
				{
					SpawnComponent cData = SpawnOneComponent(onlineItems, spaceShipPos);
					physWorld.addPhysComponent(*onlineItems[onlineItems.size() - 1]);
					onlineItems[onlineItems.size() - 1]->setPosition(cData.x, cData.y, cData.z);
					onlineItems[onlineItems.size() - 1]->setOnlineId(componentIdCounter++);
					onlineItems[onlineItems.size() - 1]->setOnlineType(ObjID::COMPONENT);
					sendBinaryDataAllPlayers<SpawnComponent>(cData, data);
					startComponentTimer = std::chrono::system_clock::now();
					once = true;
				}
			}


			//Spawns a baseBallBat
			if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - itemSpawnTimer)).count() > itemSpawnTimerLength && onlineItems.size() <= 10)
			{
				ItemSpawn itemSpawnData;
				DirectX::XMFLOAT3 temp = randomizeObjectPos(planetVector);
				if (onlineItems.size() == 0) itemSpawnData.itemType = ObjID::BAT;
				else if (!grenadeCount) itemSpawnData.itemType = (rand() % 3) + 3;		//Spawns a random item (Baseball bat, potion or grenade)
				else itemSpawnData.itemType = (rand() % 2) + 3;
				itemSpawnData.x = temp.x;
				itemSpawnData.y = temp.y;
				itemSpawnData.z = temp.z;
				itemSpawnData.itemId = componentIdCounter;
				itemSpawnData.packetId = PacketType::ITEMSPAWN;


				if (itemSpawnData.itemType == ObjID::BAT) onlineItems.push_back(new BaseballBat(componentIdCounter));//ändra
				else if (itemSpawnData.itemType == ObjID::POTION) onlineItems.push_back(new Grenade(componentIdCounter));//ändra
				else if (itemSpawnData.itemType == ObjID::GRENADE) { onlineItems.push_back(new Grenade(componentIdCounter)); grenadeCount++; }
				physWorld.addPhysComponent(*onlineItems[onlineItems.size() - 1]);
				onlineItems[onlineItems.size() - 1]->setPosition(temp.x, temp.y, temp.z);
				onlineItems[onlineItems.size() - 1]->setInUseBy(-1);
				onlineItems[onlineItems.size() - 1]->setOnlineType(ObjID::BAT);
				onlineItems[onlineItems.size() - 1]->setOnlineId(componentIdCounter++);
				onlineItems[onlineItems.size() - 1]->setOnlineType(itemSpawnData.itemType);
				//std::cout << "ID Item: " << onlineItems.back()->getOnlineId() << "\n";

				sendBinaryDataAllPlayers(itemSpawnData, data);
				itemSpawnTimer = std::chrono::system_clock::now();
			}

			////Spawns a potion
			//if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - itemSpawnTimer)).count() > itemSpawnTimerLength)
			//{
			//	PotionSpawn PotionSpawnData;
			//	
			//	DirectX::XMFLOAT3 temp = randomizeObjectPos();
			//	PotionSpawnData.xPos = temp.x;
			//	PotionSpawnData.yPos = temp.y;
			//	PotionSpawnData.zPos = temp.z;
			//	PotionSpawnData.potionId = componentIdCounter;
			//	std::cout << "item spawn id: " << std::to_string(PotionSpawnData.potionId) << std::endl;
			//	PotionSpawnData.packetId = PacketType::POTIONSPAWN;

			//	onlineItems.push_back(new Component());//ändra
			//	physWorld.addPhysComponent(*onlineItems[onlineItems.size() - 1]);
			//	onlineItems[onlineItems.size() - 1]->setPosition(temp.x, temp.y, temp.z);;
			//	onlineItems[onlineItems.size() - 1]->setInUseBy(-1);
			//	onlineItems[onlineItems.size() - 1]->setOnlineId(componentIdCounter++);
			//	sendBinaryDataAllPlayers(PotionSpawnData, data);
			//	itemSpawnTimer = std::chrono::system_clock::now();
			//}

			bool endGame = false;


			//sends data based on the server tickrate
			if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - start)).count() > timerLength)
			{
				int sizeOfPackets = 0;

				//WHAT NEEDS TO BE DONE ON SERVER TO HANDLE LANDING MINIGAME
				switch (currentMinigame)
				{
				case MiniGames::LANDINGSPACESHIP:
					//WE NEED TO RECIEVE POINTS, ADD THEM AND SEND THEM TO THE PLAYERS

					LandingMiniGameScore lScore;
					lScore.packetId = PacketType::LANDINGMINIGAMESCORE;
					int playerTeam;
					lScore.pointsBlueTeam = 0.f;
					lScore.pointsRedTeam = 0.f;

					for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
					{
						playerTeam = (MAXNUMBEROFPLAYERS) / 2;
						playerTeam = (int)(playerTeam < i + 1);
						if (playerTeam == 0) lScore.pointsRedTeam += landingPoints[i];
						else lScore.pointsBlueTeam += landingPoints[i];
					}

					sendBinaryDataAllPlayers<LandingMiniGameScore>(lScore, data);
					sizeOfPackets += sizeof(LandingMiniGameScore);
					break;

				case MiniGames::KINGOFTHEHILL:
					endGame = miniGameKTH.update(data, onlineItems, physWorld, planetVector, componentIdCounter, totalTeamScores);
					break;

					/*default:
						break;*/
				}
				//if (currentMinigame == MiniGames::LANDINGSPACESHIP) continue; //We do not need to send more data if we are in landingMiniGame

				if (endGame)
				{

					//std::cout << "UwU 0\n";
					break;//breaks while
				}

				for (int i = 0; i < 10; i++)
				{
					physWorld.update(timerLength / 10.f);
				}

				

				//Check if any onlineItems are near after the physics update

				if (progress[0] < 4 || progress[1] < 4)
				{
					for (int i = 0; i < onlineItems.size(); i++)
					{
						static DirectX::SimpleMath::Vector3 vecToComp;
						static DirectX::SimpleMath::Vector3 objPos;
						if (onlineItems[i]->getOnlineType() == ObjID::COMPONENT)
						{
							for (int j = 0; j < spaceShipPos.size(); j++)
							{
								//if (!onlineItems[i].getActiveState()) continue;

								vecToComp = spaceShipPos[j];
								objPos = onlineItems[i]->getPhysicsComponent()->getPosV3();
								subtractionXMFLOAT3(vecToComp, objPos);

								if (getLength(vecToComp) <= 10.f && onlineItems[i]->getInUseById() != -1)
								{
									//onlineItems[i].setInactive();
									DirectX::XMFLOAT3 newCompPos = randomizeObjectPos(planetVector);
									onlineItems[i]->getPhysicsComponent()->setType(reactphysics3d::BodyType::STATIC);
									onlineItems[i]->setPosition(newCompPos.x, newCompPos.y, newCompPos.z);
									onlineItems[i]->getPhysicsComponent()->setType(reactphysics3d::BodyType::DYNAMIC);
									onlineItems[i]->setInUseBy(-1);
									ComponentAdded compAdded;
									compAdded.packetId = PacketType::COMPONENTADDED;
									compAdded.spaceShipTeam = j;
									compAdded.componentID = i;
									onlineItems[i]->setInUseBy(-1);

									totalTeamScores[j] += 25;
									shipComponentCounter[j]++;
									if (shipComponentCounter[j] == 4 && shipComponentCounter[!j] >= 4) totalTeamScores[!j] += 15;

									sendBinaryDataAllPlayers<ComponentAdded>(compAdded, data);
									sizeOfPackets += sizeof(ComponentAdded);

									//Checking if someone has won
									progress[j]++;
									if (progress[j] > 3)
									{
										timeToFly = true;
										startFly = std::chrono::system_clock::now();
									}

									std::cout << "Red team score : " << (int)totalTeamScores[0] << "\nBlue team score: " << (int)totalTeamScores[1] << "\n";
								}
							}
						}
						else if (onlineItems[i]->getOnlineType() == ObjID::GRENADE)
						{
							Grenade* grenade = dynamic_cast<Grenade*>(onlineItems[i]);
							if (grenade && grenade->explode())
							{
								grenade->updateExplosion(timerLength);//updates scale for explosion
								//std::cout<< "##############exploding" << std::endl;
								for (int j = 0; j < onlineItems.size(); j++)// checking grenade explosion with items
								{
									if (onlineItems[j] == onlineItems[i]) continue;
									vecToComp = onlineItems[j]->getPhysicsComponent()->getPosV3();
									objPos = onlineItems[i]->getPosXMFLOAT3();
									subtractionXMFLOAT3(vecToComp, objPos);

									if (getLength(vecToComp) <= grenade->getScaleExplosion())
									{
										float factor = 1.f / getLength(vecToComp);
										vecToComp *= factor;
										onlineItems[j]->getPhysicsComponent()->applyForceToCenter(reactphysics3d::Vector3(
											25000 * vecToComp.x, 25000 * vecToComp.y, 25000 * vecToComp.z));
									}
								}
								for (int j = 0; j < MAXNUMBEROFPLAYERS; j++)// checking grenade explosion with players
								{
									vecToComp.x = data.users[j].playa.getposition('x');
									vecToComp.y = data.users[j].playa.getposition('y');
									vecToComp.z = data.users[j].playa.getposition('z');

									

									objPos = onlineItems[i]->getPosXMFLOAT3();
									subtractionXMFLOAT3(vecToComp, objPos);
									if (getLength(vecToComp) <= grenade->getScaleExplosion())
									{
										//std::cout << "player check" << std::endl;
										float factor = 1.f / getLength(vecToComp);
										vecToComp *= factor;
										//data.users[j].playa.getPhysComp()->applyForceToCenter();
										data.users[j].playa.playerGotHit(reactphysics3d::Vector3(
											25000 * vecToComp.x, 25000 * vecToComp.y, 25000 * vecToComp.z));
										HitByGrenade hitByGrenade;
										hitByGrenade.packetId = HITBYGRENADE;
										hitByGrenade.playerThatUsedTheItem = 0;
										hitByGrenade.itemId = i;
										hitByGrenade.xForce = 25000 * vecToComp.x;
										hitByGrenade.yForce = 25000 * vecToComp.y;
										hitByGrenade.zForce = 25000 * vecToComp.z;
										sendBinaryDataOnePlayer<HitByGrenade>(hitByGrenade, data.users[j]);
										sizeOfPackets += sizeof(HitByGrenade);
									}
								}

								if (grenade->getScaleExplosion() >= 27.0f)//timer, until the scale is is bigger
								{
									std::cout << "Explosion done!\n";
									grenade->resetExplosion();
									DirectX::XMFLOAT3 newGrenadePos = randomizeObjectPos(planetVector);
									grenade->setPosition(newGrenadePos.x, newGrenadePos.y, newGrenadePos.z);
								}

							}
						}
					}
				}

				//physWorld.update(timerLength);
				//f�r varje spelare s� skicka deras position till alla klienter
				for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
				{
					//sendDataAllPlayers(pos, data);
					if (data.users[i].playa.getDeathState())
					{
						reactphysics3d::Vector3 tempPos = data.users[i].playa.getPhysComp()->getPosition();
						data.users[i].playa.setPosition(tempPos.x, tempPos.y, tempPos.z);
						data.users[i].playa.updatePosViaPhysComp();
					}

					PositionRotation prMatrix;
					prMatrix.ifDead = data.users[i].playa.getDeathState();
					prMatrix.matrix = data.users[i].playa.getMatrix();
					prMatrix.packetId = PacketType::POSITIONROTATION;
					prMatrix.playerId = i;
					prMatrix.xRot = data.users[i].playa.getPhysComp()->getRotation().x;
					prMatrix.yRot = data.users[i].playa.getPhysComp()->getRotation().y;
					prMatrix.zRot = data.users[i].playa.getPhysComp()->getRotation().z;
					prMatrix.wRot = data.users[i].playa.getPhysComp()->getRotation().w;
					data.users[i].playa.getAnimData(prMatrix.AnimId, prMatrix.animSpeed);

					sendBinaryDataAllPlayers(prMatrix, data);
					sizeOfPackets += sizeof(PositionRotation);
				}
				for (int i = 0; i < onlineItems.size(); i++)
				{
					ComponentPosition compPosition;
					compPosition.ComponentId = onlineItems[i]->getOnlineId();
					compPosition.packetId = PacketType::COMPONENTPOSITIONNEW;
					compPosition.x = onlineItems[i]->getposition('x');
					compPosition.y = onlineItems[i]->getposition('y');
					compPosition.z = onlineItems[i]->getposition('z');
					compPosition.xRot = onlineItems[i]->getPhysicsComponent()->getRotation().x;
					compPosition.yRot = onlineItems[i]->getPhysicsComponent()->getRotation().y;
					compPosition.zRot = onlineItems[i]->getPhysicsComponent()->getRotation().z;
					compPosition.wRot = onlineItems[i]->getPhysicsComponent()->getRotation().w;
					compPosition.playerOnlineId = onlineItems[i]->getInUseById();
					//compPosition.quat = onlineItems[i].getPhysicsComponent()->getRotation();
					sendBinaryDataAllPlayers<ComponentPosition>(compPosition, data);
					sizeOfPackets += sizeof(ComponentPosition);

				}
				start = std::chrono::system_clock::now();
				Time::Update;
			}

			//Waits for the ships to fly away before starting minigames
			if (timeToFly)
			{
				if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - startFly)).count() > 10.f)
				{
					std::cout << "SENT START MINIGAMES\n";
					MinigameStart startMinigame;
					startMinigame.packetId = PacketType::STARTMINIGAMES;
					startMinigame.minigame = MiniGames::STARTOFINTERMISSION;
					currentMinigame = MiniGames::INTERMISSION;

					progress[0] = 0;
					progress[1] = 0;
					startMinigame.pointsRed = totalTeamScores[0];
					startMinigame.pointsBlue = totalTeamScores[1];
					sendBinaryDataAllPlayers<MinigameStart>(startMinigame, data);
					timeToFly = false;
				}
			}

			if (DebugSizePackets.getTimePassed(1.0f))
			{
				//std::cout << "SizeOfPackets: " << sizeOfPackets << std::endl;
				sizeOfPackets = 0;
				DebugSizePackets.resetStartTime();
			}

		}

		GAMEONGOING = false;
		

		//std::cout << "UwU 1\n";
		TimeStruct deleteThreadsTimer;
		deleteThreadsTimer.resetStartTime();
		while (!deleteThreadsTimer.getTimePassed(2.0f))
		{
			//std::cout << "timer loop\n";
		}
		//delete circBuffer;
		
		for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		{
			data.users[i].tcpSocket.disconnect();
		}
		
		for (int i = 0; i < planetVector.size(); i++)
		{
			delete planetVector[i];
		}


		for (int i = 0; i < onlineItems.size(); i++)
		{
			delete onlineItems[i];
		}
		onlineItems.clear();

		//std::cout << "UwU 2\n";
		//for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		//{
		//	mutex.unlock();
		//	recvThread[i]->join();
		//	delete recvThread[i];
		//}

		data.tcpListener.close();
		


		//std::cout << "UwU 3\n";
		//delete threads
	}
	std::cout << "end\n";
	(void)getchar();
	
	return 0;

	//Hidden code
	/*if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - startComponentTimer)).count() > timerComponentLength && !once)
	{
		SpawnComponent cData = SpawnOneComponent(components);
		physWorld.addPhysComponent(components[components.size() - 1]);
		components[components.size() - 1].setPosition(cData.x, cData.y, cData.z);
		sendBinaryDataAllPlayers(cData, data);
		startComponentTimer = std::chrono::system_clock::now();
		once = true;
	}*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging 

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file