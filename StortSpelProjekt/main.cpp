#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <time.h>

#include "Console.h"
#include "MemoryLeackChecker.h"
#include "SoundCollection.h"
#include "SoundLibrary.h"
#include "Client.h"
#include "Game.h"
#include "Menu.h"
#include "WindowHelper.h"
#include "D3D11Helper.h"

#include "ImGuiHelper.h"

//Ta bort innan merge med main?
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "SettingsMenu.h"
#include "ExitUI.h"
#include "CreditsMenu.h"
#include "WinMenu.h"
#include "LoseMenu.h"
#include "Lobby.h"
#include "ControlMenu.h"
#include "Lobby.h"
#include "InputIP.h"
#include "Time.h"

const int NROFPLAYERS = 4;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstace, _In_ LPWSTR lpCmdLine, _In_ int nCmdShhow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand((unsigned)time(0));

	SoundCollection::Load();
	SoundLibrary soundLibrary;

	//Console::Activate(); // activate console for cout and cin, to destroy console call "Console::Destroy();" 
	//std::cout << "test print \n"; //test print

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::GetIO().IniFilename = nullptr;

	int currentTeam;

	UINT WIDTH = 1920;
	UINT HEIGHT = 1080;

	HWND window;
	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShhow, window))
	{
		ErrorLog::Log("Could not set up window!");
		return -1;
	}

	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;

	if (!CreateInterfaces(WIDTH, HEIGHT, window, device, immediateContext, swapChain))
	{
		ErrorLog::Log("Could not set up D3D11!");
		return -1;
	}

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, immediateContext);

	
	Client* client = nullptr;

	State* currentState = new Menu();
	GAMESTATE stateInfo = NOCHANGE;

	MSG msg = {};
	Time::Start();
	ExitUI::Init();

	while (msg.message != WM_QUIT && stateInfo != EXIT)
	{

		stateInfo = currentState->Update();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sound::Update();

		if (ExitUI::exit)
			stateInfo = EXIT;

		if (stateInfo != NOCHANGE)
		{
			switch (stateInfo)
			{
			case GAME:
				delete currentState;
				currentState = new Game(immediateContext, device, GPU::swapChain, window, WIDTH, HEIGHT, NROFPLAYERS, client, currentTeam);
				break;
			case SETTINGS:
				delete currentState;
				currentState = new SettingsMenu();
				break;
			case CREDITS:
				delete currentState;
				currentState = new CreditsMenu();
				break;
			case WIN:
				delete currentState;
				currentState = new WinMenu(currentTeam);
				break;
			case LOSE:
				delete currentState;
				currentState = new LoseMenu(currentTeam);
				break;
			case CONTROL:
				delete currentState;
				currentState = new ControlMenu();
				break;
			case MENU:
				delete currentState;
				currentState = new Menu();
				break;
			case LOBBY:
				delete currentState;
				currentState = new Lobby(client);
				break;
			case INPUTIP:
				delete currentState;
				currentState = new InputIP();
				break;

			default:
				break;
			}
		}
		currentState->Render();
		ExitUI::Draw();
		Time::Update();
		GPU::swapChain->Present(1, 0);

	}

	#pragma region Deallocation
	delete currentState;
	
	ChangeResolution(SettingsUI::nativeWidth, SettingsUI::nativeHeight);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (Console::IsOpen())
		Console::Destroy();

	device->Release();
	immediateContext->Release();
	GPU::swapChain->Release();

	#pragma endregion
	
	return 0;
}