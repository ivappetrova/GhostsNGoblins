#pragma once
#include <vector>

class Texture;
class Player;

class EnemyBullet final
{
public:
	explicit EnemyBullet(const Vector2f& position, const Vector2f& playerPos, int bulletSrcRectY, int bulletSrcRectFrames);
	~EnemyBullet() noexcept;

	void Draw() const;
	void Update(float elapsedSec, Player* player);

	bool BulletLifeTimeEnded() const noexcept;
	bool CollidesWithPlayer() const noexcept;

	// Rule of 5
	EnemyBullet(const EnemyBullet& theOneIcopy) = delete;
	EnemyBullet(EnemyBullet&& theOneImove) = delete;
	EnemyBullet& operator= (const EnemyBullet& theOneIcopy) = delete;
	EnemyBullet& operator= (EnemyBullet&& theOneImove) = delete;

private:
	void SetDirection(const Vector2f& playerPos);
	void SetCollisionRect() noexcept;

	void CheckPlayerCollision(Player* player);
	void ChangeCurrentFrame();

	Vector2f m_Position;
	Vector2f m_Direction;
	Texture* m_pBulletTexture;
	int m_BulletSrcRectY;
	int m_BulletSrcRectFrames;
	Rectf m_CollisionRect;
	const float m_SPEED;
	float m_Lifetime;
	const float m_MAX_LIFETIME;
	bool m_CollidesWithPlayer;
	float m_AccTime;
	int m_CurrentFrameNr;


};

