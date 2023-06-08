#include "stdafx.h"
#include "LandingHud.h"
#include "Input.h"

LandingHud::LandingHud()
	:factor(1.0f)
{
	landing0 = GUISprite(DirectX::SimpleMath::Vector2(125, 320)); //bar
	landing0.Load(L"../Sprites/Bar.png");
	landing0.SetScale(0.51f, 0.51f);

	landing1 = GUISprite(125, (MinVec.y - MaxVec.y) / 2); //safebox
	landing1.Load(L"../Sprites/safeBox.png");
	landing1.SetScale(0.2f, 0.06f);

	landing2 = GUISprite(125, (MinVec.y - MaxVec.y) / 2); //ship
	landing2.Load(L"../Sprites/ship.png");
	landing2.SetScale(0.2f, 0.2f);

	wheel_L = GUISprite(225, 320);
	wheel_L.Load(L"../Sprites/control/L_wheel.png");
	wheel_L.SetScale(0.4f, 0.4f);

	arrow = GUISprite(225, 430);
	arrow.Load(L"../Sprites/arrow_g.png");
	arrow.SetScale(0.4f, 0.4f);
	arrow.SetRotation(3.1415926f);

	arrow2 = GUISprite(225, 210);
	arrow2.Load(L"../Sprites/arrow_o.png");
	arrow2.SetScale(0.4f, 0.4f);

	Skey = GUISprite(225, 360);
	Skey.Load(L"../Sprites/Skey.png");
	Skey.SetScale(0.4f, 0.4f);

	Wkey = GUISprite(225, 280);
	Wkey.Load(L"../Sprites/Wkey.png");
	Wkey.SetScale(0.4f, 0.4f);

#define nrScale 0.8f
	n0 = GUISprite(0, 340 - 0);
	n0.Load(L"../Sprites/Numbers/0.png");
	n0.SetScale(nrScale, nrScale);
	n0.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n1 = GUISprite(0, 340 - 0);
	n1.Load(L"../Sprites/Numbers/1.png");
	n1.SetScale(nrScale, nrScale);
	n1.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n2 = GUISprite(0, 340 - 0);
	n2.Load(L"../Sprites/Numbers/2.png");
	n2.SetScale(nrScale, nrScale);
	n2.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n3 = GUISprite(0, 340 - 0);
	n3.Load(L"../Sprites/Numbers/3.png");
	n3.SetScale(nrScale, nrScale);
	n3.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n4 = GUISprite(0, 340 - 0);
	n4.Load(L"../Sprites/Numbers/4.png");
	n4.SetScale(nrScale, nrScale);
	n4.SetTint(DirectX::SimpleMath::Color(0,1,0));

	n5 = GUISprite(0, 340 - 0);
	n5.Load(L"../Sprites/Numbers/5.png");
	n5.SetScale(nrScale, nrScale);
	n5.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n6 = GUISprite(0, 340 - 0);
	n6.Load(L"../Sprites/Numbers/6.png");
	n6.SetScale(nrScale, nrScale);
	n6.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n7 = GUISprite(0, 340 - 0);
	n7.Load(L"../Sprites/Numbers/7.png");
	n7.SetScale(nrScale, nrScale);
	n7.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n8 = GUISprite(0, 340 - 0);
	n8.Load(L"../Sprites/Numbers/8.png");
	n8.SetScale(nrScale, nrScale);
	n8.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	n9 = GUISprite(0, 340 - 0);
	n9.Load(L"../Sprites/Numbers/9.png");
	n9.SetScale(nrScale, nrScale);
	n9.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

	this->targetTime = 0;
	this->currentTime = 0;
}

LandingHud::~LandingHud()
{
}

void LandingHud::setLandingScore(int score)
{
	setScore = true;
	this->ggop = score;
}

void LandingHud::makeGamePad(DirectX::GamePad* gamePad)
{
	this->gamePad = gamePad;
}

bool LandingHud::handleInputs(const float& dt)
{
	bool intersected = false;

	this->state = gamePad->GetState(0);
	tracker.Update(state);
	
	if (this->currentTime >= this->targetTime)
	{
		this->targetTime = factor * (rand() % 21)*0.1f + 0.75f;
		this->currentTime = 0;

		float realTemp = (float)(rand() % 360) * (DirectX::XM_PI / 180.f);
		float sinCurve = (sin(realTemp) * 0.5f) + 0.5f;
		DirectX::SimpleMath::Vector2 targetPos;
		targetPos.y = MaxVec.y + sinCurve * (MinVec.y - MaxVec.y);

		realTemp = (float)(rand() % 360) * (DirectX::XM_PI / 180.f);
		DirectX::SimpleMath::Vector2 targetScale;
		targetScale.y = abs(sin(realTemp)) * 0.15f + 0.1f;

		this->changePos = (targetPos - this->landing1.GetPosition())/this->targetTime;
		this->changeScale = (targetScale - this->landing1.GetScale()) / this->targetTime;
	}

	this->currentTime += dt;

	float speed = 300.f;

	DirectX::SimpleMath::Vector2 rocketPos = landing2.GetPosition();
	DirectX::SimpleMath::Vector2 safePos = landing1.GetPosition();
	
	//Move "safezone" sprite
	landing1.SetPosition(DirectX::SimpleMath::Vector2(MaxVec.x, landing1.GetPosition().y + changePos.y * dt));
	landing1.SetScale(DirectX::SimpleMath::Vector2(0.2f, landing1.GetScale().y + changeScale.y * dt));

	if (state.IsConnected())
	{
		show_gamepad = true;
		if (abs(state.thumbSticks.leftY) > 0)
		{
			DirectX::SimpleMath::Vector2 test(rocketPos.x, rocketPos.y - dt * speed * state.thumbSticks.leftY);

			if (rocketPos.y < MaxVec.y - 25)
			{
				landing2.SetPosition(DirectX::SimpleMath::Vector2(MaxVec.x, rocketPos.y));
			}
			else
			{
				landing2.SetPosition(test);
			}
		}
	}
	else
	{
		show_gamepad = false;
		//Move player sprite
		if (Input::KeyDown(KeyCode::W) || Input::KeyDown(KeyCode::ARROW_Up))
		{
			DirectX::SimpleMath::Vector2 test(rocketPos.x, rocketPos.y - dt * speed);

			if (rocketPos.y < MaxVec.y - 25)
			{
				landing2.SetPosition(DirectX::SimpleMath::Vector2(MaxVec.x, rocketPos.y));
			}
			else
			{
				landing2.SetPosition(test);
			}
		}
		if (Input::KeyDown(KeyCode::S) || Input::KeyDown(KeyCode::ARROW_Down))
		{
			DirectX::SimpleMath::Vector2 test(rocketPos.x, rocketPos.y + dt * speed);

			if (rocketPos.y > MinVec.y + 25)
			{
				landing2.SetPosition(DirectX::SimpleMath::Vector2(MaxVec.x, rocketPos.y));
			}
			else
			{
				landing2.SetPosition(test);
			}
		}
	}


	//Check sprite intersection
	if (landing1.IntersectSprite(landing2))
	{
		//std::cout << "intersects" << std::endl;
		intersected = true;
	}

	return intersected;
}

void LandingHud::draw()
{
	GUI::Begin();
	
	landing0.Draw();
	landing1.Draw();
	landing2.Draw();

	if (show_gamepad)
	{
		wheel_L.Draw();
		arrow.Draw();
		arrow2.Draw();
	}
	else
	{
		Skey.Draw();
		Wkey.Draw();
		arrow.Draw();
		arrow2.Draw();
	}
	static bool changed = false;
	std::string number = std::to_string(ggop);
	
	for (int i = 0; i < number.size(); i++)
	{
		static float textSpace = 22.0f;
		#define posY1 100
		#define posX1 210 + (i * textSpace)
		float constant = 0.10f;
		if (ggop % 10 == 0 && setScore && ggop != 0 && !changed)
		{
			textSpace++;
			n0.SetScale(n0.GetScale().x + constant, n0.GetScale().y + constant);
			n1.SetScale(n1.GetScale().x + constant, n1.GetScale().y + constant);
			n2.SetScale(n2.GetScale().x + constant, n2.GetScale().y + constant);
			n3.SetScale(n3.GetScale().x + constant, n3.GetScale().y + constant);
			n4.SetScale(n4.GetScale().x + constant, n4.GetScale().y + constant);
			n5.SetScale(n5.GetScale().x + constant, n5.GetScale().y + constant);
			n6.SetScale(n6.GetScale().x + constant, n6.GetScale().y + constant);
			n7.SetScale(n7.GetScale().x + constant, n7.GetScale().y + constant);
			n8.SetScale(n8.GetScale().x + constant, n8.GetScale().y + constant);
			n9.SetScale(n9.GetScale().x + constant, n9.GetScale().y + constant);
			changed = true;
		}
		if (ggop % 11 == 0 && setScore && changed)
		{
			n0.SetScale(n0.GetScale().x - 0.1f, n0.GetScale().y - 0.1f);
			n1.SetScale(n1.GetScale().x - 0.1f, n1.GetScale().y - 0.1f);
			n2.SetScale(n2.GetScale().x - 0.1f, n2.GetScale().y - 0.1f);
			n3.SetScale(n3.GetScale().x - 0.1f, n3.GetScale().y - 0.1f);
			n4.SetScale(n4.GetScale().x - 0.1f, n4.GetScale().y - 0.1f);
			n5.SetScale(n5.GetScale().x - 0.1f, n5.GetScale().y - 0.1f);
			n6.SetScale(n6.GetScale().x - 0.1f, n6.GetScale().y - 0.1f);
			n7.SetScale(n7.GetScale().x - 0.1f, n7.GetScale().y - 0.1f);
			n8.SetScale(n8.GetScale().x - 0.1f, n8.GetScale().y - 0.1f);
			n9.SetScale(n9.GetScale().x - 0.1f, n9.GetScale().y - 0.1f);
			setScore = false;
			changed = false;
		}
		switch (number.at(i))
		{

		case '0':
		{
			n0.SetPosition({ posX1, posY1 });
			n0.Draw();

		}break;

		case '1':
		{
			n1.SetPosition({ posX1, posY1 });
			n1.Draw();
		}break;

		case '2':
		{
			n2.SetPosition({ posX1, posY1 });
			n2.Draw();
		}break;

		case '3':
		{
			n3.SetPosition({ posX1, posY1 });
			n3.Draw();
		}break;

		case '4':
		{
			n4.SetPosition({ posX1, posY1 });
			n4.Draw();
		}break;

		case '5':
		{
			n5.SetPosition({ posX1, posY1 });
			n5.Draw();
		}break;

		case '6':
		{
			n6.SetPosition({ posX1, posY1 });
			n6.Draw();
		}break;

		case '7':
		{
			n7.SetPosition({ posX1, posY1 });
			n7.Draw();
		}break;

		case '8':
		{
			n8.SetPosition({ posX1, posY1 });
			n8.Draw();
		}break;

		case '9':
		{
			n9.SetPosition({ posX1, posY1 });
			n9.Draw();
		}break;

		}

	}
	

	GUI::End();
}

void LandingHud::drawPointsForOtherGameModes(const int scores[])
{
	
	GUI::Begin();

	for (int s = 0; s < 2; s++)
	{

		std::string number = std::to_string(scores[s]);
		std::cout << "Blue team score: " << scores[s] << "\n";
		for (int i = 0; i < number.size(); i++)
		{
			static float textSpace = 22.0f;
			float posY;
			if (s == 0) posY = 100;
			else posY = 150;
			float posX = 50 + (i * textSpace);
			/*switch (number.at(i))
			{

			case '0':
			{
				n0.SetPosition({ posX, posY });
				if (s == 1) n0.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n0.SetTint(DirectX::SimpleMath::Color(1, 69.f/255.f, 0));
				n0.Draw();
				n0.SetTint(DirectX::SimpleMath::Color(0, 1, 0));


			}break;

			case '1':
			{
				n1.SetPosition({ posX, posY });
				if (s == 1) n1.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n1.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n1.Draw();
				n1.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '2':
			{
				n2.SetPosition({ posX, posY });
				if (s == 1) n2.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n2.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n2.Draw();
				n2.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '3':
			{
				n3.SetPosition({ posX, posY });
				if (s == 1) n3.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n3.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n3.Draw();
				n3.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '4':
			{
				n4.SetPosition({ posX, posY });
				if (s == 1) n4.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n4.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n4.Draw();
				n4.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '5':
			{
				n5.SetPosition({ posX, posY });
				if (s == 1) n5.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n5.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n5.Draw();
				n5.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '6':
			{
				n6.SetPosition({ posX, posY });
				if (s == 1) n6.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n6.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n6.Draw();
				n6.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '7':
			{
				n7.SetPosition({ posX, posY });
				if (s == 1) n7.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n7.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n7.Draw();
				n7.SetTint(DirectX::SimpleMath::Color(0, 1, 0));

			}break;

			case '8':
			{
				n8.SetPosition({ posX, posY });
				if (s == 1) n8.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n8.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n8.Draw();
				n8.SetTint(DirectX::SimpleMath::Color(0, 1, 0));
			}break;

			case '9':
			{
				n9.SetPosition({ posX, posY });
				if (s == 1) n9.SetTint(DirectX::SimpleMath::Color(0, 0, 1));
				else n9.SetTint(DirectX::SimpleMath::Color(1, 69.f / 255.f, 0));
				n9.Draw();
				n9.SetTint(DirectX::SimpleMath::Color(0, 1, 0));
			}break;

			}*/
		}
	}

	GUI::End();
}