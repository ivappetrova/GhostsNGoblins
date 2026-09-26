#include "pch.h"
#include "EnemyBullet.h"
#include "Texture.h"
#include "utils.h"
#include "Player.h"
#include <iostream>

EnemyBullet::EnemyBullet(const Vector2f& position, const Vector2f& playerPos, int bulletSrcRectY, int bulletSrcRectFrames) :
	m_Position{ position.x - 5.f, position.y + 55.f },
	m_pBulletTexture{new Texture("Enemies/enemyBullets.png")},
	m_BulletSrcRectY{ bulletSrcRectY },
	m_BulletSrcRectFrames{bulletSrcRectFrames},
	m_SPEED{200.f},
	m_Lifetime{},
	m_MAX_LIFETIME{5.f},
	m_CollidesWithPlayer{false},
	m_AccTime{},
	m_CurrentFrameNr{}
{
	SetDirection(playerPos);
}

void EnemyBullet::SetDirection(const Vector2f& playerPos)
{
	float dirX{};
	float dirY{};

	// x
	if (playerPos.x > m_Position.x - 100.f && playerPos.x < m_Position.x + 100.f)
	{
		dirX = 0;
	}
	else if (m_Position.x > playerPos.x)
	{
		dirX = -1.f;
	}
	else
	{
		dirX = 1.f;
	}

	//y
	if (playerPos.y > m_Position.y - 50.f && playerPos.y < m_Position.y + 100.f)
	{
		dirY = 0;
	}
	else if (m_Position.y > playerPos.y)
	{
		dirY = -1.f;
	}
	else
	{
		dirY = 1.f;
	}

	m_Direction = Vector2f{ dirX, dirY};
}

EnemyBullet::~EnemyBullet() noexcept
{
	delete m_pBulletTexture;
	m_pBulletTexture = nullptr;
}

void EnemyBullet::Draw() const
{
	Rectf dst{ m_Position.x, m_Position.y, 40.f, 40.f};
	Rectf src{ m_CurrentFrameNr * m_pBulletTexture->GetWidth() / 4, m_BulletSrcRectY * m_pBulletTexture->GetHeight()/3, m_pBulletTexture->GetWidth() /4, m_pBulletTexture->GetHeight()/3 };

	m_pBulletTexture->Draw(dst, src);

	//// Draw collision rectangle 
	//utils::SetColor(Color4f(0.f, 1.f, 0.f, 1.f));
	//utils::DrawRect(m_CollisionRect);
}

void EnemyBullet::Update(float elapsedSec, Player* player)
{
	m_AccTime += elapsedSec;
	m_Lifetime += elapsedSec;

	if (m_Direction.x == 0 || m_Direction.y == 0)
	{
		m_Position.x += m_Direction.x * m_SPEED * elapsedSec;
		m_Position.y += m_Direction.y * m_SPEED * elapsedSec;
	}
	else
	{
		m_Position.x += m_Direction.x * (m_SPEED/2) * elapsedSec;
		m_Position.y += m_Direction.y * (m_SPEED/2) * elapsedSec;
	}

	SetCollisionRect();

	ChangeCurrentFrame();

	CheckPlayerCollision(player);
}

void EnemyBullet::SetCollisionRect() noexcept
{
	m_CollisionRect = Rectf{ m_Position.x + 15.f, m_Position.y + 8.f, m_pBulletTexture->GetWidth() /4 -6.f, m_pBulletTexture->GetHeight() /3 -2.f};
}

void EnemyBullet::ChangeCurrentFrame()
{
	if (m_AccTime >= 0.15f)
	{
		m_AccTime = 0;
		m_CurrentFrameNr = (m_CurrentFrameNr + 1) % m_BulletSrcRectFrames;
	}
}

void EnemyBullet::CheckPlayerCollision(Player* player)
{
	if (utils::IsOverlapping(m_CollisionRect,player->GetCollisionRect()))
	{
		m_CollidesWithPlayer = true;
		player->TakeDmg(1);
	}
}

bool EnemyBullet::BulletLifeTimeEnded() const noexcept
{
	return m_Lifetime >= m_MAX_LIFETIME;
}

bool EnemyBullet::CollidesWithPlayer() const noexcept
{
	return m_CollidesWithPlayer;
}

