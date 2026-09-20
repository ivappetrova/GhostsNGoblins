#include "pch.h"
#include "Plant.h"
#include "Texture.h"
#include "utils.h"
#include "SoundEffect.h"
#include <iostream>
#include "Player.h"
#include "Health.h"
#include "UImanager.h"
#include "EnemyBullet.h"

Plant::Plant(const Vector2f& position, const Vector2f& playerPos) :
	Enemy(1, position, 100),
	m_pTexture(new Texture("Enemies/plant.png")),
	m_FRAME_WIDTH{ m_pTexture->GetWidth() / 5 },
	m_CurrentFrameNr{},
	m_AccTime{},
	m_SrcRect{ 0.f, 0.f, m_FRAME_WIDTH, m_pTexture->GetHeight() },
	m_IsFlipSprite{ false },
	m_HasShot{false},
	m_IsAbleToShoot{false}
{
	UpdateCollisionRect();
	UpdateDirection(playerPos.x);

	m_pBullets.reserve(3);
}

void Plant::UpdateCollisionRect()
{
	if (!m_pHp->IsDead())
	{
		m_CollisionRect = Rectf{ m_Position.x, m_Position.y, m_FRAME_WIDTH * 2.3f, m_pTexture->GetHeight() * 2.3f + 10.f };
	}
	else
	{
		m_CollisionRect = Rectf{ -200.f, -600.f, 0.f, 0.f };
	}
}

void Plant::UpdateDirection(const float playerPosX)
{
	if (m_Position.x < playerPosX)
	{
		m_PlayerCurrentDirectionX = 1.f;
	}
	else
	{
		m_PlayerCurrentDirectionX = -1.f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Plant::~Plant() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;

	for (EnemyBullet* bullet : m_pBullets)
	{
		if (bullet != nullptr)
		{
			delete bullet;
			bullet = nullptr;
		}
	}
	m_pBullets.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Plant::Draw() const
{;
	glPushMatrix();

		if (m_IsFlipSprite)
		{
			FlipSprite(); 
		}

		m_pTexture->Draw(m_DstRect, m_SrcRect);

	glPopMatrix();

	for (EnemyBullet* bullet : m_pBullets)
	{
		if (bullet != nullptr)
		{
			bullet->Draw();
		}
	}

	/*utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	utils::DrawRect(Enemy::GetCollisionRect());*/
}

void Plant::FlipSprite() const noexcept
{
	glTranslatef(m_Position.x + m_FRAME_WIDTH, m_Position.y, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-(m_Position.x + m_FRAME_WIDTH), -m_Position.y, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Plant::Update(float elapsedSec, Player* player, UImanager* UI)
{
	Enemy::Update(elapsedSec, player, UI);

	m_AccTime += elapsedSec;

	UpdateDirection(player->GetPosition().x);
	UpdateDstRect();
	UpdateSrcRect();
	UpdateCollisionRect();
	CheckFlipSprite();

	CheckIfAbleToShoot(player->GetPosition().x);
	if (m_IsAbleToShoot && !m_pHp->IsDead())
	{
		Shoot(player);
	}

	if (m_pHp->IsDead() && !m_HasHandledDeath)
	{
		OnDeath(UI);
		m_HasHandledDeath = true;
		m_IsAbleToShoot = false;
	}

	CheckPlayerCollision(player);

	UpdateCurrentFrameNr();
	UpdateBullets(elapsedSec, player);
}

void Plant::UpdateDstRect()
{
	if (m_pHp->IsDead())
	{
		m_DstRect = Rectf{ m_Position.x-20.f, m_Position.y, 80.f, 80.f };
	}
	else
	{
		m_DstRect = Rectf{ m_Position.x, m_Position.y, m_FRAME_WIDTH * 2.3f, m_pTexture->GetHeight() * 2.3f };
	}
}

void Plant::UpdateSrcRect()
{
	if (m_pHp->IsDead())
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 4, m_pTexture->GetHeight(), m_pTexture->GetWidth() / 4, m_pTexture->GetHeight() / 2 };
	}
	else
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FRAME_WIDTH, m_pTexture->GetHeight(), m_FRAME_WIDTH, m_pTexture->GetHeight() };
	}
}

void Plant::CheckFlipSprite() noexcept
{
	if (m_PlayerCurrentDirectionX > 0)
	{
		m_IsFlipSprite = true;
	}
	else
	{
		m_IsFlipSprite = false;
	}
}

void Plant::OnDeath(UImanager* UI)
{
	UI->AddPoints(m_Points);
	
	m_CurrentFrameNr = 0;
	m_pDeathEffect->Play(0);

	for (EnemyBullet* bullet : m_pBullets)
	{
		if (bullet != nullptr)
		{
			delete bullet;
			bullet = nullptr;
		}
	}
	m_pBullets.clear();

	delete m_pTexture;
	m_pTexture = new Texture("Enemies/deathAnimation.png");
}

void Plant::CheckIfAbleToShoot(const float playerX)
{
	if (playerX > m_Position.x - 400.f && playerX < m_Position.x + 400.f)
	{
		m_IsAbleToShoot = true;
	}
	else
	{
		m_IsAbleToShoot = false;
	}
}

void Plant::UpdateCurrentFrameNr()
{
	if (m_pHp->IsDead())
	{
		if (!m_HasPlayedDeathAnim && m_AccTime >= 0.15f)
		{
			m_AccTime = 0;
			++m_CurrentFrameNr;

			if (m_CurrentFrameNr >= 4)
			{
				m_CurrentFrameNr = 3;
				m_HasPlayedDeathAnim = true;
			}
		}
	}
	else if (m_AccTime >= 0.4f)
	{
		m_AccTime = 0;
		m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 5;
	}

}

void Plant::Shoot(Player* player)
{
	if (m_CurrentFrameNr != 2)
	{
		m_HasShot = false;
	}


	if (m_CurrentFrameNr == 2 && !m_HasShot)
	{
		m_HasShot = true;
		m_pBullets.push_back(new EnemyBullet(Vector2f{ m_Position.x, m_Position.y - 10.f }, player->GetPosition(), 3, 4));
	}
}

void Plant::UpdateBullets(float elapsedSec, Player* player)
{
	for (int index{}; index < m_pBullets.size(); ++index)
	{
		EnemyBullet* enemyBullet = m_pBullets[index];
		if (enemyBullet != nullptr)
		{
			enemyBullet->Update(elapsedSec, player);

			if (enemyBullet->BulletLifeTimeEnded() || enemyBullet->CollidesWithPlayer())
			{
				delete enemyBullet;
				m_pBullets.erase(m_pBullets.begin() + index);
				continue;
			}
		}
	}
}

void Plant::CheckPlayerCollision(Player* player)
{
	if (utils::IsOverlapping(m_CollisionRect, player->GetCollisionRect()))
	{
		player->TakeDmg(1);
	}
}

void Plant::TakeDamage(int amount)
{
	Enemy::TakeDamage(amount);
}




