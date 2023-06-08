#include "stdafx.h"
#include "GUISprite.h"
#include "ExitUI.h"
#include "Input.h"

void ExitUI::HandleInputs()
{

	if (Input::KeyPress(KeyCode::ESC))
	{
		show = !show;
	}


	if (show)
	{
		hit_exit = exitb.IntersectMouse();
		hit_continue = continueb.IntersectMouse();
		hit_close = close.IntersectMouse();

		if (hit_exit)
		{
			if (Input::KeyDown(KeyCode::MOUSE_L))
			{
				exit = true;
			}
		}

		if (hit_continue)
		{
			if (Input::KeyDown(KeyCode::MOUSE_L))
			{
				show = false;
			}
		}

		if (hit_close)
		{
			if (Input::KeyDown(KeyCode::MOUSE_L))
			{
				show = false;
			}
		}

	}
	else
	{
		hit_exit = hit_continue = hit_close = false;
	}

	

}

void ExitUI::SpritePass()
{
	
	if (show)
	{
		bg.Draw();
		menuBg.Draw();
		menuBg.Draw();


		hit_exit ? exitb2.Draw() : exitb.Draw();
		hit_continue ? continueb2.Draw() : continueb.Draw();
		hit_close ? close2.Draw() : close.Draw();
	}

}

void ExitUI::Init()
{
#define scale 0.3f,0.3f
#define scale2 0.35f,0.35f

#define centerX 632
#define centerY 340

	exitb = GUISprite(centerX, centerY);
	exitb.Load(L"../Sprites/Menu/exit.png");
	exitb.SetScale(scale);

	exitb2 = GUISprite(centerX, centerY);
	exitb2.Load(L"../Sprites/Menu/exit2_r.png");
	exitb2.SetScale(scale2);


	continueb = GUISprite(centerX, centerY - 75.0f);
	continueb.Load(L"../Sprites/Menu/continue.png");
	continueb.SetScale(scale);

	continueb2 = GUISprite(centerX, centerY - 75.0f);
	continueb2.Load(L"../Sprites/Menu/continue_r.png");
	continueb2.SetScale(scale2);

	close = GUISprite(centerX + 110.0f, centerY - 125.0f);
	close.Load(L"../Sprites/Menu/close.png");
	close.SetScale(scale);

	close2 = GUISprite(centerX + 110.0f, centerY - 125.0f);
	close2.Load(L"../Sprites/Menu/close_r.png");
	close2.SetScale(scale2);


	menuBg = GUISprite(centerX, centerY);
	menuBg.Load(L"../Sprites/BackHudTransparent.png");
	menuBg.SetScale(0.7f, 0.7f);

	bg = GUISprite(centerX, centerY);
	bg.Load(L"../Sprites/BackHudTransparent.png");
	bg.SetScale(50, 50);

}

void ExitUI::Draw()
{
	HandleInputs();
	GUI::Begin();
	SpritePass();
	GUI::End();
}
