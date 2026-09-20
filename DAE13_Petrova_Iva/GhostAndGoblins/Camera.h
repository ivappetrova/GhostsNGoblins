#pragma once
class Camera final
{
public:
	explicit Camera(const float screenWidth, const float screenHeight);

	void Aim(const float levelW, const float levelH, const Vector2f& trackCenter) noexcept;
	Vector2f Reset() noexcept;

private:
	const float m_ScreenWidth;
	const float m_ScreenHeight;
};
