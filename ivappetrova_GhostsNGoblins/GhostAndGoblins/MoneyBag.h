#pragma once
#include "Collectable.h"

class Texture;

class MoneyBag final : public Collectable
{
public:
	MoneyBag(const Vector2f& position);
	~MoneyBag() noexcept;

	void Update(float elapsedSec) override;
	void Draw() const override;
	const Rectf& GetCollisionRect() const override;

	// Rule of 5
	MoneyBag(const MoneyBag& theOneIcopy) = delete;
	MoneyBag(MoneyBag&& theOneImove) = delete;
	MoneyBag& operator= (const MoneyBag& theOneIcopy) = delete;
	MoneyBag& operator= (MoneyBag&& theOneImove) = delete;

private:
	Texture* m_pTexture;
	Rectf m_CollisionRect;

	bool m_IsCollected;

	void SetCollisionRect() noexcept;
};

