#include "pch.h"
#include "CoinCollectable.h"
#include "Texture.h"
#include "utils.h"

CoinCollectable::CoinCollectable(const Vector2f& position):
	Collectable(position, 200),
	m_pTexture{new Texture("Collectables/coin.png")},
	m_FrameTime{ 0.15f },
	m_AccTime{},
	m_CurrentFrameNr{ 0 },
	m_FRAME_WIDTH{ m_pTexture->GetWidth() / 3 },
	m_FRAME_HEIGHT{ m_pTexture->GetHeight() },
	m_SrcRectX{ 0 },
	m_IsCollected{false}
{
	SetCollisionRect();
}

CoinCollectable::~CoinCollectable() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void CoinCollectable::Update(float elapsedSec)
{
	m_AccTime += elapsedSec;

	if (m_AccTime >= m_FrameTime)
	{
		m_AccTime = 0;
		m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 3;

	}
	
}

void CoinCollectable::Draw() const
{
	Rectf dst{ m_Position.x, m_Position.y, m_pTexture->GetWidth()/3*2, m_pTexture->GetHeight() *2};
	Rectf src{ m_CurrentFrameNr * m_pTexture->GetWidth()/3,  m_pTexture->GetHeight(), m_pTexture->GetWidth() / 3 , m_pTexture->GetHeight()};


	m_pTexture->Draw(dst, src);

	/*utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });
	utils::DrawRect(m_CollisionRect);*/
}

const Rectf& CoinCollectable::GetCollisionRect() const
{
	return m_CollisionRect;
}


void CoinCollectable::SetCollisionRect() noexcept
{
	m_CollisionRect = Rectf{ m_Position.x, m_Position.y - 1.f, m_pTexture->GetWidth()/3*2, m_pTexture->GetHeight()*2};

}