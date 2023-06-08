#pragma once
#include "GUIObject.h"

class GUISprite final :public GUIObject
{
private:
	
	float resetTime = 1.8f;
	float time = 1.f;
	float time2 = 0.f;
	bool toFadeIn = false;
	bool toFadeOut = false;
	virtual bool fadeIn();
	virtual bool fadeOut();

public:

	inline static float BaseWidth, BaseHeight;

	GUISprite();
	GUISprite(const float x, const float y, const float layer = 0.0f);
	GUISprite(const DirectX::SimpleMath::Vector2& position, float layer = 0.0f);

	//Load
	virtual void Load(ID3D11Device* device, const wchar_t* file) override;
	void Load(const wchar_t* file);

	//Get Functions
	virtual const DirectX::SimpleMath::Vector2 GetPosition() const;
	virtual const DirectX::SimpleMath::Vector2& GetOrigin() const;
	virtual const DirectX::SimpleMath::Vector2& GetScale() const;
	virtual const DirectX::SimpleMath::Color& GetTint() const;
	virtual const float GetAlpha() const;
	virtual const float GetRotation() const;
	virtual const float GetWidth() const;
	virtual const float GetHeight() const;

	//Set
	virtual void SetPosition(const DirectX::SimpleMath::Vector2& position);
	virtual void SetOrigin(const DirectX::SimpleMath::Vector2& origin);
	virtual void SetScale(const DirectX::SimpleMath::Vector2& scale);
	virtual void SetScale(const float x, const float y);
	virtual void SetTint(const DirectX::SimpleMath::Color& color);
	virtual void SetAlpha(const float alpha);
	virtual void SetRotation(const float rotation);

	//FadeIn/out
	virtual void update();
	virtual void setFade(bool in_T_out_F);
	virtual void setFadeTime(const float time);

	//Render
	bool Move();
	virtual void Draw();
	bool IntersectSprite(const GUISprite& toCheck) const;
	bool IntersectMouse() const;

	//For comic book
	bool first = true;
	int step = 1;
	DirectX::SimpleMath::Vector2 distance;
	DirectX::SimpleMath::Vector2 scaleDistance;
	float endTimer;
	float deltaTime;
	float timer = 0;
	std::chrono::time_point<std::chrono::system_clock> currentTime;
	std::chrono::time_point<std::chrono::system_clock> lastUpdate;

protected:

	Microsoft::WRL::ComPtr<ID3D11Resource> m_pResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;
	UINT m_Width;
	UINT m_Height;
	RECT m_SourceRect;

	DirectX::SimpleMath::Vector2  m_Position;

	DirectX::SimpleMath::Vector2  m_Origin;
	DirectX::SimpleMath::Vector2  m_Scale;
	DirectX::SimpleMath::Color	 m_Tint;
	float    m_Alpha;
	float    m_Rotation;
	float	 m_Layer;

};
