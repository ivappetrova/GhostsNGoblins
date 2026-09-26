#include "pch.h"
#include "ShieldBearer.h"
#include "Texture.h"
#include "utils.h"
#include "Player.h"
#include "Health.h"
#include <iostream>
#include "UImanager.h"
#include "Enemy.h"
#include "SoundEffect.h"

ShieldBearer::ShieldBearer(const Vector2f& position) :
	Enemy(1, position, 100),
	m_pTexture(new Texture("Enemies/shieldBearer.png")),
	m_CurrentFrameNr{},
	m_AccTime{},
	m_IsAbleToMove{ false },
	m_WIDTH{ 35.f },
	m_HEIGHT{ 70.f },
	m_TickCounter{},
	m_DstRect{m_Position.x, m_Position.y, m_WIDTH, m_HEIGHT},
	m_pMoveEffect{new SoundEffect("Sounds/Effects/shieldBearer.wav")}
{
}

ShieldBearer::~ShieldBearer() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;

	delete m_pMoveEffect;
	m_pMoveEffect = nullptr;
}

void ShieldBearer::Draw() const
{
	m_pTexture->Draw(m_DstRect, m_SrcRect);

	/*utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::DrawRect(m_DstRect);

	utils::SetColor(Color4f{ 0.f, 0.f, 1.f, 1.f });
	utils::DrawRect(this->GetShieldRect());

	utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	utils::DrawRect(Enemy::GetCollisionRect());*/
}

void ShieldBearer::Update(float elapsedSec, Player* player, UImanager* UI)
{
	Enemy::Update(elapsedSec, player, UI);

	m_AccTime += elapsedSec;
	m_pMoveEffect->SetVolume(1);

	if (!m_IsAbleToMove && !m_pHp->IsDead())
	{
		CheckIfAbleToMove(player->GetPosition().x);
	}

	if (m_IsAbleToMove)
	{
		Move(elapsedSec);
		UpdateCollisionRect();
		UpdateShieldRect();
	}

	if (m_IsAbleToMove &&
		m_Position.x > player->GetPosition().x - 300.f && 
		m_Position.x < player->GetPosition().x + 300.f)
	{
		m_pMoveEffect->Play(0);
	}

	if (m_pHp->IsDead() && !m_HasHandledDeath)
	{
		OnDeath(UI);
		m_HasHandledDeath = true;
	}

	CheckPlayerCollision(player);
	UpdateCurrentFrameNr();
	UpdateSrcRect();
}

void ShieldBearer::UpdateCollisionRect()
{
	if (!m_pHp->IsDead())
	{
		m_CollisionRect = Rectf{ m_DstRect.left + 10.f, m_DstRect.bottom, m_DstRect.width - 10.f, m_DstRect.height };
	}
	else
	{
		m_CollisionRect = Rectf{ 0.f, -200.f, 0.f, 0.f };
	}
}

void ShieldBearer::UpdateShieldRect() noexcept
{
	if (!m_pHp->IsDead())
	{
		m_ShieldRect = Rectf{ m_DstRect.left, m_DstRect.bottom, 10.f, m_DstRect.height };
	}
	else
	{
		m_ShieldRect = Rectf{ 0.f, -200.f, 0.f, 0.f };
	}
}

const Rectf ShieldBearer::GetShieldRect() const noexcept
{
	return m_ShieldRect;
}


void ShieldBearer::CheckIfAbleToMove(const float playerX)
{
	if (playerX > m_Position.x - 450.f && !m_IsAbleToMove)
	{
		m_IsAbleToMove = true;
	}
}

void ShieldBearer::Move(float elapsedSec) noexcept
{
	const float SPEED_X{ 70.f };

	m_Position.x -= SPEED_X * elapsedSec;

	const float FRAMERATE{ 50.f };
	const float PERIOD_IN_SECONDS{ 1.f };
	const float AMPLITUDE{ 100.f };
	const float WAVE_SPEED{ 100.f }; 

	// Update wave time
	m_TickCounter += elapsedSec;

	// Calculate wave percentage (0 to 1 over the period)
	float waveMovementInPercent = m_TickCounter / (FRAMERATE * PERIOD_IN_SECONDS);

	// Vertical deviation based on sine wave
	float deviationY = AMPLITUDE * float(sin(2 * M_PI - WAVE_SPEED * waveMovementInPercent));

	// Calculate final drawing position
	float posX = m_Position.x - m_WIDTH / 2;
	float posY = m_Position.y - m_HEIGHT / 2 - deviationY;

	// Set destination rectangle
	m_DstRect = Rectf{ posX, posY, m_WIDTH, m_HEIGHT };

}


void ShieldBearer::CheckPlayerCollision(Player* player)
{
	if (utils::IsOverlapping(m_CollisionRect, player->GetCollisionRect()))
	{
		player->TakeDmg(1);
	}
}

void ShieldBearer::OnDeath(UImanager* UI)
{
	UI->AddPoints(m_Points);
	m_IsAbleToMove = false;
	m_CurrentFrameNr = 0;

	m_pDeathEffect->Play(0);

	delete m_pTexture;
	m_pTexture = new Texture("Enemies/deathAnimation.png");

	m_DstRect = Rectf{ m_DstRect.left - 20.f, m_DstRect.bottom, m_WIDTH * 2, m_HEIGHT };
}

void ShieldBearer::UpdateCurrentFrameNr() noexcept
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

void ShieldBearer::UpdateSrcRect() noexcept
{
	if (m_pHp->IsDead())
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 4, m_pTexture->GetHeight(), m_pTexture->GetWidth() / 4, m_pTexture->GetHeight() / 2 };
	}
	else
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_pTexture->GetWidth() / 4, m_pTexture->GetHeight(), m_pTexture->GetWidth() / 4, m_pTexture->GetHeight() };
	}
}

