#include "pch.h"
#include "KeyCollectable.h"
#include "Texture.h"
#include "utils.h"
#include "Door.h"

KeyCollectable::KeyCollectable(Door* doorPtr) :
	Collectable(Vector2f{ 7300.f , 200.f}, 1000),
	m_pDoor{doorPtr},
	m_pTexture{ new Texture("Collectables/key.png") },
	m_FrameTime{ 0.15f },
	m_AccTime{},
	m_CurrentFrameNr{ 0 },
	m_FRAME_WIDTH{ m_pTexture->GetWidth() / 3 },
	m_FRAME_HEIGHT{ m_pTexture->GetHeight() },
	m_SrcRectX{ 0 },
	m_IsCollected{ false },
	m_BossIsDead{false},
	m_VelocityY{}
{
}

KeyCollectable::~KeyCollectable() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}


void KeyCollectable::Draw() const
{
	Rectf dst = Rectf{ m_Position.x, m_Position.y, m_pTexture->GetWidth() / 3 * 2, m_pTexture->GetHeight() * 2 };;
	Rectf src = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 3,  m_pTexture->GetHeight(), m_pTexture->GetWidth() / 3 , m_pTexture->GetHeight() };;

	m_pTexture->Draw(dst, src);

	/*utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });
	utils::DrawRect(m_CollisionRect);*/
}

void KeyCollectable::Update(float elapsedSec)
{
	FallFromSky(elapsedSec);
	SetCollisionRect();
	UpdateCurrFrameNumber(elapsedSec);
}

void KeyCollectable::FallFromSky(float elapsedSec) noexcept
{
	const float GRAVITY{ -100.f };

	if (m_Position.y > 75.f)
	{
		m_VelocityY += GRAVITY * elapsedSec;
		m_Position.y += m_VelocityY * elapsedSec;
	}
	else
	{
		m_Position.y = 75.f;
	}
}

void KeyCollectable::UpdateCurrFrameNumber(float elapsedSec) noexcept
{
	m_AccTime += elapsedSec;

	if (m_AccTime >= m_FrameTime)
	{
		m_AccTime = 0;
		m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 3;
	}
}

const Rectf& KeyCollectable::GetCollisionRect() const
{
	return m_CollisionRect;
}

void KeyCollectable::OpenDoor()
{
	m_pDoor->OpenDoor();
}

void KeyCollectable::SetCollisionRect() noexcept
{
	m_CollisionRect = Rectf{ m_Position.x, m_Position.y - 1.f, m_pTexture->GetWidth() / 3 * 2, m_pTexture->GetHeight() * 2 };

}
