#include "pch.h"
#include "Zombie.h"
#include "Texture.h"
#include <iostream>
#include "utils.h"
#include "Player.h"
#include "Health.h"
#include "UImanager.h"
#include "SoundEffect.h"

Zombie::Zombie(const Vector2f& spawnLocation, const float playerPosXInEnemyInitTime) :
	Enemy(1, spawnLocation, 200),
	m_pTexture(new Texture("Enemies/zombie.png")),
	m_FRAME_WIDTH{ m_pTexture->GetWidth() / 16 },
	m_CurrentFrameNr{ 0 },
	m_AccTime{ 0.f },
	m_SPEED(120.f),
	m_DstRect{ m_Position.x, m_Position.y, m_FRAME_WIDTH * 2, m_pTexture->GetHeight() * 2 },
	m_SrcRect{ 0.f, 0.f, m_FRAME_WIDTH, m_pTexture->GetHeight() },
	m_IsAbleToMove{false},
	m_EnemyState{State::spawning},
	m_TimeAlive{0.f},
	m_WentDown{false}
{
	SetDirection(playerPosXInEnemyInitTime);
	UpdateCollisionRect();
}

void Zombie::SetDirection(const float playerPosXInEnemyInitTime)
{
	if (this->m_Position.x < playerPosXInEnemyInitTime)
	{
		m_DirectionX = 1.f;
	}
	else
	{
		m_DirectionX = -1.f;
	}
}

void Zombie::UpdateCollisionRect()
{
	switch (m_EnemyState)
	{
	case State::spawning:
	case State::goingDown:
	case State::dead:
		m_CollisionRect = Rectf{ 0.f, - 200.f, 0.f, 0.f };
		break;

	case State::walking:
		m_CollisionRect = Rectf{ m_DstRect.left, m_DstRect.bottom, m_DstRect.width, m_DstRect.height-10.f };
		break;
	}
}

Zombie::~Zombie() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void Zombie::Draw() const
{
	if (m_HasPlayedDeathAnim) return;

	glPushMatrix(); 

		if (m_DirectionX > 0)
		{
			FlipSprite();
		}

		m_pTexture->Draw(m_DstRect, m_SrcRect);

		/*utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
		utils::DrawRect(m_DstRect);

		utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
		utils::DrawRect(Enemy::GetCollisionRect());*/

	glPopMatrix();
}

void Zombie::FlipSprite() const noexcept
{
	glTranslatef(m_Position.x + m_FRAME_WIDTH * 2 / 2.f, m_Position.y, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-(m_Position.x + m_FRAME_WIDTH * 2 / 2.0f), -m_Position.y, 0);
}

void Zombie::Update(float elapsedSec, Player* player, UImanager* UI)
{
	Enemy::Update(elapsedSec, player, UI);

	m_AccTime += elapsedSec;

	TransitionToWalking();
	
	if (m_IsAbleToMove)
	{
		Move(elapsedSec);
		m_TimeAlive += elapsedSec;
	}

	TransitionToGoingDown();
	CheckIfWentDownFinished();

	
	UpdateDstRect();
	UpdateCollisionRect();
	CheckPlayerCollision(player);
	

	if (m_EnemyState == State::dead && !m_HasHandledDeath)
	{
		OnDeath(UI);
		m_HasHandledDeath = true;
	}
	// adds the pts more than once

	UpdateCurrentFrameNr();
}

void Zombie::TransitionToWalking()
{
	if (m_EnemyState == State::spawning && m_CurrentFrameNr >= 6)
	{
		m_EnemyState = State::walking;
		m_IsAbleToMove = true;
		m_CurrentFrameNr = 0;
		m_AccTime = 0.f;
	}
}

void Zombie::Move(float elapsedSec)
{
	m_Position.x += m_DirectionX * m_SPEED * elapsedSec;
}

void Zombie::TransitionToGoingDown()
{
	if ( (m_EnemyState == State::walking) && ( (m_TimeAlive >= 10.f) ||
		 (m_Position.y > 100.f && m_Position.x < 1325.f) ||
		 (m_Position.y > 100.f && m_Position.x > 2360.f) ) )
	{ 
		m_EnemyState = State::goingDown;
		m_IsAbleToMove = false;
		m_CurrentFrameNr = 0;
		m_AccTime = 0.f;
	}
}

void Zombie::CheckIfWentDownFinished()
{
	if (m_EnemyState == State::goingDown && m_CurrentFrameNr >= 6)
	{
		m_WentDown = true;
	}
}

bool Zombie::WentDown() const noexcept
{
	return m_WentDown;
}


void Zombie::UpdateDstRect()
{
	m_DstRect = Rectf{ m_Position.x, m_Position.y, 48.f, 74.f };
}

void Zombie::TakeDamage(int amount)
{
	Enemy::TakeDamage(amount);
	m_EnemyState = State::dead;
}

void Zombie::CheckPlayerCollision(Player* player)
{
	if (utils::IsOverlapping(m_CollisionRect, player->GetCollisionRect()))
	{
		player->TakeDmg(1);
	}
}

void Zombie::OnDeath(UImanager* UI)
{
	m_IsAbleToMove = false;
	UI->AddPoints(m_Points); 
	m_CurrentFrameNr = 0;

	m_pDeathEffect->Play(0);

	delete m_pTexture;
	m_pTexture = new Texture("Enemies/deathAnimation.png");
}

void Zombie::UpdateCurrentFrameNr() 
{
	const float SPAWN_TIME{ 0.25f };
	const float WALK_TIME{ 0.2f };

	switch (m_EnemyState)
	{
	case State::walking:
		if (m_AccTime >= WALK_TIME)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 2;
		}
		break;

	case State::spawning:
	case State::goingDown:
		if (m_AccTime >= SPAWN_TIME)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 7;
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

	UpdateSrcRect();
}

void Zombie::UpdateSrcRect() noexcept
{
	switch (m_EnemyState)
	{
	case Zombie::State::spawning:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FRAME_WIDTH, 0.f, m_FRAME_WIDTH, m_pTexture->GetHeight() };
		break;

	case Zombie::State::walking:
		m_SrcRect = Rectf{ 7 * m_FRAME_WIDTH + m_CurrentFrameNr * m_FRAME_WIDTH, 0.f, m_FRAME_WIDTH, m_pTexture->GetHeight() };
		break;

	case Zombie::State::goingDown:
		m_SrcRect = Rectf{ 9 * m_FRAME_WIDTH + m_CurrentFrameNr * m_FRAME_WIDTH, 0.f, m_FRAME_WIDTH, m_pTexture->GetHeight() };
		break;

	case Zombie::State::dead:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 4, m_pTexture->GetHeight()/2, m_pTexture->GetWidth()/4, m_pTexture->GetHeight() / 2};
		break;
	}
}

