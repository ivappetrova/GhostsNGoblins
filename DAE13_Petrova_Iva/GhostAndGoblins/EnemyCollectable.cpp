#include "pch.h"
#include "EnemyCollectable.h"
#include "Texture.h"
#include "utils.h"

EnemyCollectable::EnemyCollectable(Vector2f position) :
	Collectable(position, 200),
	m_FrameTime{ 0.15f },
	m_AccTime{},
	m_CurrentFrameNr{ 0 },
	m_SrcRectX{ 0 },
	m_IsCollected{ false },
	m_BossIsDead{ false },
	m_VelocityY{}
{
	int randomCollectable = rand() % 2 + 1;

	if (randomCollectable == 1)
	{
		m_pTexture= new Texture("Collectables/doll.png");
	}
	else
	{
		m_pTexture = new Texture("Collectables/necklace.png");
	}
}

EnemyCollectable::~EnemyCollectable() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}


void EnemyCollectable::Draw() const
{
	Rectf dst = Rectf{ m_Position.x, m_Position.y, m_pTexture->GetWidth() / 3 * 2, m_pTexture->GetHeight() * 2 };;
	Rectf src = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 3,  m_pTexture->GetHeight(), m_pTexture->GetWidth() / 3 , m_pTexture->GetHeight() };;

	m_pTexture->Draw(dst, src);

	/*utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });
	utils::DrawRect(m_CollisionRect);*/
}

void EnemyCollectable::Update(float elapsedSec)
{
	FallFromSky(elapsedSec);
	SetCollisionRect();
	UpdateCurrFrameNumber(elapsedSec);
}

void EnemyCollectable::FallFromSky(float elapsedSec) noexcept
{
	const float GRAVITY{ -100.f };

	if (m_Position.y > 70.f)
	{
		m_VelocityY += GRAVITY * elapsedSec;
		m_Position.y += m_VelocityY * elapsedSec;
	}
	else
	{
		m_Position.y = 70.f;
	}
}

void EnemyCollectable::UpdateCurrFrameNumber(float elapsedSec) noexcept
{
	m_AccTime += elapsedSec;

	if (m_AccTime >= m_FrameTime)
	{
		m_AccTime = 0;
		m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 3;
	}
}

const Rectf& EnemyCollectable::GetCollisionRect() const
{
	return m_CollisionRect;
}

void EnemyCollectable::SetCollisionRect() noexcept
{
	m_CollisionRect = Rectf{ m_Position.x, m_Position.y - 1.f, m_pTexture->GetWidth() / 3 * 2, m_pTexture->GetHeight() * 2 };

}
