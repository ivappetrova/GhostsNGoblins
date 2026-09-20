#pragma once
class Collectable
{
public:
	explicit Collectable(const Vector2f& position, const int points, const bool isBagOpen = true );
	virtual ~Collectable() = default;

	virtual void Update(float elapsedSec) = 0;
	virtual void Draw() const = 0;
	virtual const Rectf& GetCollisionRect() const = 0; 

	virtual const int GetPoints() const;

	// Rule of 5
	Collectable(const Collectable& theOneIcopy) = delete;
	Collectable(Collectable&& theOneImove) = delete;
	Collectable& operator= (const Collectable& theOneIcopy) = delete;
	Collectable& operator= (Collectable&& theOneImove) = delete;

protected:
	Vector2f m_Position;
	bool m_IsBagOpen;
	int m_Points;

private:

};

