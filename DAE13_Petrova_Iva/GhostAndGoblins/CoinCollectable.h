#pragma once
#include "Collectable.h"

class Texture;

class CoinCollectable final: public Collectable 
{
public:
	CoinCollectable(const Vector2f& position);
	virtual ~CoinCollectable() noexcept;

	void Update(float elapsedSec) override;
	void Draw() const override;
	const Rectf& GetCollisionRect() const override;

	// Rule of 5
	CoinCollectable(const CoinCollectable& theOneIcopy) = delete;
	CoinCollectable(CoinCollectable&& theOneImove) = delete;
	CoinCollectable& operator= (const CoinCollectable& theOneIcopy) = delete;
	CoinCollectable& operator= (CoinCollectable&& theOneImove) = delete;

private:
	Texture* m_pTexture;
	Rectf m_CollisionRect;

	float m_FrameTime;
	float m_AccTime;
	int m_CurrentFrameNr;
	const float m_FRAME_WIDTH;
	const float	m_FRAME_HEIGHT;
	float m_SrcRectX;

	bool m_IsCollected;

	void SetCollisionRect() noexcept;
};

