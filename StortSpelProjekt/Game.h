#pragma once
#include "GameInclude.h"
#include "HudUI.h"
#include "HazardBox.h"

struct wirefameInfo
{
	DirectX::XMFLOAT3 wireframeClr;
	float padding;
};

class Game : public State
{
public:
	inline static bool IFONLINE = true;
private:
	ID3D11DeviceContext* immediateContext;
	HWND* window;
	MiniGames currentMinigame;
	GAMESTATE currentGameState;
	Sound gameMusic;
	ImGuiHelper imGui;

	const int NROFPLAYERS;

	bool wireframe = false;
	bool objectDraw = true;
	bool drawDebug = false;
	bool landingMinigame = false;

	DirectX::GamePad* gamePad;
	GravityField* field;
	GravityField* oldField;
	bool changedPlanet = false;

	//Used for delta time
	float endTimer;
	float dt;
	std::chrono::time_point<std::chrono::system_clock> currentTime;
	std::chrono::time_point<std::chrono::system_clock> lastUpdate;

	//Server related variables
	std::chrono::time_point<std::chrono::system_clock> serverStart;
	float serverTimerLength =  1.f / 60.0f;
	Client* client;

	ModelManager manager;
	ID3D11Buffer* vBuff;
	ID3D11Buffer* iBuff;
	Mesh* tmpMesh;
	Mesh* otherTmpMesh;
	std::vector<int> subMeshRanges;
	std::vector<int> verticies;
	ID3D11ShaderResourceView* tempSRV;
	AnimationData animData;
	AnimatedMesh* sexyMan;

	//Gravity vector and velocity for the player (grav is "constant", velocity is "dynmic")
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 grav;
	BasicRenderer basicRenderer;
	GravityField* planetGravityField;
	PhysicsWorld physWorld;
	std::vector<Planet*> planetVector;
	AsteroidHandler* asteroids;
	PacketEventManager* packetEventManager;
	std::vector<Player*> players;
	std::vector<Item*> onlineItems;

	//variables to handle packets
	CircularBufferClient* circularBuffer;
	Camera camera;
	bool velocityCamera = true;
	bool fadedIn = false;
	Arrow* arrow;
	SkyboxObj skybox;
	Player* currentPlayer;
	Mesh* planetMeshes;

	Mesh* tmpMesh2;
	Mesh* tmpMesh3;
	Mesh* tmpMesh4;
	std::vector<ID3D11ShaderResourceView*> planetSRV_online;
	GameObject* testCube;

	//Items
	int id;
	BaseballBat* baseballBat;
	Grenade* grenade;
	HazardBox* testBox;
	//Objects
	std::vector<Component*> components;
	std::vector<GameObject*> gameObjects;
	std::vector<SpaceShip*> spaceShips;
	std::vector<Item*> items;
	std::vector< Mesh*> meshes;
	LightHandler ltHandler;
	ImGuiHelper* imguiHelper;
	PlayerVectors playerVecRenderer;
	std::vector<ParticleEmitter> ptEmitters;
	CaptureZone* captureZone;

	
	//HUD
	HudUI ui;
	MiniGameUI miniGameUI;
	LandingHud landingUi;

	
	float landingMiniGamePoints = 0;


	//InterMissionVariables
	int Stage = 0;
	float totalTime;
	float timer;
	DirectX::XMFLOAT3 centerPos;
	DirectX::XMFLOAT2 offset;
	UINT HEIGHT;
	UINT WIDTH;
	bool firstFrame = false;

	void loadObjects();
	void drawShadows();
	void drawFresnel();
	void drawIcons();
	void drawObjects(bool drawDebug);
	void drawParticles();
	void drawNormalObjects();
	ID3D11ShaderResourceView* loadTexture(const std::string& fileName);

	GAMESTATE updateComponentGame();
	GAMESTATE startLanding();
	GAMESTATE updateLandingGame();
	GAMESTATE updateKingOfTheHillGame();
	GAMESTATE startIntermission();
	GAMESTATE updateIntermission();
	void randomizeObjectPos(GameObject* item);

public:
	inline static int teamScores[2]{ 0, 0 };
	inline static float teamScoreLandingMiniGame = 0;
	inline static float enemyTeamScoreLandingMiniGame = 0;
	Game(ID3D11DeviceContext* immediateContext, ID3D11Device* device, IDXGISwapChain* swapChain, HWND& window, UINT WIDTH, UINT HEIGHT,
		const int NROFPLAYERS, Client* client, int& currentTeam);
	virtual ~Game() override;

	// Inherited via State
	virtual GAMESTATE Update() override;
	virtual void Render() override;
};