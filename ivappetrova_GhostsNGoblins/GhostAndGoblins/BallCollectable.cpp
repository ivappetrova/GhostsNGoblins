#include "pch.h"
#include "BallCollectable.h"
#include "Texture.h"
#include "utils.h"

BallCollectable::BallCollectable(const Vector2f& position) :
	Collectable(position, 10000, false),
	m_pTexture{ new Texture("Collectables/pot.png") },
	m_FrameTime{ 0.15f },
	m_AccTime{},
	m_CurrentFrameNr{ 0 },
	m_SrcRectX{ 0 },
	m_IsCollected{ false },
	m_VelocityY{},
	m_TransitionEnded{false}
{
}

BallCollectable::~BallCollectable() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void BallCollectable::Draw() const
{
	Rectf dst;
	Rectf src;

	if (!m_IsBagOpen)
	{
		dst= Rectf{ m_Position.x, m_Position.y, m_pTexture->GetWidth() * 2, m_pTexture->GetHeight() * 2 };
		src= Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth(),  m_pTexture->GetHeight(), m_pTexture->GetWidth(), m_pTexture->GetHeight() };
	}
	else
	{
		dst = Rectf{ m_Position.x, m_Position.y, m_pTexture->GetWidth() / 3 * 2, m_pTexture->GetHeight() * 2 };
		src = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 3,  m_pTexture->GetHeight(), m_pTexture->GetWidth() / 3 , m_pTexture->GetHeight() };
	}

	m_pTexture->Draw(dst, src);

	/*utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });
	utils::DrawRect(m_CollisionRect);*/
}

void BallCollectable::Update(float elapsedSec)
{
	if (!m_IsBagOpen)
	{
		FallFromSky(elapsedSec);
	}

	if (m_IsBagOpen)
	{
		if (!m_TransitionEnded)
		{
			SwitchToBall();
		}
		else
		{
			SetCollisionRect();
			UpdateCurrFrameNumber(elapsedSec);
		}
	}
}

void BallCollectable::FallFromSky(float elapsedSec) noexcept
{
	const float GRAVITY{ -980.f };

	if (m_Position.y > 75.f)
	{
		m_VelocityY += GRAVITY * elapsedSec;
		m_Position.y += m_VelocityY * elapsedSec;
	}
	else
	{
		m_Position.y = 75.f;
		m_IsBagOpen = true;
	}
}

void BallCollectable::SwitchToBall()
{
	delete m_pTexture;
	m_pTexture = new Texture("Collectables/ball.png");

	m_TransitionEnded = true;
	SetCollisionRect();
}

void BallCollectable::SetCollisionRect() noexcept
{
	m_CollisionRect = Rectf{ m_Position.x, m_Position.y - 1.f, m_pTexture->GetWidth() / 3 * 2, m_pTexture->GetHeight() * 2 };

}

void BallCollectable::UpdateCurrFrameNumber(float elapsedSec)
{
	m_AccTime += elapsedSec;

	if (m_AccTime >= m_FrameTime)
	{
		m_AccTime = 0;
		m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 3;
	}
}

const Rectf& BallCollectable::GetCollisionRect() const
{
	return m_CollisionRect;
}



