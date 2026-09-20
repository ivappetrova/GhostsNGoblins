#pragma once
#include "Collectable.h"

class Texture;

class EnemyCollectable final : public Collectable
{
public:
	explicit EnemyCollectable(Vector2f position);
	virtual ~EnemyCollectable() noexcept;

	void Update(float elapsedSec) override;
	void Draw() const override;
	const Rectf& GetCollisionRect() const override;

	// Rule of 5
	EnemyCollectable(const EnemyCollectable& theOneIcopy) = delete;
	EnemyCollectable(EnemyCollectable&& theOneImove) = delete;
	EnemyCollectable& operator= (const EnemyCollectable& theOneIcopy) = delete;
	EnemyCollectable& operator= (EnemyCollectable&& theOneImove) = delete;

private:
	Texture* m_pTexture;
	Rectf m_CollisionRect;

	float m_FrameTime;
	float m_AccTime;
	int m_CurrentFrameNr;
	float m_SrcRectX;
	float m_VelocityY;

	bool m_IsCollected;
	bool m_BossIsDead;

	void SetCollisionRect() noexcept;
	void FallFromSky(float elapsedSec) noexcept;
	void UpdateCurrFrameNumber(float elapsedSec) noexcept;

};