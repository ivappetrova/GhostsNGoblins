#pragma once
#include "Enemy.h"

class Texture;

class Player;
class HUDmanager;

class Bird final : public Enemy
{
public:
	explicit Bird(const Vector2f& position);
	virtual ~Bird() noexcept;

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Player* player, HUDmanager* HUD) override;
	virtual void TakeDamage(int amount) override;

	// Rule of 5
	Bird(const Bird& theOneIcopy) = delete;
	Bird(Bird&& theOneImove) = delete;
	Bird& operator= (const Bird& theOneIcopy) = delete;
	Bird& operator= (Bird&& theOneImove) = delete;

private:
	/////// functions
	//overriden
	virtual void UpdateCollisionRect() override;
	virtual void CheckPlayerCollision(Player* player) override;

	//update
	void UpdateDstRect() noexcept;
	void CheckIfAbleToMove(const float playerX);
	void Fly(float elapsedSec) noexcept;
	void OnDeath(HUDmanager* HUD);
	void UpdateCurrentFrameNr() noexcept; 
	void UpdateSrcRect() noexcept;

	// data members
	Texture* m_pTexture;
	float m_FrameWidth;
	int m_CurrentFrameNr;
	float m_AccTime;

	Rectf m_DstRect;
	Rectf m_SrcRect;
	const float m_SPEED;
	bool m_IsAbleToFly;
	float m_DeathAnimAccTime{};

	enum class State
	{
		sit, fly, dead
	};

	State m_EnemyState;
};