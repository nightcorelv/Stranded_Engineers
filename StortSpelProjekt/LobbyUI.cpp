#include "stdafx.h"
#include "LobbyUI.h"
#include "Input.h"

void LobbyUI::SpritePass()
{

	hit_ready = ready_b.IntersectMouse();

	if (Input::KeyPress(KeyCode::MOUSE_L))
	{
		if (hit_ready)
		{
			ready = !ready;
		}
	}

	hit_ready ? ready2_r.Draw() : ready_b.Draw();
	ready ? checkbox_true.Draw() : checkbox_false.Draw();
	p1 ? player1_checkbox_true.Draw() : player1_checkbox_false.Draw();
	p2 ? player2_checkbox_true.Draw() : player2_checkbox_false.Draw();
	p3 ? player3_checkbox_true.Draw() : player3_checkbox_false.Draw();
	p4 ? player4_checkbox_true.Draw() : player4_checkbox_false.Draw();

	p1_connect ? player1_checkbox_true_r.Draw() : player1_checkbox_false_r.Draw();
	p2_connect ? player2_checkbox_true_r.Draw() : player2_checkbox_false_r.Draw();
	p3_connect ? player3_checkbox_true_r.Draw() : player3_checkbox_false_r.Draw();
	p4_connect ? player4_checkbox_true_r.Draw() : player4_checkbox_false_r.Draw();

	player1.Draw();
	player2.Draw();
	player3.Draw();
	player4.Draw();

	readyText1.Draw();
	readyText2.Draw();
	readyText3.Draw();
	readyText4.Draw();
	//connectedPlayersText.Draw();

//	std::string number = std::to_string(connectedPlayers);
//	for (int i = 0; i < number.size(); i++)
//	{
//#define offsetY 250 
//#define centerX 632
//#define centerY 340
//#define textSpace 20.0f
//#define posY1 centerY - 50
//#define posX1 (centerX + (i * textSpace))-25
//		switch (number.at(i))
//		{
//
//		case '0':
//		{
//			n0.SetPosition({ posX1, posY1 });
//			n0.Draw();
//
//		}break;
//
//		case '1':
//		{
//			n1.SetPosition({ posX1, posY1 });
//			n1.Draw();
//		}break;
//
//		case '2':
//		{
//			n2.SetPosition({ posX1, posY1 });
//			n2.Draw();
//		}break;
//
//		case '3':
//		{
//			n3.SetPosition({ posX1, posY1 });
//			n3.Draw();
//		}break;
//
//		case '4':
//		{
//			n4.SetPosition({ posX1, posY1 });
//			n4.Draw();
//		}break;
//
//		case '5':
//		{
//			n5.SetPosition({ posX1, posY1 });
//			n5.Draw();
//		}break;
//
//		case '6':
//		{
//			n6.SetPosition({ posX1, posY1 });
//			n6.Draw();
//		}break;
//
//		case '7':
//		{
//			n7.SetPosition({ posX1, posY1 });
//			n7.Draw();
//		}break;
//
//		case '8':
//		{
//			n8.SetPosition({ posX1, posY1 });
//			n8.Draw();
//		}break;
//
//		case '9':
//		{
//			n9.SetPosition({ posX1, posY1 });
//			n9.Draw();
//		}break;
//
//		}
//
//	}
}

LobbyUI::LobbyUI()
{
	GUI::Init();// ta v�ck senare eftersom det ska h�nda p� ett annat st�lle 
#define centerX 632
#define centerY 340
#define scale 0.3f,0.3f
#define scale2 0.5f,0.5f
	Loading = GUISprite(centerX, centerY);
	Loading.Load(GPU::device, L"../Sprites/Loding.png");
	Loading.SetScale(1, 1);

	ready_b = GUISprite(centerX, centerY + 200);
	ready_b.Load(GPU::device, L"../Sprites/Menu/ready.png");
	ready_b.SetScale(scale);

	ready2_b = GUISprite(centerX, centerY + 200);
	ready2_b.Load(GPU::device, L"../Sprites/Menu/ready2.png");
	ready2_b.SetScale(scale);

	ready2_r = GUISprite(centerX, centerY + 200);
	ready2_r.Load(GPU::device, L"../Sprites/Menu/ready_r.png");
	ready2_r.SetScale(scale);

	ready_r = GUISprite(centerX, centerY + 200);
	ready_r.Load(GPU::device, L"../Sprites/Menu/ready_r.png");
	ready_r.SetScale(scale);

	ready2_r = GUISprite(centerX, centerY + 200);
	ready2_r.Load(GPU::device, L"../Sprites/Menu/ready2_r.png");
	ready2_r.SetScale(scale);

	readyText1 = GUISprite(centerX + 100, centerY - 200);
	readyText1.Load(GPU::device, L"../Sprites/Lobby/readyText.png");
	readyText1.SetScale(scale2);

	readyText2 = GUISprite(centerX + 100, centerY - 150);
	readyText2.Load(GPU::device, L"../Sprites/Lobby/readyText.png");
	readyText2.SetScale(scale2);

	readyText3 = GUISprite(centerX + 100, centerY - 100);
	readyText3.Load(GPU::device, L"../Sprites/Lobby/readyText.png");
	readyText3.SetScale(scale2);

	readyText4 = GUISprite(centerX + 100, centerY - 50);
	readyText4.Load(GPU::device, L"../Sprites/Lobby/readyText.png");
	readyText4.SetScale(scale2);
	

	checkbox_false = GUISprite(centerX - 150, centerY + 200);
	checkbox_false.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	checkbox_false.SetScale(0.75f, 0.75f);

	player1_checkbox_false = GUISprite(centerX - 50, centerY - 200);
	player1_checkbox_false.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player1_checkbox_false.SetScale(0.75f, 0.75f);

	player2_checkbox_false = GUISprite(centerX - 50, centerY - 150);
	player2_checkbox_false.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player2_checkbox_false.SetScale(0.75f, 0.75f);

	player3_checkbox_false = GUISprite(centerX - 50, centerY - 100);
	player3_checkbox_false.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player3_checkbox_false.SetScale(0.75f, 0.75f);

	player4_checkbox_false = GUISprite(centerX - 50, centerY - 50);
	player4_checkbox_false.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player4_checkbox_false.SetScale(0.75f, 0.75f);

	player1_checkbox_false_r = GUISprite(centerX + 250, centerY - 200);
	player1_checkbox_false_r.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player1_checkbox_false_r.SetScale(0.75f, 0.75f);

	player2_checkbox_false_r = GUISprite(centerX + 250, centerY - 150);
	player2_checkbox_false_r.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player2_checkbox_false_r.SetScale(0.75f, 0.75f);

	player3_checkbox_false_r = GUISprite(centerX + 250, centerY - 100);
	player3_checkbox_false_r.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player3_checkbox_false_r.SetScale(0.75f, 0.75f);

	player4_checkbox_false_r = GUISprite(centerX + 250, centerY - 50);
	player4_checkbox_false_r.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	player4_checkbox_false_r.SetScale(0.75f, 0.75f);

	checkbox_true = GUISprite(centerX - 150, centerY + 200);
	checkbox_true.Load(GPU::device, L"../Sprites/box_checked2.png");
	checkbox_true.SetScale(0.75f, 0.75f);

	player1_checkbox_true = GUISprite(centerX - 50, centerY - 200);
	player1_checkbox_true.Load(GPU::device, L"../Sprites/box_checked2.png");
	player1_checkbox_true.SetScale(0.75f, 0.75f);

	player2_checkbox_true = GUISprite(centerX - 50, centerY - 150);
	player2_checkbox_true.Load(GPU::device, L"../Sprites/box_checked2.png");
	player2_checkbox_true.SetScale(0.75f, 0.75f);

	player3_checkbox_true = GUISprite(centerX - 50, centerY - 100);
	player3_checkbox_true.Load(GPU::device, L"../Sprites/box_checked2.png");
	player3_checkbox_true.SetScale(0.75f, 0.75f);

	player4_checkbox_true = GUISprite(centerX - 50, centerY - 50);
	player4_checkbox_true.Load(GPU::device, L"../Sprites/box_checked2.png");
	player4_checkbox_true.SetScale(0.75f, 0.75f);

	player1_checkbox_true_r = GUISprite(centerX + 250, centerY - 200);
	player1_checkbox_true_r.Load(GPU::device, L"../Sprites/box_checked2.png");
	player1_checkbox_true_r.SetScale(0.75f, 0.75f);

	player2_checkbox_true_r = GUISprite(centerX + 250, centerY - 150);
	player2_checkbox_true_r.Load(GPU::device, L"../Sprites/box_checked2.png");
	player2_checkbox_true_r.SetScale(0.75f, 0.75f);

	player3_checkbox_true_r = GUISprite(centerX + 250, centerY - 100);
	player3_checkbox_true_r.Load(GPU::device, L"../Sprites/box_checked2.png");
	player3_checkbox_true_r.SetScale(0.75f, 0.75f);

	player4_checkbox_true_r = GUISprite(centerX + 250, centerY - 50);
	player4_checkbox_true_r.Load(GPU::device, L"../Sprites/box_checked2.png");
	player4_checkbox_true_r.SetScale(0.75f, 0.75f);

	player1 = GUISprite(centerX - 200, centerY - 200);
	player1.Load(GPU::device, L"../Sprites/Lobby/Player1.png");
	player1.SetScale(0.75f, 0.75f);

	player2 = GUISprite(centerX - 200, centerY - 150);
	player2.Load(GPU::device, L"../Sprites/Lobby/Player2.png");
	player2.SetScale(0.75f, 0.75f);

	player3 = GUISprite(centerX - 200, centerY - 100);
	player3.Load(GPU::device, L"../Sprites/Lobby/Player3.png");
	player3.SetScale(0.75f, 0.75f);

	player4 = GUISprite(centerX - 200, centerY - 50);
	player4.Load(GPU::device, L"../Sprites/Lobby/Player4.png");
	player4.SetScale(0.75f, 0.75f);

	connectedPlayersText = GUISprite(centerX - 200, centerY - 50);
	connectedPlayersText.Load(GPU::device, L"../Sprites/Lobby/connectedPlayer.png");
	connectedPlayersText.SetScale(0.75f, 0.75f);

#define nrScale 0.75f
	n0 = GUISprite(centerX + 200, centerY - 50);
	n0.Load(GPU::device, L"../Sprites/Numbers/0.png");
	n0.SetScale(nrScale, nrScale);

	n1 = GUISprite(centerX + 200, centerY - 50);
	n1.Load(GPU::device, L"../Sprites/Numbers/1.png");
	n1.SetScale(nrScale, nrScale);

	n2 = GUISprite(centerX + 200, centerY - 50);
	n2.Load(GPU::device, L"../Sprites/Numbers/2.png");
	n2.SetScale(nrScale, nrScale);

	n3 = GUISprite(centerX + 200, centerY - 50);
	n3.Load(GPU::device, L"../Sprites/Numbers/3.png");
	n3.SetScale(nrScale, nrScale);

	n4 = GUISprite(centerX + 200, centerY - 50);
	n4.Load(GPU::device, L"../Sprites/Numbers/4.png");
	n4.SetScale(nrScale, nrScale);

	n5 = GUISprite(centerX + 200, centerY - 50);
	n5.Load(GPU::device, L"../Sprites/Numbers/5.png");
	n5.SetScale(nrScale, nrScale);

	n6 = GUISprite(centerX + 200, centerY - 50);
	n6.Load(GPU::device, L"../Sprites/Numbers/6.png");
	n6.SetScale(nrScale, nrScale);

	n7 = GUISprite(centerX + 200, centerY - 50);
	n7.Load(GPU::device, L"../Sprites/Numbers/7.png");
	n7.SetScale(nrScale, nrScale);

	n8 = GUISprite(centerX + 200, centerY - 50);
	n8.Load(GPU::device, L"../Sprites/Numbers/8.png");
	n8.SetScale(nrScale, nrScale);

	n9 = GUISprite(centerX + 200, centerY - 50);
	n9.Load(GPU::device, L"../Sprites/Numbers/9.png");
	n9.SetScale(nrScale, nrScale);
}

LobbyUI::~LobbyUI()
{

}

void LobbyUI::Draw()
{
	GUI::Begin();

	SpritePass();

	GUI::End();
}

void LobbyUI::DrawLoading()
{
	GUI::Begin();

	Loading.Draw();

	GUI::End();
}
