#include "pch.h"
#include "PlayerBullet.h"
#include "Texture.h"
#include "utils.h"
#include "Enemy.h"
#include "ShieldBearer.h"
#include <iostream>

PlayerBullet::PlayerBullet(const Vector2f& position, float direction):
	m_Position{ position.x, position.y },
	m_pBulletTexture{ new Texture ("Player/Lance.png")},
	m_SPEED{400.f},
	m_Lifetime{},
	m_MAX_LIFETIME{1.f},
	m_Direction{direction},
	m_CollidesWithLevel{false},
	m_CollidesWithEnemy{false},
	m_IsFlipSprite{false}
{
	if (m_Direction == -1.f)
	{
		m_IsFlipSprite = true;
	}
	SetCollisionRect();
}

void PlayerBullet::SetCollisionRect() noexcept
{
	if (m_IsFlipSprite)
	{
		m_CollisionRect = Rectf{ m_Position.x + m_pBulletTexture->GetWidth() * 1.5f, m_Position.y - 1.f, m_pBulletTexture->GetWidth() * 1.5f, m_pBulletTexture->GetHeight() * 2 };
	}
	else
	{
		m_CollisionRect = Rectf{ m_Position.x, m_Position.y - 1.f, m_pBulletTexture->GetWidth() * 1.5f, m_pBulletTexture->GetHeight() * 2 };
	}
}

PlayerBullet::~PlayerBullet() noexcept
{
	delete m_pBulletTexture;
	m_pBulletTexture = nullptr;
}

void PlayerBullet::Draw() const
{
	glPushMatrix();

	if (m_IsFlipSprite)
	{
		FlipSprite();
	}

	m_pBulletTexture->Draw(Rectf{ m_Position.x, m_Position.y, m_pBulletTexture->GetWidth() * 1.5f, m_pBulletTexture->GetHeight() * 1.5f });


	glPopMatrix();

	//// Draw collision rectangle 
	//utils::SetColor(Color4f(0.f, 1.f, 0.f, 1.f));
	//utils::DrawRect(m_CollisionRect);

}

void PlayerBullet::FlipSprite() const noexcept
{
	glTranslatef(m_Position.x + m_pBulletTexture->GetWidth() * 1.5f, m_Position.y, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-(m_Position.x + m_pBulletTexture->GetWidth() * 1.5f), -m_Position.y, 0);
}

void PlayerBullet::Update(float elapsedSec, const std::vector< std::vector<Vector2f> >& levelvertices, 
					std::vector<Enemy*>& enemies)
{
	m_Lifetime += elapsedSec;
	m_Position.x += m_Direction * m_SPEED * elapsedSec;

	SetCollisionRect();

	CheckLevelCollision(levelvertices);
	CheckEnemyCollision(enemies);
	
}

bool PlayerBullet::BulletLifeTimeEnded() const noexcept
{
	return m_Lifetime >= m_MAX_LIFETIME;
	
}

void PlayerBullet::CheckLevelCollision(const std::vector< std::vector<Vector2f> >& levelVertices)
{
	utils::HitInfo hitinfo;

	/////HORIZONTAL = left/right collision
	// Check collision with bottom of the rect
	Vector2f bottomStart{ m_CollisionRect.left + 1.f, m_CollisionRect.bottom + 5.f };
	Vector2f bottomEnd{ m_CollisionRect.left + m_CollisionRect.width, m_CollisionRect.bottom + 5.f };

	// Check collision with vertical mid of the rect
	Vector2f midHStart{ m_CollisionRect.left + 1.f, m_CollisionRect.bottom + m_CollisionRect.height / 2 };
	Vector2f midHEnd{ m_CollisionRect.left + m_CollisionRect.width, m_CollisionRect.bottom + m_CollisionRect.height / 2 };

	// Check collision with top of the rect
	Vector2f topStart{ m_CollisionRect.left + 1.f, m_CollisionRect.bottom + m_CollisionRect.height - 5.f };
	Vector2f topEnd{ m_CollisionRect.left + m_CollisionRect.width, m_CollisionRect.bottom + m_CollisionRect.height - 5.f };


	for (int index{}; index < levelVertices.size(); ++index)
	{
		// check for left/right collision
		if (utils::Raycast(levelVertices[index], bottomStart, bottomEnd, hitinfo)
			|| utils::Raycast(levelVertices[index], midHStart, midHEnd, hitinfo)
			|| utils::Raycast(levelVertices[index], topStart, topEnd, hitinfo))
		{
			// left
			if (hitinfo.intersectPoint.x >= m_CollisionRect.left &&
				hitinfo.intersectPoint.x <= m_CollisionRect.left + m_CollisionRect.width / 2)
			{
				m_Position.x = hitinfo.intersectPoint.x;
				m_CollidesWithLevel = true;


			}
			// right
			else if (hitinfo.intersectPoint.x >= m_CollisionRect.left + m_CollisionRect.width / 2 &&
				hitinfo.intersectPoint.x <= m_CollisionRect.left + m_CollisionRect.width)
			{
				m_Position.x = hitinfo.intersectPoint.x - m_CollisionRect.width;
				m_CollidesWithLevel = true;
			}
		}
	}

}

void PlayerBullet::CheckEnemyCollision(std::vector<Enemy*>& enemies)
{
	for (int index{}; index < enemies.size(); ++index)
	{
		// check specially for shield
		if (ShieldBearer* shieldBearer = dynamic_cast<ShieldBearer*>(enemies[index]))
		{
			if (utils::IsOverlapping(m_CollisionRect, shieldBearer->GetShieldRect()))
			{
				m_CollidesWithLevel = true;
			}
			else if (utils::IsOverlapping(m_CollisionRect, shieldBearer->GetCollisionRect()))
			{
				m_CollidesWithEnemy = true;
				enemies[index]->TakeDamage(1);
			}
		} 
		else if (utils::IsOverlapping(m_CollisionRect, enemies[index]->GetCollisionRect()))
		{
			m_CollidesWithEnemy= true;
			enemies[index]->TakeDamage(1);
		}
	}
}

bool PlayerBullet::CollidesWithLevel() const noexcept
{
	return m_CollidesWithLevel;
}

bool PlayerBullet::CollidesWithEnemy() const noexcept
{
	return m_CollidesWithEnemy;
}


