#pragma once
class HUDmanager;

class Timer final
{
public:
	explicit Timer(int mins, int seconds);
	explicit Timer(int seconds);

	void Update(float elapsedTime);
	int GetMinutes() const noexcept;
	int GetSeconds() const noexcept;
	bool TimeEnded() const noexcept;

private:
	int m_Seconds;
	float m_AccTime;
	bool m_TimeEnded;


};

