
#include "GUISprite.h"
#include "GUIText.h"
#include "Player.h"
#include "TimeStruct.h"
class SpaceShip;

class HudUI final
{
	GUISprite fade;

	GUISprite redTeam0, redTeam1, redTeam2, redTeam3, redTeam4;
	GUISprite bar_b, bar_e, bar_p;
	GUISprite bar_b2, bar_e2, bar_p2;
	GUISprite blueTeam0,blueTeam1,blueTeam2,blueTeam3,blueTeam4;

	GUISprite ready, set, go; 
	GUISprite dot1, dot2, exclamation;

	GUISprite run_button, use_button, throws_button, center_bkground, run_text, pickUse_text, throw_text;
	GUISprite run_button2, use_button2, throws_button2, center_bkground2;


	GUISprite control, control2;
	GUISprite useText;
	GUISprite throwText;
	GUISprite pickText;
	GUISprite objective;
	GUISprite blackBackground;
	GUISprite kthObjective, kthObjective2;
	GUISprite objective2;
	GUISprite blackBackground2;
	float objectiveDisplayTime = 5.0f;

	GUISprite controls, controls2;
	GUISprite useControls;
	GUISprite useControls1;
	GUISprite useControls2;

	TimeStruct rocketTimer;

	float scaleTimer;
	bool changedSprite = false;
	void SpritePass();
	bool Bready = false;
	bool Bset = false;
	bool Bgo = false;
	bool Bdot1 = false;
	bool Bdot2 = false;
	bool handle = false;
	bool done = false;
	float pointRed = 0, pointBlue = 0;

	inline static DirectX::GamePad* gamepad = nullptr;
	inline static DirectX::GamePad::State state;
	float readyTimer = 0.0f;
public:
	bool kth = false;
	float count = 1.0f;
	inline static SpaceShip* red = nullptr;
	inline static SpaceShip* blue = nullptr;
	inline static Player* player = nullptr;

	HudUI();
	~HudUI();
	void handleInputs();
	bool fadeIn();
	void displayObjective();
	void fadeOut();
	void moveSprite();
	bool readySetGo();
	void setOpacity(bool onOff);
	static void SetGamePad(DirectX::GamePad* gamepad);
	void DrawFade(float teamScoreLandingMiniGame = 0, float enemyTeamScoreLandingMiniGame = 0);
	void resetReadySetGo();
	void Draw();
	void setKTH(bool stuff);
	bool isDone() const;
	void setPointRed(const float point);
	void setPointBlue(const float point);
};