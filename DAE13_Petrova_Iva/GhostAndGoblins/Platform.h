#pragma once
#include <vector>
class Texture;

class Platform final
{
public:
	explicit Platform(const Vector2f& position);
	~Platform() noexcept;

	void Draw() const;
	void Update(float elapsedSec);

	const std::vector<Vector2f>& GetVertices() const;
	float GetDeltaX() const noexcept;

	// Rule of 5
	Platform(const Platform& theOneIcopy) = delete;
	Platform(Platform&& theOneImove) = delete;
	Platform& operator= (const Platform& theOneIcopy) = delete;
	Platform& operator= (Platform&& theOneImove) = delete;


private:
	// functions
	void UpdateCollisionVertices();

	// data members
	Vector2f m_Position;
	Texture* m_pTexture;

	std::vector<Vector2f> m_Vertices;

	const float m_SPEED;
	const float m_WIDTH;
	const float m_HEIGHT;

	float m_TickCounter;
	const float m_FRAMERATE;
	const float m_AMPLITUDE;
	const float m_PERIOD_IN_SECONDS;

	float m_WaveMovementInPercent;
	float m_DeviationX;
	float m_PosWaveX;
	float m_PosWaveY;

	Rectf m_DstRect;

	float m_PreviousPosX;
	float m_DeltaX;
};