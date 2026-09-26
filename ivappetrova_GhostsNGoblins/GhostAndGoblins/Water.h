#include <vector>
class Texture;

class Water final
{
public:
	explicit Water(const Vector2f& position, const float width);
	~Water() noexcept;

	void Draw() const;
	void Update(float elapsedSec);
	Rectf GetCollisionRect() const noexcept;

	// Rule of 5
	Water(const Water& theOneIcopy) = delete;
	Water(Water&& theOneImove) = delete;
	Water& operator= (const Water& theOneIcopy) = delete;
	Water& operator= (Water&& theOneImove) = delete;


private:
	// data members
	Vector2f m_Position;
	Texture* m_pTexture;

	const float m_SPEED;
	const float m_WIDTH;
	const float m_HEIGHT;

	float m_SrcPosX;
	float m_SrcPosY;

	Rectf m_SrcRect;
	Rectf m_CollisionRect;

};