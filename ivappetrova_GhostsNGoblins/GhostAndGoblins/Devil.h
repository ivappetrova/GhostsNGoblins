#pragma once
#include "Enemy.h"
#include <vector>
class Texture;
class EnemyBullet;

class Devil final : public Enemy
{
public:
	explicit Devil(const Vector2f& position, const float playerPosX, const std::vector<std::vector<Vector2f>>& levelVertices);
	~Devil() noexcept;

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Player* player, HUDmanager* HUD) override;
	virtual void TakeDamage(int amount) override;

	const Vector2f& GetPos() const noexcept;

	// Rule of 5
	Devil(const Devil& theOneIcopy) = delete;
	Devil(Devil&& theOneImove) = delete;
	Devil& operator= (const Devil& theOneIcopy) = delete;
	Devil& operator= (Devil&& theOneImove) = delete;

private:
	// draw
	void FlipSprite() const noexcept;

	//update
	void TransitionToActive();
	void UpdateDstRect();
	void UpdateSrcRect() noexcept;
	void UpdateDirection(const float playerPosX);
	virtual void UpdateCollisionRect() override;
	void HandleGroundCollision();
	void ApplyGravity(float elapsedSec) noexcept;

	void PickRandomAction();
	void HandleRandomAction(float elapsedSec, Player* player);
	void Move(float elapsedSec);
	void Fly(float elapsedSec);
	void Shoot(Player* player);


	void OnDeath(HUDmanager* HUD);
	virtual void CheckPlayerCollision(Player* player) override;
	void UpdateCurrentFrameNr() noexcept;
	void UpdateBullets(float elapsedSec, Player* player);

	// data members
	Texture* m_pTexture;
	float m_FrameWidth;
	float m_FrameHeight;
	int m_CurrentFrameNr;
	float m_AccTime;

	const float m_SPEED;
	float m_DirectionX;
	float m_PlayerPosX;
	float m_VelocityY;

	Rectf m_DstRect;
	Rectf m_SrcRect;

	enum class State
	{
		inactive, goingUp, idle, move = 3, fly = 4, shootDown = 5, shootFlying= 6, takeDmg, dead
	};

	State m_EnemyState;

	int m_RandomNumberAction;

	//bool states
	bool m_IsFlipSprite;
	bool m_HasShot;
	bool m_HasFlew;
	bool m_HasMoved;
	bool m_IsOnGround;
	bool m_IsActive;
	bool m_IsDead;

	void ResetStates();

	bool m_NewActionNeeded;
	const float m_ACTION_COOLDOWN;
	float m_ActionTimer;

	float m_TakeDmgTimer;
	const float m_TAKE_DMG_DURATION; 
	bool m_IsTakingDamage;

	std::vector<EnemyBullet*> m_pBullets;
	const std::vector<std::vector<Vector2f>> m_LevelVertices;
};


