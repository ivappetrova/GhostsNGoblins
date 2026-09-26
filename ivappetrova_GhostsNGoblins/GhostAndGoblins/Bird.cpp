#include "pch.h"
#include "Bird.h"
#include "Texture.h"
#include "SoundEffect.h"
#include "utils.h"
#include "Player.h"
#include "Health.h"
#include <iostream>
#include "UImanager.h"
#include "Enemy.h"

Bird::Bird(const Vector2f& position):
	Enemy(1, position, 100),
	m_pTexture(new Texture("Enemies/bird.png")),
	m_FrameWidth{m_pTexture->GetWidth()/4},
	m_CurrentFrameNr{},
	m_AccTime{},
	m_IsAbleToFly{false},
	m_SPEED{200.f},
	m_EnemyState{State::sit}
{
}

Bird::~Bird() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void Bird::Draw() const
{
	m_pTexture->Draw(m_DstRect, m_SrcRect);


	/*utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::DrawRect(m_DstRect);

	utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	utils::DrawRect(Enemy::GetCollisionRect());*/
}

void Bird::Update(float elapsedSec, Player* player, UImanager* UI)
{
	Enemy::Update(elapsedSec, player, UI);

	m_AccTime += elapsedSec;

	UpdateDstRect();
	UpdateCollisionRect();

	if (!m_IsAbleToFly && m_EnemyState != State::dead)
	{
	CheckIfAbleToMove(player->GetPosition().x);
	}

	if (m_IsAbleToFly)
	{
		Fly(elapsedSec);
	}

	if ((m_pHp->IsDead() || m_EnemyState == State::dead) && !m_HasHandledDeath)
	{
		OnDeath(UI);
		m_HasHandledDeath = true;
	}

	CheckPlayerCollision(player);
	UpdateCurrentFrameNr();
	UpdateSrcRect();
}

void Bird::UpdateDstRect() noexcept
{
	m_DstRect = Rectf{ m_Position.x, m_Position.y, 30.f, 30.f };
}

void Bird::UpdateCollisionRect()
{
	if (m_EnemyState != State::dead && !m_pHp->IsDead())
	{
		m_CollisionRect = Rectf{ m_Position.x, m_Position.y, 30.f, 30.f };
	}
	else
	{
		m_CollisionRect = Rectf{ 0.f, -200.f, 0.f, 0.f };
	}
}

void Bird::CheckIfAbleToMove(const float playerX)
{
	if (playerX > m_Position.x - 270.f && !m_IsAbleToFly)
	{
		m_IsAbleToFly = true;
		m_EnemyState = State::fly;
	}
	else
	{
		if (!m_pHp->IsDead())
		{
			m_EnemyState = State::sit;
		}
	}
}

void Bird::Fly(float elapsedSec) noexcept
{
	m_Position.x -= m_SPEED * elapsedSec;
}

void Bird::CheckPlayerCollision(Player* player)
{
	if (utils::IsOverlapping(m_CollisionRect, player->GetCollisionRect()))
	{
		player->TakeDmg(1);
	}
}

void Bird::OnDeath(UImanager* UI)
{
	UI->AddPoints(m_Points);
	m_IsAbleToFly = false;
	m_CurrentFrameNr = 0;

	m_pDeathEffect->Play(0);

	delete m_pTexture;
	m_pTexture = new Texture("Enemies/deathAnimation.png");

	m_FrameWidth = m_pTexture->GetWidth() / 4;
}

void Bird::UpdateCurrentFrameNr() noexcept
{
	switch (m_EnemyState)
	{
	case State::sit:
	case State::fly:
		if (m_AccTime >= 0.2f)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 4;
		}
		break;

	case State::dead:
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
		break;
	}
}

void Bird::UpdateSrcRect() noexcept
{
	if (m_pHp->IsDead())
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, m_pTexture->GetHeight()/2, m_FrameWidth, m_pTexture->GetHeight()/2 };
	}
	else if (m_EnemyState== State::sit)
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, m_pTexture->GetHeight(), m_FrameWidth, m_pTexture->GetHeight()/2};
	}
	else
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, m_pTexture->GetHeight()/2, m_FrameWidth, m_pTexture->GetHeight()/2};
	}
}

void Bird::TakeDamage(int amount)
{
	Enemy::TakeDamage(amount);
	m_EnemyState = State::dead;
}