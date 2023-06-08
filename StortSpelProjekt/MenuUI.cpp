#include "stdafx.h"
#include "MenuUI.h"
#include "GPU.h"
#include "Input.h"
#include "SettingsUI.h"
#include "Credits.h"
#include "SoundLibrary.h"
#include "Time.h"
#include "Game.h"

using namespace DirectX::SimpleMath;

void MenuUI::SpritePass()
{
	if (isLoading)
	{
		Loading.Draw();
		return;
	}
	
	hit_start ? start2.Draw() : start.Draw();
	hit_setting? settings2.Draw() :settings.Draw();
	hit_credits ? credits2.Draw() : credits.Draw();
	hit_exit ? exit2.Draw() : exit.Draw();
	hit_control ? control2.Draw() : control.Draw();

	hit_offline ? offline2.Draw() : offline.Draw();
	Game::IFONLINE ? checkbox_false.Draw() : checkbox_true.Draw();

	title.Draw();
	comic.Draw();

	//Update fade
	title.update();
	comic.update();
	start.update();
	settings.update();
	credits.update();
	exit.update();
	control.update();
	start2.update();
	settings2.update();
	credits2.update();
	exit2.update();
	control2.update();
	checkbox_false.update();
	checkbox_true.update();
	offline2.update();
	offline.update();

	//Animate comic
	if (firstTime)
	{	
		firstTime = comic.Move();
	}
	
	//set up automatic fade in
	if (!firstTime && !fadedIn || Input::anykey && !fadedIn || controllerPress && !fadedIn || Input::KeyDown(KeyCode::MOUSE_L) && !fadedIn || Input::KeyDown(KeyCode::MOUSE_R) && !fadedIn)
	{
		comic.setFade(true);
		fadedIn = true;
		title.SetAlpha(0);
		start.SetAlpha(0);
		settings.SetAlpha(0);
		credits.SetAlpha(0);
		exit.SetAlpha(0);
		control.SetAlpha(0);
		start2.SetAlpha(0);
		settings2.SetAlpha(0);
		credits2.SetAlpha(0);
		exit2.SetAlpha(0);
		control2.SetAlpha(0);
		checkbox_false.SetAlpha(0);
		checkbox_true.SetAlpha(0);
		offline2.SetAlpha(0);
		offline.SetAlpha(0);
	}

	//Fade in title screen manual
	if (Input::anykey && fadedIn || controllerPress && fadedIn || Input::KeyDown(KeyCode::MOUSE_L) && fadedIn || Input::KeyDown(KeyCode::MOUSE_R) && fadedIn)
	{
		title.SetAlpha(1);
		start.SetAlpha(1);
		settings.SetAlpha(1);
		credits.SetAlpha(1);
		exit.SetAlpha(1);
		control.SetAlpha(1);
		start2.SetAlpha(1);
		settings2.SetAlpha(1);
		credits2.SetAlpha(1);
		exit2.SetAlpha(1);
		control2.SetAlpha(1);
		checkbox_false.SetAlpha(1);
		checkbox_true.SetAlpha(1);
		offline2.SetAlpha(1);
		offline.SetAlpha(1);
	}

	//Fade in title screen automatic
	if ((int)timer == 3)
	{
		title.setFade(false);
	}
	if ((int)timer == 4)
	{
		title.setFade(false);
		start.setFade(false);
		settings.setFade(false);
		credits.setFade(false);
		exit.setFade(false);
		control.setFade(false);
		start2.setFade(false);
		settings2.setFade(false);
		credits2.setFade(false);
		exit2.setFade(false);
		control2.setFade(false);
		checkbox_false.setFade(false);
		checkbox_true.setFade(false);
		offline2.setFade(false);
		offline.setFade(false);
	}

	//Restarts title loop
	if ((int)timer == 60)
	{
	firstTime = true;
	fadedIn = false;
	comic.setFade(false);
	comic.first = true;
	comic.timer = 0;
	comic.step = 0;
	comic.deltaTime = 0;
	timer = 0;
	}

}

void MenuUI::HandleInputs()
{
	if (fadedIn)
	{
		timer += Time::DeltaTimeInSeconds();
	}

	
	controllerPress = false;
	auto state = gamePad->GetState(0);

	if (state.IsAPressed() || state.IsBPressed() || state.IsXPressed() || state.IsYPressed() || state.IsStartPressed() || state.IsMenuPressed()) controllerPress = true;

	if (fadedIn)
	{
		// any gamepad connected
		if (state.IsConnected())
		{
			//back to default value
			if (selectIndex == 6)
			{
				selectIndex = 0;
			}

			// reset all to false
			hit_start = hit_setting = hit_credits = hit_exit = hit_control = false;

			static bool downTrigged = false;
			static bool upTrigged = false;

			// xbox r or l wheel down
			if (state.IsDPadDownPressed() || state.IsLeftThumbStickDown())
			{
				if (!downTrigged)
				{
					switch (selectIndex)
					{
					case 0:
						selectIndex = 1;
						break;
					case 1:
						selectIndex = 2;
						break;
					case 2:
						selectIndex = 3;
						break;
					case 3:
						selectIndex = 4;
						break;
					}
					downTrigged = true;
				}

			}
			else
			{
				downTrigged = false;
			}

			if (state.IsDPadUpPressed() || state.IsLeftThumbStickUp())
			{
				if (!upTrigged)
				{
					switch (selectIndex)
					{
					case 1:
						selectIndex = 0;
						break;
					case 2:
						selectIndex = 1;
						break;
					case 3:
						selectIndex = 2;
						break;
					case 4:
						selectIndex = 3;
						break;
					}
					upTrigged = true;
				}

			}
			else
			{
				upTrigged = false;
			}

			switch (selectIndex)
			{
			case 0:
				hit_start = true;
				break;
			case 1:
				hit_control = true;
				break;
			case 2:
				hit_setting = true;
				break;
			case 3:
				hit_credits = true;
				break;
			case 4:
				hit_exit = true;
				break;
			}
		}
		//else mouse
		else
		{
			// reset all to false
			hit_start = hit_setting = hit_credits = hit_exit = hit_control = false;

			static bool downTrigged = false;
			static bool upTrigged = false;
			bool mouseIntersection = true;
			static int lastSelectIndex = 0;
			selectIndex = 6;

			if (start.IntersectMouse())
			{
				hit_start = true;
				lastSelectIndex = 0;
			}
			else if (control.IntersectMouse())
			{
				hit_control = true;
				lastSelectIndex = 1;
			}
			else if (settings.IntersectMouse())
			{
				hit_setting = true;
				lastSelectIndex = 2;
			}
			else if (credits.IntersectMouse())
			{
				hit_credits = true;
				lastSelectIndex = 3;
			}
			else if (exit.IntersectMouse())
			{
				hit_exit = true;
				lastSelectIndex = 4;
			}
			else
			{
				mouseIntersection = false;
			}


			if (!mouseIntersection)
			{
				// W or UP
				if (Input::KeyPress(KeyCode::S) || Input::KeyPress(KeyCode::ARROW_Down))
				{
					if (selectIndex == 6)
					{
						selectIndex = lastSelectIndex;
					}
					if (!downTrigged)
					{
						switch (selectIndex)
						{
						case 0:
							selectIndex = 1;
							break;
						case 1:
							selectIndex = 2;
							break;
						case 2:
							selectIndex = 3;
							break;
						case 3:
							selectIndex = 4;
							break;
						}
						downTrigged = true;
					}

				}
				else
				{
					downTrigged = false;
				}

				if (Input::KeyPress(KeyCode::W) || Input::KeyPress(KeyCode::ARROW_Up))
				{
					if (selectIndex == 6)
					{
						selectIndex = lastSelectIndex;
					}
					if (!upTrigged)
					{
						switch (selectIndex)
						{
						case 1:
							selectIndex = 0;
							break;
						case 2:
							selectIndex = 1;
							break;
						case 3:
							selectIndex = 2;
							break;
						case 4:
							selectIndex = 3;
							break;
						}
						upTrigged = true;
					}

				}
				else
				{
					upTrigged = false;
				}

				if (lastSelectIndex != selectIndex && selectIndex != 6)
				{
					//std::cout << "changed Indx: " << std::endl;
					lastSelectIndex = selectIndex;
				}

				//std::cout << "lastIndx: " << lastSelectIndex << std::endl;
				switch (lastSelectIndex)
				{
				case 0:
					hit_start = true;
					break;
				case 1:
					hit_control = true;
					break;
				case 2:
					hit_setting = true;
					break;
				case 3:
					hit_credits = true;
					break;
				case 4:
					hit_exit = true;
					break;
				}
			}
	}
		
	}



	if (Input::KeyPress(KeyCode::MOUSE_L) || state.IsAPressed() || Input::KeyPress(KeyCode::ENTER)|| (Input::KeyPress(KeyCode::ENTER_KeyPad)))
	{
		if (Input::KeyPress(KeyCode::MOUSE_L))
		{
			if (start.IntersectMouse() || control.IntersectMouse() || settings.IntersectMouse() || credits.IntersectMouse() || exit.IntersectMouse())
			{
				if (hit_start)
				{
					SoundLibrary::menuMusic.stop();
					SoundLibrary::clickSfx.stop();
					SoundLibrary::clickSfx.play();

					Game::IFONLINE ? gameState = INPUTIP : gameState = GAME;
					

					isLoading = true;
				}
				else if (hit_setting)
				{
					SoundLibrary::clickSfx.stop();
					SoundLibrary::clickSfx.play();
					gameState = SETTINGS;
				}
				else if (hit_credits)
				{
					SoundLibrary::clickSfx.stop();
					SoundLibrary::clickSfx.play();
					gameState = CREDITS;
				}
				else if (hit_exit)
				{
					SoundLibrary::menuMusic.stop();
					SoundLibrary::clickSfx.stop();
					SoundLibrary::clickSfx.play();
					gameState = EXIT;
				}
				else if (hit_control)
				{
					SoundLibrary::clickSfx.stop();
					SoundLibrary::clickSfx.play();
					gameState = CONTROL;
				}
			}
		}
		else
		{
			if (hit_start)
			{
				SoundLibrary::menuMusic.stop();
				SoundLibrary::clickSfx.stop();
				SoundLibrary::clickSfx.play();

				Game::IFONLINE ? gameState = INPUTIP : gameState = GAME;

				isLoading = true;
			}
			else if (hit_setting)
			{
				SoundLibrary::clickSfx.stop();
				SoundLibrary::clickSfx.play();
				gameState = SETTINGS;
			}
			else if (hit_credits)
			{
				SoundLibrary::clickSfx.stop();
				SoundLibrary::clickSfx.play();
				gameState = CREDITS;
			}
			else if (hit_exit)
			{
				SoundLibrary::menuMusic.stop();
				SoundLibrary::clickSfx.stop();
				SoundLibrary::clickSfx.play();
				gameState = EXIT;
			}
			else if (hit_control)
			{
				SoundLibrary::clickSfx.stop();
				SoundLibrary::clickSfx.play();
				gameState = CONTROL;
			}
		}
	}

	hit_offline = offline.IntersectMouse();
	if (hit_offline && Input::KeyPress(KeyCode::MOUSE_L))
	{
		Game::IFONLINE = !Game::IFONLINE;
	}

}

GAMESTATE MenuUI::GetGameState()
{
	return gameState;
}

MenuUI::MenuUI()
{
	GUI::Init();

#define upp 60
#define left 80

#define centerX 632
#define centerY 340
#define scale 0.3f,0.3f
#define scale2 0.35f,0.35f

	gamePad = std::make_unique<DirectX::GamePad>();
	start = GUISprite(centerX, 300);
	start.Load(L"../Sprites/Menu/start.png");
	start.SetScale(scale);

	comic = GUISprite(1020, 1530);
	comic.Load(L"../Sprites/Menu/Comic.jpeg");
	comic.SetScale(0.96, 0.96);

	control = GUISprite(centerX, 300 + 75 * 1);
	control.Load(L"../Sprites/Menu/control.png");
	control.SetScale(scale);

	settings = GUISprite(centerX, 300 + 75 * 2);
	settings.Load(L"../Sprites/Menu/settings.png");
	settings.SetScale(scale);

	credits = GUISprite(centerX, 300 + 75 * 3);
	credits.Load(L"../Sprites/Menu/credits.png");
	credits.SetScale(scale);

	exit = GUISprite(centerX, 300 + 75 * 4);
	exit.Load(L"../Sprites/Menu/exit.png");
	exit.SetScale(scale);

	start2 = GUISprite(centerX, 300);
	start2.Load(L"../Sprites/Menu/start2_r.png");
	start2.SetScale(scale2);

	control2 = GUISprite(centerX, 300 + 75 * 1);
	control2.Load(L"../Sprites/Menu/control2_r.png");
	control2.SetScale(scale2);

	settings2 = GUISprite(centerX, 300 + 75 * 2);
	settings2.Load(L"../Sprites/Menu/settings2_r.png");
	settings2.SetScale(scale2);

	credits2 = GUISprite(centerX, 300 + 75 * 3);
	credits2.Load(L"../Sprites/Menu/credits2_r.png");
	credits2.SetScale(scale2);

	exit2 = GUISprite(centerX, 300 + 75 * 4);
	exit2.Load(L"../Sprites/Menu/exit2_r.png");
	exit2.SetScale(scale2);

	Loading = GUISprite(centerX, centerY);
	Loading.Load(L"../Sprites/Loding.png");
	Loading.SetScale(1, 1);

	title = GUISprite(centerX, 340-200);
	title.Load(L"../Sprites/title2.png");
	title.SetScale(0.6f, 0.6f);

	title = GUISprite(centerX, 340 - 200);
	title.Load(L"../Sprites/title2.png");
	title.SetScale(0.6f, 0.6f);

	offline = GUISprite(centerX + 500, centerY + 300);
	offline.Load(L"../Sprites/menu/offline.png");
	offline.SetScale(scale);

	offline2 = GUISprite(centerX + 500, centerY + 300);
	offline2.Load(L"../Sprites/menu/offline2.png");
	offline2.SetScale(scale);

	checkbox_false = GUISprite(centerX - 150 + 500, centerY + 300);
	checkbox_false.Load(GPU::device, L"../Sprites/box_unchecked2.png");
	checkbox_false.SetScale(0.75f, 0.75f);

	checkbox_true = GUISprite(centerX - 150 + 500, centerY + 300);
	checkbox_true.Load(GPU::device, L"../Sprites/box_checked2.png");
	checkbox_true.SetScale(0.75f, 0.75f);

}

void MenuUI::Draw()
{
	Input::Update();
	HandleInputs();
	GUI::Begin();
	SpritePass();
	GUI::End();
}
