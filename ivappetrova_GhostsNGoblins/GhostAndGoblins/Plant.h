#pragma once
#include "Enemy.h"
#include <vector>

class Texture;
class Player;
class HUDmanager;
class EnemyBullet;

class Plant final : public Enemy
{
public:
	explicit Plant(const Vector2f& position, const Vector2f& playerPos);
	virtual ~Plant() noexcept;

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Player* player, HUDmanager* HUD) override;
	virtual void TakeDamage(int amount) override;

	// Rule of 5
	Plant(const Plant& theOneIcopy) = delete;
	Plant(Plant&& theOneImove) = delete;
	Plant& operator= (const Plant& theOneIcopy) = delete;
	Plant& operator= (Plant&& theOneImove) = delete;


private:
	//// functions: 

	//overriden
	virtual void UpdateCollisionRect() override;
	virtual void CheckPlayerCollision(Player* player) override;

	void UpdateDirection(const float playerPosXInEnemyInitTime);

	// deletes the old enemy texture to load the flame death effect
	void OnDeath(HUDmanager* HUD);

	// draw
	void FlipSprite() const noexcept;
	void UpdateDstRect();
	void UpdateSrcRect();

	//update
	void CheckIfAbleToShoot(const float playerX);
	void UpdateCurrentFrameNr();
	void CheckFlipSprite() noexcept;
	void Shoot(Player* player);
	void UpdateBullets(float elapsedSec, Player* player);


	//// data members
	Texture* m_pTexture;
	const float m_FRAME_WIDTH;
	int m_CurrentFrameNr;
	float m_AccTime;

	Rectf m_SrcRect;
	Rectf m_DstRect;
	float m_PlayerCurrentDirectionX;

	bool m_IsFlipSprite;
	bool m_HasShot;
	bool m_IsAbleToShoot;

	std::vector<EnemyBullet*> m_pBullets;
};