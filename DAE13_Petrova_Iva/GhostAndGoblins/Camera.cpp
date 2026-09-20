#include "pch.h"
#include "Camera.h"

Camera::Camera(const float screenWidth, const float screenHeight)
	:m_ScreenWidth{ screenWidth },
	 m_ScreenHeight{ screenHeight }
{
}

void Camera::Aim(const float levelW, const float levelH, const Vector2f& trackCenter) noexcept
{
	Vector2f cameraBotLeft{ trackCenter.x - m_ScreenWidth / 2, trackCenter.y - m_ScreenHeight / 2 };

	if (cameraBotLeft.x < 0)
	{
		cameraBotLeft.x = 0;
	}
	else if (cameraBotLeft.x + m_ScreenWidth > levelW)
	{
		cameraBotLeft.x = levelW - m_ScreenWidth;
	}

	if (cameraBotLeft.y < 0)
	{
		cameraBotLeft.y = 0;
	}
	else if (cameraBotLeft.y + m_ScreenHeight > levelH)
	{
		cameraBotLeft.y = levelH - m_ScreenHeight;
	}

	// Apply the transformation
	glPushMatrix();
	glTranslatef(-cameraBotLeft.x, -cameraBotLeft.y, 0);
}

Vector2f Camera::Reset() noexcept
{
	glPopMatrix();
	return Vector2f{ 0.f, 0.f };
}
