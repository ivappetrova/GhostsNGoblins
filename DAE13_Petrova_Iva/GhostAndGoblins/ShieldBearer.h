#pragma once
#include "Enemy.h"

class Texture;
class SoundEffect;

class Player;
class UImanager;

class ShieldBearer final : public Enemy
{
public:
	explicit ShieldBearer(const Vector2f& position);
	virtual ~ShieldBearer() noexcept;

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Player* player, UImanager* UI) override;
	const Rectf GetShieldRect() const noexcept;

	// Rule of 5
	ShieldBearer(const ShieldBearer& theOneIcopy) = delete;
	ShieldBearer(ShieldBearer&& theOneImove) = delete;
	ShieldBearer& operator= (const ShieldBearer& theOneIcopy) = delete;
	ShieldBearer& operator= (ShieldBearer&& theOneImove) = delete;

private:
	/////// functions
	//overriden
	virtual void UpdateCollisionRect() override;
	virtual void CheckPlayerCollision(Player* player) override;

	//update
	void CheckIfAbleToMove(const float playerX);
	void Move(float elapsedSec) noexcept;
	void OnDeath(UImanager* UI);
	void UpdateCurrentFrameNr() noexcept;
	void UpdateSrcRect() noexcept;
	void UpdateShieldRect() noexcept;

	// data members
	Texture* m_pTexture;
	const float	m_WIDTH;
	const float	m_HEIGHT;
	int m_CurrentFrameNr;
	float m_AccTime;

	bool m_IsAbleToMove;
	float m_TickCounter;

	Rectf m_DstRect;
	Rectf m_SrcRect;
	Rectf m_ShieldRect;

	SoundEffect* m_pMoveEffect;
};
