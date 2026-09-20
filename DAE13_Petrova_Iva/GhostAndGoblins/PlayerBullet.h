#pragma once
#include <vector>

class Texture;
class Enemy;

class PlayerBullet final
{
public:
	explicit PlayerBullet(const Vector2f& position, float direction);
	~PlayerBullet() noexcept; 

	void Draw() const;
	void SetCollisionRect() noexcept;
	void Update(float elapsedSec, const std::vector< std::vector<Vector2f> >& levelVertices,
				std::vector<Enemy*>& enemies);

	bool BulletLifeTimeEnded() const noexcept;  
	bool CollidesWithLevel() const noexcept;
	bool CollidesWithEnemy() const noexcept;

	// Rule of 5
	PlayerBullet(const PlayerBullet& theOneIcopy) = delete;
	PlayerBullet(PlayerBullet&& theOneImove) = delete;
	PlayerBullet& operator= (const PlayerBullet& theOneIcopy) = delete;
	PlayerBullet& operator= (PlayerBullet&& theOneImove) = delete;

private:
	void FlipSprite() const noexcept;
	void CheckLevelCollision(const std::vector< std::vector<Vector2f> >& levelvertices);
	void CheckEnemyCollision(std::vector<Enemy*>& enemies);

	Vector2f m_Position;
	Texture* m_pBulletTexture;
	Rectf m_CollisionRect;
	const float m_SPEED;
	float m_Lifetime;       
	const float m_MAX_LIFETIME; 
	float m_Direction;
	bool m_IsFlipSprite;
	bool m_CollidesWithLevel;
	bool m_CollidesWithEnemy;
};

