#include "pch.h"
#include "MoneyBag.h"
#include "Texture.h"
#include "utils.h"

MoneyBag::MoneyBag(const Vector2f& position) :
	Collectable(position, 500),
	m_pTexture{ new Texture("Collectables/moneyBag.png") },
	m_IsCollected{ false }
{
	SetCollisionRect();
}

MoneyBag::~MoneyBag() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void MoneyBag::Update(float elapsedSec)
{
	// do nothing
}

void MoneyBag::Draw() const
{
	Rectf dst{ m_Position.x, m_Position.y, m_pTexture->GetWidth() * 2, m_pTexture->GetHeight() * 2 };


	m_pTexture->Draw(dst);

	/*utils::SetColor(Color4f{ 0.f, 1.f, 1.f, 1.f });
	utils::DrawRect(m_CollisionRect);*/
}

const Rectf& MoneyBag::GetCollisionRect() const
{
	return m_CollisionRect;
}


void MoneyBag::SetCollisionRect() noexcept
{
	m_CollisionRect = Rectf{ m_Position.x, m_Position.y - 1.f, m_pTexture->GetWidth() * 2, m_pTexture->GetHeight() * 2 };

}