#include "pch.h"
#include "Collectable.h"

Collectable::Collectable(const Vector2f& position, const int points, const bool isBagOpen):
	m_Position{position},
	m_IsBagOpen{ isBagOpen },
	m_Points{points}
{
}

const int Collectable::GetPoints() const
{
	return m_Points;
}
