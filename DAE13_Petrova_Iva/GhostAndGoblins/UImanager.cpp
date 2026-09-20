#include "pch.h"
#include "UImanager.h"
#include <iostream>
#include "Texture.h"
#include "Timer.h"

UImanager::UImanager(Timer* timer):
	m_CurrentScore{0},
	m_TopScore{10000},
	m_FONT_PATH{"UI/GhostsNGoblinsFont.ttf"},
	m_WonTheGame{false},
	m_TakeTheKey{false}
{
	// score
	m_pPlayer1Text = new Texture("PLAYER 1", m_FONT_PATH, 25, Color4f{193/255.f, 170/255.f, 0.f, 1.f});
	m_pPlayer1Score = new Texture(std::to_string(this->GetScore()), m_FONT_PATH, 25, Color4f{ 238/255.f, 238 / 255.f, 221 / 255.f, 1.f });
	m_pTopText = new Texture("TOP SCORE", m_FONT_PATH, 25, Color4f{229/255.f, 10/255.f, 97/255.f, 1.f});
	m_pTopScore = new Texture(std::to_string(this->GetHighScore()), m_FONT_PATH, 25, Color4f{ 238 / 255.f, 238 / 255.f, 221 / 255.f, 1.f });

	// timer
	m_pTimerText = new Texture("TIME", m_FONT_PATH, 25, Color4f{ 244 / 255.f, 125 / 255.f, 125 / 255.f, 1.f });
	m_TimeStr = std::to_string(timer->GetMinutes()) + ":" + (timer->GetSeconds() < 10 ? "0" : "") + std::to_string(timer->GetSeconds());
	m_pTime = new Texture(m_TimeStr, m_FONT_PATH, 25, Color4f{125 / 255.f, 244 / 255.f, 244 / 255.f, 1.f});

	// game over
	m_pGameOver = new Texture("GAME OVER PLAYER 1", m_FONT_PATH, 35, Color4f{ 193 / 255.f, 170 / 255.f, 0.f, 1.f });

	// victory
	m_pTakeAkeyText = new Texture("TAKE A KEY FOR COMING IN!", m_FONT_PATH, 25, Color4f{ 244 / 255.f, 125 / 255.f, 125 / 255.f, 1.f });

	m_pVictoryText = new Texture("You won!", m_FONT_PATH, 30, Color4f{125 / 255.f, 244 / 255.f, 244 / 255.f, 1.f});
	m_pResetText = new Texture("Click R to reset", m_FONT_PATH, 25, Color4f{ 238 / 255.f, 238 / 255.f, 221 / 255.f, 1.f });

}

UImanager::~UImanager() noexcept
{
	delete m_pPlayer1Text;
	delete m_pPlayer1Score;
	delete m_pTopText;
	delete m_pTopScore;
	delete m_pGameOver;

	delete m_pTimerText;
	delete m_pTime;

	delete m_pTakeAkeyText;
	delete m_pVictoryText;
	delete m_pResetText;
}

void UImanager::AddPoints(int points) noexcept
{
	m_CurrentScore += points;
	delete m_pPlayer1Score;
	m_pPlayer1Score = new Texture(std::to_string(this->GetScore()), m_FONT_PATH, 25, Color4f{ 238 / 255.f, 238 / 255.f, 221 / 255.f, 1.f });

	if (m_CurrentScore > m_TopScore)
	{
		m_TopScore = m_CurrentScore;

		delete m_pTopScore;
		m_pTopScore = new Texture(std::to_string(this->GetHighScore()), m_FONT_PATH, 25, Color4f{ 238 / 255.f, 238 / 255.f, 221 / 255.f, 1.f });
	}
}

int UImanager::GetScore() const noexcept
{
	return m_CurrentScore;
}

void UImanager::TakeKey() noexcept
{
	m_TakeTheKey = true;
}

void UImanager::SetVictory() noexcept
{
	m_WonTheGame = true;
	m_TakeTheKey = false;
}

int UImanager::GetHighScore() const noexcept
{
	return m_TopScore;
}

void UImanager::DrawUI(const Vector2f& playerPos, float windowWidth, float levelWidth, bool isPlayerDead, bool timeEnded) const
{
	float playerX{ };
	if (playerPos.x < windowWidth/2 - 40.f)
	{
		playerX = windowWidth / 2 - 40.f;
	}
	else if (playerPos.x > levelWidth - windowWidth / 2 - 40.f)
	{
		playerX = levelWidth - windowWidth / 2 - 40.f;
	}
	else
	{
		playerX = playerPos.x;
	}
	m_pPlayer1Text->Draw(Vector2f{ playerX - 270.f, 420.f });
	m_pPlayer1Score->Draw(Vector2f{ playerX - 195.f, 400.f });
	m_pTopText->Draw(Vector2f{ playerX - 80.f, 420.f });
	m_pTopScore->Draw(Vector2f{ playerX, 400.f });

	m_pTimerText->Draw(Vector2f{ playerX - 250.f, 380.f });
	m_pTime->Draw(Vector2f{ playerX - 250.f, 360.f });


	
	if (isPlayerDead || timeEnded)
	{
		m_pGameOver->Draw(Vector2f {playerX - 160.f, 230.f});
		m_pResetText->Draw(Vector2f{ playerX - 65.f, 190.f });
	}

	if (m_TakeTheKey && !m_WonTheGame)
	{
		m_pTakeAkeyText->Draw(Vector2f{ playerX - 120.f, 230.f });
	}

	if (!m_TakeTheKey && m_WonTheGame)
	{
		m_pVictoryText->Draw(Vector2f{ playerX - 10.f, 230.f });
		m_pResetText->Draw(Vector2f{ playerX - 65.f, 190.f });
	}
}

void UImanager::UpdateTimerUI(float elapsedSec, Timer* timer, bool isPlayerDead)
{
	if (!isPlayerDead)
	{
		timer->Update(elapsedSec);

		m_TimeStr = std::to_string(timer->GetMinutes()) + ":" + (timer->GetSeconds() < 10 ? "0" : "") + std::to_string(timer->GetSeconds());
		delete m_pTime;
		m_pTime = new Texture(m_TimeStr, m_FONT_PATH, 25, Color4f{ 125 / 255.f, 244 / 255.f, 244 / 255.f, 1.f });
	}

}
