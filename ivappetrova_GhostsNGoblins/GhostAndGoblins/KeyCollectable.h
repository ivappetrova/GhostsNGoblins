#pragma once
#include "Collectable.h"

class Texture;
class Door;

class KeyCollectable final: public Collectable
{
public:
	explicit KeyCollectable(Door* doorPtr);
	virtual ~KeyCollectable() noexcept;

	void Update(float elapsedSec) override;
	void Draw() const override;
	const Rectf& GetCollisionRect() const override;

	void OpenDoor();

	// Rule of 5
	KeyCollectable(const KeyCollectable& theOneIcopy) = delete;
	KeyCollectable(KeyCollectable&& theOneImove) = delete;
	KeyCollectable& operator= (const KeyCollectable& theOneIcopy) = delete;
	KeyCollectable& operator= (KeyCollectable&& theOneImove) = delete;

private:
	Texture* m_pTexture;
	Rectf m_CollisionRect;

	float m_FrameTime;
	float m_AccTime;
	int m_CurrentFrameNr;
	const float m_FRAME_WIDTH;
	const float	m_FRAME_HEIGHT;
	float m_SrcRectX;
	float m_VelocityY;

	bool m_IsCollected;
	bool m_BossIsDead;

	void SetCollisionRect() noexcept;
	void FallFromSky(float elapsedSec) noexcept;
	void UpdateCurrFrameNumber(float elapsedSec) noexcept;

	Door* m_pDoor;

};

