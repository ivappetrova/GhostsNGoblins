#include "pch.h"
#include "Boss.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>
#include "SoundEffect.h"

#include "Player.h"
#include "UImanager.h"
#include "Health.h"
#include "EnemyBullet.h"


Boss::Boss(const Vector2f& position, const float playerPosX, const std::vector<std::vector<Vector2f>>& levelVertices):
	Enemy(10, position, 2000),
	m_pTexture(new Texture("Enemies/boss.png")),
	m_FrameWidth{ m_pTexture->GetWidth() / 3 },
	m_CurrentFrameNr{},
	m_AccTime{},
	m_SPEED{ 100.f },
	m_EnemyState{ State::idle },
	m_DirectionX{-1}, 
	m_VelocityY{},
	m_PlayerPosX{playerPosX},
	m_IsFlipSprite{false},
	m_IsOnGround{false},
	m_NewActionNeeded{true},
	m_RandomNumberAction{0},
	m_ACTION_COOLDOWN{2.f},
	m_ActionTimer{},
	m_LevelVertices{levelVertices},
	m_IsActive{false},
	m_IsDead{false}
{
	m_pBullets.reserve(3);

	UpdateDstRect();
	UpdateSrcRect();
	UpdateCollisionRect();

	ResetStates();
}

Boss::~Boss() noexcept
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
void Boss::Draw() const
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

	/*utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	utils::DrawRect(Enemy::GetCollisionRect());*/
}

void Boss::FlipSprite() const noexcept
{
	glTranslatef(m_Position.x + m_FrameWidth, m_Position.y, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-(m_Position.x + m_FrameWidth +75.f), -m_Position.y, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update(float elapsedSec, Player* player, UImanager* UI)
{
	Enemy::Update(elapsedSec, player, UI);

	if (player->GetPosition().x > 7000 && !m_IsActive)
	{
		m_IsActive = true;
		std::cout << "boss is active" << std::endl;
	}

	if (m_EnemyState == State::dead && m_HasPlayedDeathAnim)
	{
		return;
	}

	if (m_IsActive)
	{
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

		if (m_EnemyState != State::dead)
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
			OnDeath(UI);
			m_HasHandledDeath = true;
			m_NewActionNeeded = false;

			return;
		}

		CheckPlayerCollision(player);
		UpdateCurrentFrameNr();

		UpdateBullets(elapsedSec, player);
	}
}

void Boss::UpdateDstRect()
{
	if (m_pHp->IsDead())
	{
		m_DstRect = Rectf{ m_Position.x , 70.f, 150.f, 150.f };
	}
	else
	{
		m_DstRect = Rectf{ m_Position.x, m_Position.y, m_FrameWidth * 3.f, m_pTexture->GetHeight() /4 * 3.f };
	}
}

void Boss::UpdateSrcRect() noexcept
{
	if (m_pHp->IsDead())
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, m_pTexture->GetHeight(), m_FrameWidth, m_pTexture->GetHeight()/2};
	}
	else if (m_EnemyState == State::idle)
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 0.f, m_FrameWidth, m_pTexture->GetHeight() / 4};
	}
	else if (m_EnemyState == State::move)
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, m_pTexture->GetHeight() / 4, m_FrameWidth, m_pTexture->GetHeight() / 4 };
	}
	else if (m_EnemyState == State::jump)
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 2*m_pTexture->GetHeight() / 4, m_FrameWidth, m_pTexture->GetHeight() / 4 };
	}
	else if (m_EnemyState == State::shoot)
	{
		m_SrcRect = Rectf{ m_CurrentFrameNr * m_FrameWidth, 3*m_pTexture->GetHeight() / 4, m_FrameWidth, m_pTexture->GetHeight() / 4 };
	}
}

void Boss::UpdateDirection(const float playerPosX)
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

void Boss::UpdateCollisionRect()
{
	if (m_EnemyState != State::dead)
	{
		m_CollisionRect = Rectf{ m_Position.x + 20.f, m_Position.y, m_FrameWidth * 3.f - 20.f, m_pTexture->GetHeight() / 4 * 3.f };
	}
	else
	{
		m_CollisionRect = Rectf{ 0.f, -200.f, 0.f, 0.f };
	}
}

void Boss::HandleGroundCollision()
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
					m_HasJumped = false;

					if (m_EnemyState == State::jump)
					{
						m_EnemyState = State::idle;
					}
				}
			}
		}
	}
}

void Boss::ApplyGravity(float elapsedSec) noexcept
{
	const float GRAVITY{ -980.f };
	m_VelocityY += GRAVITY * elapsedSec;

	m_Position.y += m_VelocityY * elapsedSec;
}

void Boss::PickRandomAction()
{
	m_RandomNumberAction = rand() % 3 + 1;
}

void Boss::HandleRandomAction(float elapsedSec, Player* player)
{
	if (m_EnemyState == State::dead) return;


	if (m_RandomNumberAction == 1 && !m_HasMoved)
	{
		Move(elapsedSec);
	}

	if (m_RandomNumberAction == 2 && !m_HasJumped)
	{
		Jump(elapsedSec);
	}

	if (m_RandomNumberAction == 3 && !m_HasShot)
	{
		Shoot(player);
	}
}

void Boss::Move(float elapsedSec)
{
	m_Position.x += m_DirectionX * m_SPEED * elapsedSec;
	m_EnemyState = State::move;
}

void Boss::Jump(float elapsedSec)
{
	const float JUMP_SPEED{ 500.f };

	if (m_IsOnGround)
	{
		m_VelocityY = JUMP_SPEED;
		m_CurrentFrameNr = 0;
		m_IsOnGround = false;
		m_HasJumped = true;

		m_EnemyState = State::jump;
	}
}

void Boss::Shoot(Player* player)
{
	m_pBullets.push_back(new EnemyBullet(Vector2f{ m_Position.x, m_Position.y - 30.f }, player->GetPosition(), 1, 2));
	m_EnemyState = State::shoot;
	m_HasShot = true;
}

void Boss::ResetStates()
{
	m_HasShot = false;
	m_HasJumped = false;
	m_HasMoved = false;
}

void Boss::OnDeath(UImanager* UI)
{
	if (m_IsDead) return;
	m_IsDead = true;

	UI->AddPoints(m_Points);

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

void Boss::CheckPlayerCollision(Player* player)
{
	if (m_EnemyState == State::dead) return;

	if (utils::IsOverlapping(m_CollisionRect, player->GetCollisionRect()))
	{
		player->TakeDmg(1);
	}
}

void Boss::UpdateCurrentFrameNr() noexcept
{
	switch (m_EnemyState)
	{
	case State::idle:
	case State::shoot:
		m_AccTime = 0;
		m_CurrentFrameNr = 0;
		break;

	case State::move:
		if (m_AccTime >= 0.2f)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 3;
		}
		break;

	case State::jump:
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

void Boss::UpdateBullets(float elapsedSec, Player* player)
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

void Boss::TakeDamage(int amount)
{
	Enemy::TakeDamage(amount);
}

const Vector2f& Boss::GetPos() const noexcept
{
	return m_Position;
}
