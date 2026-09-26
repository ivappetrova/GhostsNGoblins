#include "pch.h"
#include "Water.h"
#include "Texture.h"
#include "utils.h"

Water::Water(const Vector2f& position, const float width):
	m_Position{position},
	m_pTexture{new Texture("BG/water.png")},
	m_SPEED{ 50.f },
	m_WIDTH{ width },
	m_HEIGHT{ 45.f },
	m_SrcPosX{ },
	m_SrcPosY{ },
	m_SrcRect{m_SrcPosX, m_SrcPosY, m_WIDTH, m_pTexture-> GetHeight()},
	m_CollisionRect{m_Position.x, m_Position.y, m_WIDTH, m_HEIGHT-10.f}
{
}

Water::~Water() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void Water::Draw() const
{
	Rectf dstRect{ m_Position.x, m_Position.y, m_WIDTH, m_HEIGHT };
	m_pTexture->Draw(dstRect, m_SrcRect);

	/*utils::SetColor(Color4f{ 1.f, 1.f, 0.f, 1.f });
	utils::DrawRect(m_CollisionRect);*/
}

void Water::Update(float elapsedSec)
{
	m_SrcPosX -= m_SPEED * elapsedSec;

	m_SrcRect = Rectf{ m_SrcPosX, m_SrcPosY, m_WIDTH, m_pTexture->GetHeight()};

}

Rectf Water::GetCollisionRect() const noexcept
{
	return m_CollisionRect;
}


