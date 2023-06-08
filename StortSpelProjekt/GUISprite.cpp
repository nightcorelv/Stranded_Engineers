#include "stdafx.h"
#include "GUISprite.h"
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include "ImGui/imgui.h"
#include "SettingsUI.h"
#include "Input.h"
#include "Time.h"

GUISprite::GUISprite()
{
    BaseWidth = GPU::windowWidth / 1264.0f;
    BaseHeight = GPU::windowHeight / 681.0f;

    currentTime = std::chrono::system_clock::now();
    lastUpdate = currentTime;
    deltaTime = ((std::chrono::duration<float>)(currentTime - lastUpdate)).count();
}

GUISprite::GUISprite(const float x, const float y, const float layer)
{
    BaseWidth = GPU::windowWidth / 1264.0f;
    BaseHeight = GPU::windowHeight / 681.0f;
    
    m_Position = DirectX::SimpleMath::Vector2(BaseWidth * x, BaseHeight * y);
    m_Layer = layer;
    m_Scale = { BaseWidth * 1, BaseWidth * 1 };            //same scale as object
    m_Tint = DirectX::Colors::White.v;  //.v - xmvextor should be able to store in it
    m_Alpha = 1.0f;
    m_Rotation = 0.0f;
}

GUISprite::GUISprite(const DirectX::SimpleMath::Vector2& position, float layer)
{
    BaseWidth = GPU::windowWidth / 1264.0f;
    BaseHeight = GPU::windowHeight / 681.0f;

    m_Position = position;
    m_Position.x *= BaseWidth;
    m_Position.y *= BaseHeight;
    m_Layer = layer;
    m_Scale = { BaseWidth * 1, BaseWidth * 1 };            //same scale as object
    m_Tint = DirectX::Colors::White.v;  //.v - xmvextor should be able to store in it
    m_Alpha = 1.0f;
    m_Rotation = 0.0f;

}

void GUISprite::Load(ID3D11Device* device, const wchar_t* file)
{
    //Import texture
    //DirectX::CreateDDSTextureFromFile(device, file, &m_pResource, &m_pTexture);
    DirectX::CreateWICTextureFromFile(device, file, &m_pResource, &m_pTexture);
    Utility::GetTextureDimentions(m_pResource.Get(), &m_Width, &m_Height);
    m_Origin = DirectX::SimpleMath::Vector2(m_Width / 2.0f, m_Height / 2.0f); //origin = center of texture also important to rotate as rotates around centrepoint
    m_SourceRect.left = 0;
    m_SourceRect.top = 0;
    m_SourceRect.right = m_Width;
    m_SourceRect.bottom = m_Height;

}

void GUISprite::Load(const wchar_t* file)
{
    //Import texture
    //DirectX::CreateDDSTextureFromFile(device, file, &m_pResource, &m_pTexture);
    DirectX::CreateWICTextureFromFile(GPU::device, file, &m_pResource, &m_pTexture);
    Utility::GetTextureDimentions(m_pResource.Get(), &m_Width, &m_Height);
    m_Origin = DirectX::SimpleMath::Vector2(m_Width / 2.0f, m_Height / 2.0f); //origin = center of texture also important to rotate as rotates around centrepoint
    m_SourceRect.left = 0;
    m_SourceRect.top = 0;
    m_SourceRect.right = m_Width;
    m_SourceRect.bottom = m_Height;
}

const DirectX::SimpleMath::Vector2 GUISprite::GetPosition() const
{
    auto pos = m_Position;
    pos.x /= BaseWidth;
    pos.y /= BaseHeight;
    return pos;
}

const DirectX::SimpleMath::Vector2& GUISprite::GetOrigin() const
{
    return m_Origin;
}

const DirectX::SimpleMath::Vector2& GUISprite::GetScale() const
{
    auto s = m_Scale;
    s.x /= BaseWidth;
    s.y /= BaseHeight;
    return s;
}

const DirectX::SimpleMath::Color& GUISprite::GetTint() const
{
    return m_Tint;
}

const float GUISprite::GetAlpha() const
{
    return m_Alpha;
}

const float GUISprite::GetRotation() const
{
    return m_Rotation;
}

const float GUISprite::GetWidth() const
{
    return m_Width * m_Scale.x;
}

const float GUISprite::GetHeight() const
{
    return m_Height * m_Scale.y;
}

void GUISprite::SetPosition(const DirectX::SimpleMath::Vector2& position)
{
    m_Position = position;
    m_Position.x *= BaseWidth;
    m_Position.y *= BaseHeight;
}

void GUISprite::SetOrigin(const DirectX::SimpleMath::Vector2& origin)
{
    m_Origin = origin;
}

void GUISprite::SetScale(const DirectX::SimpleMath::Vector2& scale)
{
    m_Scale = scale;
    m_Scale.x *= BaseWidth;
    m_Scale.y *= BaseHeight;
}
void GUISprite::SetScale(const float x, const float y)
{
    m_Scale = { BaseWidth * x, BaseWidth * y };
}

void GUISprite::SetTint(const DirectX::SimpleMath::Color& color)
{
    m_Tint = color;
}

void GUISprite::SetAlpha(const float alpha)
{
    DirectX::SimpleMath::Color opacity = this->GetTint();
    opacity.w = alpha;
    this->SetTint(opacity);
}

void GUISprite::SetRotation(const float rotation)
{
    m_Rotation = rotation;
}

bool GUISprite::fadeIn()
{
    bool done = false;
    DirectX::SimpleMath::Color gg = this->GetTint();
    float delta = Time::DeltaTimeInSeconds(); 
    float constant = 1.5f;

    //countdown & alpha change
    time -= constant * delta;
    gg.w = time;

    if (gg.w <= 0)
    {
        done = true;
        gg.w = 0;
        this->SetTint(gg);
        time = resetTime;
    }
    else
    {
        this->SetTint(gg);
    }
    return done;
}

bool GUISprite::fadeOut()
{
    bool done = false;
    DirectX::SimpleMath::Color gg = this->GetTint();
    float delta = Time::DeltaTimeInSeconds();
    float constant = 0.05f;

    //countdown & alpha change
    time2 += constant/resetTime * delta;
    gg.w += time2;

    if (gg.w >= 1)
    {
        done = true;
        gg.w = 1;
        this->SetTint(gg);
        time2 = 0;
    }
    else
    {
        this->SetTint(gg);
    }
    return done;
}

void GUISprite::update()
{    
    if (toFadeIn)
    {
        if (!this->fadeIn())
        {
        }
        else
        {
            toFadeIn = false;
        }
    }
    else if (toFadeOut)
    {
        if (!this->fadeOut())
        {
        }
        else
        {
            toFadeOut = false;
        }
    }
}

void GUISprite::setFade(bool in_T_out_F)
{
    if (in_T_out_F)
    {
        toFadeIn = true;
    }
    else
    {
        toFadeOut = true;
    }
}

void GUISprite::setFadeTime(const float time)
{
    resetTime = time;
}


bool GUISprite::Move()
{
    //Updating deltatime
    lastUpdate = currentTime;
    currentTime = std::chrono::system_clock::now();
    deltaTime = ((std::chrono::duration<float>)(currentTime - lastUpdate)).count();
    timer += deltaTime;
    float timestep = 3.8f;


    //When timestep passed the "camera" (comic) moves
    if (timer > 500) timer = 0;
    if (timer >= timestep) step++, timer = 0;
    switch (step)
    {
    case 1:
        distance = DirectX::SimpleMath::Vector2(1020, 1530) - this->GetPosition();
        this->SetPosition(this->GetPosition() + distance * deltaTime * 6.f);
        this->SetScale(1,1);
        break;

    case 2:
        distance = DirectX::SimpleMath::Vector2(220, 1530) - this->GetPosition();
        this->SetPosition(this->GetPosition() + distance * deltaTime * 6.f);
        break;

    case 3:
        distance = DirectX::SimpleMath::Vector2(1020, 550) - this->GetPosition();
        this->SetPosition(this->GetPosition() + distance * deltaTime * 6.f);
        scaleDistance = DirectX::SimpleMath::Vector2(BaseWidth * 0.75, BaseWidth * 0.75) - m_Scale;
        m_Scale += scaleDistance * deltaTime * 5.f;
        break;

    case 4:
        distance = DirectX::SimpleMath::Vector2(220, 550) - this->GetPosition();
        this->SetPosition(this->GetPosition() + distance * deltaTime * 6.f);
        break;

    case 5:
        distance = DirectX::SimpleMath::Vector2(620, -300) - this->GetPosition();
        this->SetPosition(this->GetPosition() + distance * deltaTime * 6.f);
        scaleDistance = DirectX::SimpleMath::Vector2(BaseWidth * 0.6, BaseWidth * 0.6) - m_Scale;
        m_Scale += scaleDistance * deltaTime * 5.f;
        break;

    case 6:
        first = false;
        break;
    }

    return first;
}

void GUISprite::Draw()
{
    GUI::spriteBatch->Draw(m_pTexture.Get(), m_Position, &m_SourceRect, m_Tint, m_Rotation, m_Origin, m_Scale, DirectX::SpriteEffects_None, m_Layer);
}

bool GUISprite::IntersectMouse() const
{
    //static POINT mousePos;
    //if (GetCursorPos(&mousePos))
    //{
    //    if (ScreenToClient(GetActiveWindow()/*FindWindowW(L"Window Class", L"Projekt")*/, &mousePos))
    //    {
    //    }
    //}

    ImVec2 mousePos = ImGui::GetMousePos();
#define InsideX mousePos.x > m_Position.x/* * BaseWidth*/ - (m_Width * m_Scale.x / 2.0f)/* * BaseWidth*/ && mousePos.x < m_Position.x/* * BaseWidth*/ + (m_Width * m_Scale.x / 2.0f)/* * BaseWidth*/
#define InsideY mousePos.y > m_Position.y/* * BaseHeight*/ - (m_Height * m_Scale.y / 2.0f)/* * BaseHeight*/ && mousePos.y < m_Position.y/* * BaseHeight*/ + (m_Height * m_Scale.y / 2.0f)/* * BaseHeight*/
    return InsideX && InsideY;
}

bool GUISprite::IntersectSprite(const GUISprite & toCheck) const
{
    using namespace DirectX::SimpleMath;
    Vector2 otherPos = toCheck.GetPosition();
    Vector2 pos = this->GetPosition();
    Vector2 scl = this->GetScale();
    float hgt =this->GetHeight();
    float wdth = this->GetWidth();
#define InsideX otherPos.x > pos.x - (wdth * scl.x / 2.0f) && otherPos.x < pos.x + (wdth * scl.x / 2.0f)
#define InsideY otherPos.y > pos.y - (hgt * scl.y / 2.0f) && otherPos.y < pos.y + (hgt * scl.y / 2.0f)
    return InsideX && InsideY;
}