#pragma once
#include "Collectable.h"

class Texture;

class BallCollectable final : public Collectable
{
public:
	explicit BallCollectable(const Vector2f& position);
	virtual ~BallCollectable() noexcept;

	void Update(float elapsedSec) override;
	void Draw() const override;
	const Rectf& GetCollisionRect() const override;

	// Rule of 5
	BallCollectable(const BallCollectable& theOneIcopy) = delete;
	BallCollectable(BallCollectable&& theOneImove) = delete;
	BallCollectable& operator= (const BallCollectable& theOneIcopy) = delete;
	BallCollectable& operator= (BallCollectable&& theOneImove) = delete;

private:
	Texture* m_pTexture;
	Rectf m_CollisionRect;

	float m_FrameTime;
	float m_AccTime;
	int m_CurrentFrameNr;
	float m_SrcRectX;

	bool m_IsCollected;
	bool m_TransitionEnded;
	float m_VelocityY;

	void SetCollisionRect() noexcept;

	void FallFromSky(float elapsedSec) noexcept;
	void SwitchToBall();
	void UpdateCurrFrameNumber(float elapsedSec);
};

