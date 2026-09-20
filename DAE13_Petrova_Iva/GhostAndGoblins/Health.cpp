#include "pch.h"
#include "Health.h"
#include <iostream>

Health::Health(int hpAmount):
	m_HealthPts{hpAmount},
	m_IsDead{false}
{
}

int Health::GetHealth() const noexcept
{
	return m_HealthPts;
}

void Health::TakeDMG(int amount) noexcept
{
	if (m_HealthPts > amount)
	{
		m_HealthPts -= amount;
	}
	else
	{
		m_IsDead = true;
	}
}


bool Health::IsDead() const noexcept
{
	return m_IsDead;
}

