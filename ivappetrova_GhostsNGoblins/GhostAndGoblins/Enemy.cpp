#include "pch.h"
#include "Enemy.h"
#include "SoundEffect.h"
#include "Health.h"
#include "EnemyCollectable.h"

Enemy::Enemy(const int Hp, const Vector2f& position, int points):
	m_pHp{new Health(Hp)},
	m_Position{position},
	m_HasPlayedDeathAnim{false},
	m_Points{points},
	m_HasHandledDeath{false},
	m_pDeathEffect{ new SoundEffect("Sounds/Effects/enemyDeath.wav") }
{
}

Enemy::~Enemy() noexcept
{
	delete m_pHp;
	m_pHp = nullptr;


	delete m_pDeathEffect;
	m_pDeathEffect = nullptr;
}

void Enemy::Update(float elapsedSec, Player* player, UImanager* UI)
{
	m_pDeathEffect->SetVolume(20);
}

void Enemy::TakeDamage(int amount)
{
	m_pHp->TakeDMG(amount);
}

Collectable* Enemy::DropCollectable() const
{
	const int CHANCE = rand() % 101;
	// 10% drop chance
	if (CHANCE <= 10) 
	{
		return new EnemyCollectable(m_Position);
	}
	return nullptr;
}

const bool Enemy::IsDead() const noexcept
{
	return m_pHp->IsDead();
}

const bool Enemy::HasFinishedDeathAnim() const noexcept
{
	return m_HasPlayedDeathAnim;
}

const Rectf& Enemy::GetCollisionRect() const noexcept
{
	return m_CollisionRect;
}

