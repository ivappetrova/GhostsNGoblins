#pragma once
class Health final
{
public:
	explicit Health(int hpAmount);
	
	int GetHealth() const noexcept;
	void TakeDMG(int amount) noexcept;
	bool IsDead() const noexcept;

private:
	int m_HealthPts;
	bool m_IsDead;
};

