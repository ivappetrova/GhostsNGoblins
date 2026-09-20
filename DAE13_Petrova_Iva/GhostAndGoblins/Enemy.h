#pragma once
class SoundEffect;

class Health;
class UImanager;
class Player;
class Collectable;

class Enemy
{
public:
	explicit Enemy(const int Hp, const Vector2f& position, int points);
	virtual ~Enemy() noexcept;

	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec, Player* player, UImanager* UI);

	// to delete the object when the animation is done
	virtual const bool IsDead() const noexcept;
	virtual const bool HasFinishedDeathAnim() const noexcept;
	virtual const Rectf& GetCollisionRect() const noexcept;
	virtual void TakeDamage(int amount);

	virtual Collectable* DropCollectable() const;

	// Rule of 5
	Enemy(const Enemy& theOneIcopy) = delete;
	Enemy(Enemy&& theOneImove) = delete;
	Enemy& operator= (const Enemy& theOneIcopy) = delete;
	Enemy& operator= (Enemy&& theOneImove) = delete;

protected:
	// functions
	virtual void UpdateCollisionRect() = 0;
	virtual void CheckPlayerCollision(Player* player) = 0;
	// members
	Health* m_pHp;
	Vector2f m_Position;
	bool m_HasPlayedDeathAnim;
	Rectf m_CollisionRect;
	int m_Points;
	bool m_HasHandledDeath;

	SoundEffect* m_pDeathEffect;
};




