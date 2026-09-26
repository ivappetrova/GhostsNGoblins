#include "pch.h"
#include "Timer.h"
#include <iostream>
#include "HUDmanager.h"

Timer::Timer(int mins, int seconds):
	m_Seconds{mins * 60 + seconds},
	m_AccTime{0.f},
	m_TimeEnded {false}
{
	//std::cout << m_Seconds << std::endl;
}

Timer::Timer(int seconds):
	Timer(0, seconds)
{
}

void Timer::Update(float elapsedTime)
{
	if (!m_TimeEnded)
	{
		if (m_Seconds >= 1)
		{
			if (m_AccTime >= 1)
			{
				m_Seconds -= int(m_AccTime);
				m_AccTime = 0;

				//std::cout << m_Seconds << std::endl;
			}
			else
			{
				m_AccTime += elapsedTime;
			}
		}
		else
		{
			m_Seconds = 0;
			m_TimeEnded = true;
		}
	}

}

int Timer::GetMinutes() const noexcept
{
	return m_Seconds / 60;
}

int Timer::GetSeconds() const noexcept
{
	return m_Seconds % 60;
}

bool Timer::TimeEnded() const noexcept
{
	return m_TimeEnded;
}
