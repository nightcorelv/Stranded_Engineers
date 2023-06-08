#pragma once

class ExitUI final
{

	inline static GUISprite bg;
	inline static GUISprite menuBg;

	inline static GUISprite exitb;
	inline static GUISprite exitb2;

	inline static GUISprite continueb;
	inline static GUISprite continueb2;

	inline static GUISprite close;
	inline static GUISprite close2;

	static void HandleInputs();
	static void SpritePass();

	inline static bool hit_exit = false;
	inline static bool hit_continue = false;
	inline static bool hit_close = false;

public:

	inline static bool show = false;
	inline static bool exit = false;

	static void Init();
	static void Draw();
};