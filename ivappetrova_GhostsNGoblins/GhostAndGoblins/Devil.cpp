#include "pch.h"
#include "Devil.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>
#include "SoundEffect.h"

#include "Player.h"
#include "HUDmanager.h"
#include "Health.h"
#include "EnemyBullet.h"


Devil::Devil(const Vector2f& position, const float playerPosX, const std::vector<std::vector<Vector2f>>& levelVertices) :
	Enemy(3, position, 1000),
	m_pTexture(new Texture("Enemies/devil.png")),
	m_FrameWidth{ m_pTexture->GetWidth() / 3 },
	m_FrameHeight{ m_pTexture->GetHeight() / 7},
	m_CurrentFrameNr{},
	m_AccTime{},
	m_SPEED{ 160.f },
	m_EnemyState{ State::inactive },
	m_DirectionX{ -1 },
	m_VelocityY{},
	m_PlayerPosX{ playerPosX },
	m_IsFlipSprite{ false },
	m_IsOnGround{ false },
	m_NewActionNeeded{ true },
	m_RandomNumberAction{ },
	m_ACTION_COOLDOWN{ 2.f },
	m_ActionTimer{},
	m_LevelVertices{ levelVertices },
	m_IsActive{ false },
	m_IsDead{ false },
	m_TAKE_DMG_DURATION{0.1f},
	m_TakeDmgTimer{},
	m_IsTakingDamage{false}
{
	m_pBullets.reserve(3);

	UpdateDstRect();
	UpdateSrcRect();
	UpdateCollisionRect();

	ResetStates();
}

Devil::~Devil() noexcept
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
void Devil::Draw() const
{
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

	/*utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::DrawRect(m_DstRect);


	utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	utils::DrawRect(Enemy::GetCollisionRect());*/
}

void Devil::FlipSprite() const noexcept
{
	glTranslatef(m_Position.x + m_FrameWidth, m_Position.y, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-(m_Position.x + m_FrameWidth ), -m_Position.y, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Devil::Update(float elapsedSec, Player* player, HUDmanager* HUD)
{
	Enemy::Update(elapsedSec, player, HUD);

	if (player->GetPosition().x > this->m_Position.x -290.f && !m_IsActive)
	{
		m_IsActive = true;
		m_EnemyState = State::goingUp;
		std::cout << "devil is active" << std::endl;
	}

	if (m_EnemyState == State::dead && m_HasPlayedDeathAnim)
	{
		return;
	}

	if (m_IsTakingDamage)
	{
		m_TakeDmgTimer += elapsedSec;
		if (m_TakeDmgTimer >= m_TAKE_DMG_DURATION)
		{
			m_IsTakingDamage = false;
			m_EnemyState = State::takeDmg;
		}
		UpdateSrcRect();
		UpdateCurrentFrameNr(); 
		return;
	}

	if (m_IsActive)
	{
		TransitionToActive();

		m_AccTime += elapsedSec;
		if (!m_NewActionNeeded && m_EnemyState != State::dead)
		{
			if (m_ActionTimer >= m_ACTION_COOLDOWN)
			{
				m_ActionTimer = 0;
				ResetStates();
				m_NewActionNeeded = true;
			}
			m_ActionTimer += elapsedSec;
		}

		UpdateDirection(player->GetPosition().x);
		UpdateDstRect();
		UpdateSrcRect();
		UpdateCollisionRect();

		if (m_EnemyState != State::dead )
		{
			HandleGroundCollision();
			ApplyGravity(elapsedSec);
		}

		if (m_NewActionNeeded && m_EnemyState != State::dead)
		{
			PickRandomAction();
			m_NewActionNeeded = false;
		}


		HandleRandomAction(elapsedSec, player);

		if (m_pHp->IsDead() && !m_HasHandledDeath)
		{
			OnDeath(HUD);
			m_HasHandledDeath = true;
			m_NewActionNeeded = false;

			return;
		}

		//CheckPlayerCollision(player);
		UpdateCurrentFrameNr();

		UpdateBullets(elapsedSec, player);
	}
}

void Devil::TransitionToActive()
{
	if (m_EnemyState == State::goingUp && m_CurrentFrameNr >= 2)
	{
		m_EnemyState = State::idle;
		m_NewActionNeeded = true;
		m_CurrentFrameNr = 0;
		m_AccTime = 0.f;
	}
}

void Devil::UpdateDstRect()
{
	if (m_EnemyState == State::dead)
	{
		m_DstRect = Rectf{ m_Position.x , m_Position.y, 80.f, 80.f };
	}
	else
	{
		m_DstRect = Rectf{ m_Position.x, m_Position.y, 100.f, 90.f };
	}
}

void Devil::UpdateSrcRect() noexcept
{
	switch (m_EnemyState)
	{
	case State::inactive:
		m_SrcRect = Rectf{ 0.f, 0.f, m_FrameWidth, m_FrameHeight };
		break;
	case State::takeDmg:
		m_SrcRect = Rectf{ 0.f , 6 * m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::goingUp:
		m_SrcRect = Rectf{ 0.f, m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::idle:
		m_SrcRect = Rectf{ 2 * m_FrameWidth, m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::move:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 2 * m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::fly:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 3 * m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::shootDown:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 4 * m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::shootFlying:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 5 * m_FrameHeight, m_FrameWidth, m_FrameHeight };
		break;
	case State::dead:
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, m_pTexture->GetHeight(), m_FrameWidth, m_pTexture->GetHeight() / 2 };
		break;
	}
}

void Devil::UpdateDirection(const float playerPosX)
{
	if (m_Position.x < playerPosX)
	{
		m_DirectionX = 1.f;
		m_IsFlipSprite = true;
	}
	else
	{
		m_DirectionX = -1.f;
		m_IsFlipSprite = false;
	}
}

void Devil::UpdateCollisionRect()
{
	if (m_EnemyState != State::dead)
	{
		m_CollisionRect = Rectf{ m_Position.x, m_Position.y, 90.f, 80.f };
	}
	else
	{
		m_CollisionRect = Rectf{ 0.f, -700.f, 0.f, 0.f };
	}
}

void Devil::HandleGroundCollision()
{
	utils::HitInfo hitinfo{};
	m_IsOnGround = false;

	Vector2f velocity{ 0.f, m_VelocityY };

	// Check collision with left side of the rect
	Vector2f leftStart{ m_CollisionRect.left + 5.f, m_CollisionRect.bottom };
	Vector2f leftEnd{ m_CollisionRect.left + 5.f, m_CollisionRect.bottom + m_CollisionRect.height };

	// Check collision with vertical mid of the rect
	Vector2f midVStart{ m_CollisionRect.left + m_CollisionRect.width / 2, m_CollisionRect.bottom };
	Vector2f midVEnd{ m_CollisionRect.left + m_CollisionRect.width / 2, m_CollisionRect.bottom + m_CollisionRect.height };

	// Check collision with right side of the rect
	Vector2f rightStart{ m_CollisionRect.left + m_CollisionRect.width - 5.f, m_CollisionRect.bottom };
	Vector2f rightEnd{ m_CollisionRect.left + m_CollisionRect.width - 5.f, m_CollisionRect.bottom + m_CollisionRect.height };


	////////////////////////////////////////////////////////////////////////////////////////////////////////// floor
	for (int index{}; index < m_LevelVertices.size(); ++index)
	{

		// check for bot/top collision
		if (utils::Raycast(m_LevelVertices[index], leftStart, leftEnd, hitinfo)
			|| utils::Raycast(m_LevelVertices[index], midVStart, midVEnd, hitinfo)
			|| utils::Raycast(m_LevelVertices[index], rightStart, rightEnd, hitinfo))
		{

			if (velocity.DotProduct(hitinfo.normal) < 0)
			{
				// floor
				if (velocity.y < 0)
				{
					m_Position.y = hitinfo.intersectPoint.y + 1.f;
					m_VelocityY = 0.f;
					m_IsOnGround = true;
					m_HasFlew = false;

					if (m_EnemyState == State::fly || m_EnemyState == State::shootFlying)
					{
						m_EnemyState = State::idle;
					}
				}
			}
		}
	}
}

void Devil::ApplyGravity(float elapsedSec) noexcept
{
	const float GRAVITY{ -980.f };
	m_VelocityY += GRAVITY * elapsedSec;

	m_Position.y += m_VelocityY * elapsedSec;
}

void Devil::PickRandomAction()
{
	m_RandomNumberAction = rand() % 4 + 3;
	std::cout << "number: " << m_RandomNumberAction << std::endl;
}

void Devil::HandleRandomAction(float elapsedSec, Player* player)
{
	if (m_EnemyState == State::dead) return;

	if (m_RandomNumberAction == 3 && !m_HasMoved)
	{
		Move(elapsedSec);
	}

	if (m_RandomNumberAction == 4 && !m_HasFlew)
	{
		Fly(elapsedSec);
	}

	if (m_RandomNumberAction == 5 && !m_HasShot)
	{
		Shoot(player);
	}

	if (m_RandomNumberAction == 6 && !m_HasFlew && !m_HasShot)
	{
		Fly(elapsedSec);
		Shoot(player);
	}

}

void Devil::Move(float elapsedSec)
{
	m_Position.x += m_DirectionX * m_SPEED * elapsedSec;
	m_EnemyState = State::move;
}

void Devil::Fly(float elapsedSec)
{
	const float FLY_HEIGHT{ 700.f };

	if (m_IsOnGround)
	{
		m_VelocityY = FLY_HEIGHT;
		m_CurrentFrameNr = 0;
		m_IsOnGround = false;
		m_HasFlew = true;

		m_EnemyState = State::fly;
	}
}

void Devil::Shoot(Player* player)
{
	m_pBullets.push_back(new EnemyBullet(Vector2f{ m_Position.x, m_Position.y - 30.f }, player->GetPosition(), 1, 2));
	if (m_HasFlew)
	{
		m_EnemyState = State::shootFlying;
	}
	else
	{
	m_EnemyState = State::shootDown;
	}
	m_HasShot = true;
}

void Devil::ResetStates()
{
	m_HasShot = false;
	m_HasFlew = false;
	m_HasMoved = false;
}

void Devil::OnDeath(HUDmanager* HUD)
{
	if (m_IsDead) return;
	m_IsDead = true;

	HUD->AddPoints(m_Points);

	m_EnemyState = State::dead;
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

	m_FrameWidth = m_pTexture->GetWidth() / 4;
}

void Devil::CheckPlayerCollision(Player* player)
{
	if (m_EnemyState == State::dead) return;

	if (utils::IsOverlapping(m_CollisionRect, player->GetCollisionRect()))
	{
		player->TakeDmg(1);
	}
}

void Devil::UpdateCurrentFrameNr() noexcept
{
	switch (m_EnemyState)
	{
	case State::inactive:
	case State::idle:
	case State::takeDmg:
		m_AccTime = 0;
		m_CurrentFrameNr = 0;
		break;

	case State::goingUp:
		if (m_AccTime >= 0.25f)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 3;
		}
		break;

	case State::move:
	case State::fly:
	case State::shootDown:
	case State::shootFlying:
		if (m_AccTime >= 0.2f)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 2;
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

void Devil::UpdateBullets(float elapsedSec, Player* player)
{
	if (m_EnemyState == State::dead) return;

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

void Devil::TakeDamage(int amount)
{
	Enemy::TakeDamage(amount);
	m_EnemyState = State::takeDmg;
	m_TakeDmgTimer = 0.f;
	m_IsTakingDamage = true;
}

const Vector2f& Devil::GetPos() const noexcept
{
	return m_Position;
}
