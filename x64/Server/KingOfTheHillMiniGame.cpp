#include "KingOfTheHillMiniGame.h"
#include "ObjectId.h"

KingOfTheHillMiniGame::KingOfTheHillMiniGame(const short& nrOfPlayers)
	:kingOfTheHillOrigo(0,40,-40), radius(30), nrOfPlayers(nrOfPlayers), team1Score(0), team2Score(0), pointsToAdd(10), time(5), goalScore(100), timeToSpawnItems(5)
{
	this->timer = std::chrono::system_clock::now();
	this->timerToSend = std::chrono::system_clock::now();
	this->itemSpawnTimer = std::chrono::system_clock::now();
	timerSend =  4.0f;
}

void KingOfTheHillMiniGame::sendKingOfTheHillZone(serverData& data)
{
	CreateZone zone;
	zone.xPos = kingOfTheHillOrigo.x;
	zone.yPos = kingOfTheHillOrigo.y;
	zone.zPos = kingOfTheHillOrigo.z;
	zone.scale = radius;
	zone.packetId = PacketType::CREATEZONE;
	sendBinaryDataAllPlayers<CreateZone>(zone, data);
}


void KingOfTheHillMiniGame::update(serverData& data, std::vector<Item*>& onlineItems, PhysicsWorld& physWorld, std::vector<Planet*> planets, int& componentIdCounter, float totalTeamScores [])
{
	static float xPos;
	static float yPos;
	static float zPos;

	redInside = false;
	blueInside = false;

	static DirectX::XMFLOAT3 playerPos;
	if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - timer)).count() > time)
	{
		for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
		{
			xPos = data.users[i].playa.getposition('x');
			yPos = data.users[i].playa.getposition('y');
			zPos = data.users[i].playa.getposition('z');
			playerPos = DirectX::XMFLOAT3(xPos, yPos, zPos);
			subtractionXMFLOAT3(playerPos, kingOfTheHillOrigo);
			if (getLength(playerPos) <= radius)
			{
				int playerTeam;
				playerTeam = (MAXNUMBEROFPLAYERS) / 2;
				playerTeam = (int)(playerTeam < i + 1);
				//std::cout << "Innanf�r zonen\n";

				if (playerTeam == 0)
				{
					redInside = true;
					team1Score += pointsToAdd;
					std::cout << "team1 got points, total points: " << team1Score << "\n";
				}
				
				if (playerTeam == 1)
				{
					blueInside = true;
					team2Score += pointsToAdd;
					std::cout << "team2 got points, total points: " << team2Score << "\n";
				}
			}
			else
			{
				//std::cout << "utanf�r zonen\n";
			}
		}

		KTHPoints sendPoints;
		sendPoints.packetId = PacketType::KTHPOINTS;
		sendPoints.redPoints = team1Score;
		sendPoints.bluePoints = team2Score;
		if (blueInside && redInside) sendPoints.teamColor = 0;
		else if (blueInside) sendPoints.teamColor = 1;
		else if (redInside) sendPoints.teamColor = 2;
		else sendPoints.teamColor = 3;
		sendBinaryDataAllPlayers<KTHPoints>(sendPoints, data);
		timer = std::chrono::system_clock::now();
	}

	if (team1Score >= goalScore)
	{
		if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - this->timerToSend)).count() > timerSend)
		{
			int catchUp = 0;
			if (totalTeamScores[0] < (totalTeamScores[1] + 50)) catchUp = 25;
			totalTeamScores[0] += 100 + catchUp;
			totalTeamScores[1] += (int)(((float)team2Score / (float)team1Score) * 100.f);
			for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
			{
				int playerTeam;
				playerTeam = (MAXNUMBEROFPLAYERS) / 2;
				playerTeam = (int)(playerTeam < i + 1);

				if (playerTeam == 0)
				{
					if (totalTeamScores[0] < totalTeamScores[1])
					{
						Loser lose;
						lose.packetId = PacketType::LOSER;
						sendBinaryDataOnePlayer<Loser>(lose, data.users[i]);
					}
					else
					{
						winner win;
						win.packetId = PacketType::WINNER;
						sendBinaryDataOnePlayer<winner>(win, data.users[i]);
					}

				}
				else if (playerTeam == 1)
				{
					if (totalTeamScores[0] < totalTeamScores[1])
					{
						winner win;
						win.packetId = PacketType::WINNER;
						sendBinaryDataOnePlayer<winner>(win, data.users[i]);
					}
					else
					{
						Loser lose;
						lose.packetId = PacketType::LOSER;
						sendBinaryDataOnePlayer<Loser>(lose, data.users[i]);
					}
				}
			}
			this->timerToSend = std::chrono::system_clock::now();
			std::cout << "Red team score : " << (int)totalTeamScores[0] << "\nBlue team score: " << (int)totalTeamScores[1] << "\n";
		}
	}

	if (team2Score >= goalScore)
	{
		if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - this->timerToSend)).count() > timerSend)
		{
			int catchUp = 0;
			if (totalTeamScores[1] < (totalTeamScores[0] + 50)) catchUp = 25;
			totalTeamScores[1] += 100 + catchUp;
			totalTeamScores[0] += (int)(((float)team1Score / (float)team2Score) * 100.f);

			for (int i = 0; i < MAXNUMBEROFPLAYERS; i++)
			{
				int playerTeam;
				playerTeam = (MAXNUMBEROFPLAYERS) / 2;
				playerTeam = (int)(playerTeam < i + 1);

				if (playerTeam == 0)
				{
					if (totalTeamScores[0] > totalTeamScores[1])
					{
						winner win;
						win.packetId = PacketType::WINNER;
						sendBinaryDataOnePlayer<winner>(win, data.users[i]);
					}
					else
					{
						
						Loser lose;
						lose.packetId = PacketType::LOSER;
						sendBinaryDataOnePlayer<Loser>(lose, data.users[i]);
					}
					
				}
				else if (playerTeam == 1)
				{
					if (totalTeamScores[0] > totalTeamScores[1])
					{
						Loser lose;
						lose.packetId = PacketType::LOSER;
						sendBinaryDataOnePlayer<Loser>(lose, data.users[i]);
					}
					else
					{
						winner win;
						win.packetId = PacketType::WINNER;
						sendBinaryDataOnePlayer<winner>(win, data.users[i]);
					}
				}
			}
			this->timerToSend = std::chrono::system_clock::now();
			std::cout << "Red team score : " << (int)totalTeamScores[0] << "\nBlue team score: " << (int)totalTeamScores[1] << "\n";
		}
	}

	if (((std::chrono::duration<float>)(std::chrono::system_clock::now() - this->itemSpawnTimer)).count() > timeToSpawnItems && onlineItems.size() <= 10)
	{

		ItemSpawn itemSpawnData;
		DirectX::XMFLOAT3 temp = randomizeObjectPos(planets);
		itemSpawnData.x = temp.x;
		itemSpawnData.y = temp.y;
		itemSpawnData.z = temp.z;
		itemSpawnData.itemId = componentIdCounter;
		std::cout << "item spawn id: " << std::to_string(itemSpawnData.itemId) << std::endl;
		itemSpawnData.packetId = PacketType::ITEMSPAWN;
		itemSpawnData.itemType = ObjID::BAT;

		onlineItems.push_back(new BaseballBat(componentIdCounter));//�ndra
		physWorld.addPhysComponent(*onlineItems[onlineItems.size() - 1]);
		onlineItems[onlineItems.size() - 1]->setPosition(temp.x, temp.y, temp.z);;
		onlineItems[onlineItems.size() - 1]->setInUseBy(-1);
		onlineItems[onlineItems.size() - 1]->setOnlineId(componentIdCounter++);
		sendBinaryDataAllPlayers(itemSpawnData, data);
		itemSpawnTimer = std::chrono::system_clock::now();
	}

	//fixa f�r team 2

}