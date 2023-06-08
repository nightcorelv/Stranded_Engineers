#include "stdafx.h"
#include "ImGuiHelper.h"
#include "Profiler.h"
#include "Player.h"


ImGuiHelper::ImGuiHelper(Client*& client)
	:client(client)
{
	receivedMsg = "not yet received!";
}

ImGuiHelper::ImGuiHelper()
	:client(nullptr)
{
}

ImGuiHelper::~ImGuiHelper()
{
}

void ImGuiHelper::drawInterface(std::string message)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	

	ImGui::NewFrame();
	{
		bool begun = ImGui::Begin("Stranded engineers");
		if (begun)
		{
			
		}

		ImGui::Text("My port: %d", client->getport());



		char buf[255]{};
		std::string ip;
		if (ImGui::InputText("ipAdress", buf, sizeof(buf)))
		{
			client->setIpAndPort(buf, 2001);
		}

		//selector.add(tcpSocket);

		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiHelper::setupImGui(float bgColour[])
{
	ImGuiStyle* style = &ImGui::GetStyle();
	style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.5f, 0.0f, 1.00f);
}

void ImGuiHelper::react3D(bool& wireframe, bool &drawObjects, bool& landingMinigame, const float& dt, bool& velocityCamera, Player* currentPlayer)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();



	ImGui::NewFrame();
	{
		float fps = 1.f / dt;
		bool begun = ImGui::Begin("React Physics 3D");
		if (begun)
		{
			std::string tmpStr = "FPS: " + std::to_string(fps);
			ImGui::Text(tmpStr.c_str());

			ImVec4 col(0.0f, 0.8f, 0.0f, 1.0f);
			//ImGui::TextColored(col, ("Vram: " + std::to_string(getVramUsage())).c_str());
			//ImGui::TextColored(col, ("Ram: " + std::to_string(getRamUsage())).c_str());


			ImGui::Checkbox("Draw collision boxes", &wireframe);
			ImGui::Checkbox("Draw objects", &drawObjects);

			/*if (ImGui::Button("vram Usage"))
			{
				std::cout << std::to_string(getVramUsage());
			}*/
			std::string tmpStrX = "xPos: "+std::to_string(currentPlayer->getPos().x);
			std::string tmpStrY = "yPos: " + std::to_string(currentPlayer->getPos().y);
			std::string tmpStrZ = "zPos: " + std::to_string(currentPlayer->getPos().z);
			ImGui::Checkbox("Landing Minigame", &landingMinigame);
			ImGui::Checkbox("Camera with collision", &velocityCamera);
			ImGui::Text(tmpStrX.c_str());
			ImGui::Text(tmpStrY.c_str());
			ImGui::Text(tmpStrZ.c_str());

			std::string playerHoldingItem;
			if (currentPlayer->getItem() == nullptr)
			{
				playerHoldingItem = "Player item == nullptr\n";
			}
			else
			{
				playerHoldingItem = "Player item != nullptr\n";
			}

			ImGui::Text(playerHoldingItem.c_str());
		}

		ImGui::End();

		bool playerMovementStuff = ImGui::Begin("Player Movement");
		if (playerMovementStuff)
		{
			playerMovement* pInfo = currentPlayer->getPlayerMovementInfo();
			ImGui::SliderFloat("Acceleration Speed", &pInfo->speed, 0.f, 500.f);
			ImGui::SliderFloat("Max Speed", &pInfo->maxVel, 0.f, 100.f);
			ImGui::SliderFloat("Jump Height", &pInfo->jump, 0.f, 500.f);
			ImGui::SliderFloat("Player Deacceleration Timer", &pInfo->deaccTimer, 0.1f, 5.f);
			ImGui::SliderFloat("Push Power (PP)", &pInfo->pushPower, 0.0f, 100.f);
		}
		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiHelper::spriteBegin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiHelper::spriteFloat(const std::string name,float& x, const float min, const float max)
{
	ImGui::Begin("sprite pos: ");

	ImGui::SliderFloat(std::string(name + " X: ").c_str(), &x, min, max);

	ImGui::End();
}

void ImGuiHelper::spriteEnd()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
