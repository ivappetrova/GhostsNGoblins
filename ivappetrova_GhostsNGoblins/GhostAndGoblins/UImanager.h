#pragma once
class Texture;
class Timer;

class UImanager final
{
public:
	explicit UImanager(Timer* timer);
	~UImanager() noexcept;

	void AddPoints(int points) noexcept;
	int GetScore() const noexcept;

	void TakeKey() noexcept;
	void SetVictory() noexcept;

	void DrawUI(const Vector2f& playerPos, float windowWidth, float levelWidth, bool isPlayerDead, bool timeEnded) const;
	void UpdateTimerUI(float elapsedSec, Timer* timer, bool isPlayerDead);

	// Rule of 5
	UImanager(const UImanager& theOneIcopy) = delete;
	UImanager(UImanager&& theOneImove) = delete;
	UImanager& operator= (const UImanager& theOneIcopy) = delete;
	UImanager& operator= (UImanager&& theOneImove) = delete;

private:
	// functions
	int GetHighScore() const noexcept;

	// members
	int m_CurrentScore;
	int m_TopScore;

	bool m_TakeTheKey;
	bool m_WonTheGame;

	const std::string m_FONT_PATH;

	Texture* m_pPlayer1Text;
	Texture* m_pPlayer1Score;
	Texture* m_pTopText;
	Texture* m_pTopScore;

	Texture* m_pTimerText;
	Texture* m_pTime;
	std::string m_TimeStr;

	Texture* m_pGameOver;

	Texture* m_pTakeAkeyText;

	Texture* m_pVictoryText;
	Texture* m_pResetText;
};