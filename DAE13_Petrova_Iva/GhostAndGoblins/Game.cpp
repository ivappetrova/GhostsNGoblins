#include "pch.h"
#include "Game.h"
#include "Texture.h"
#include "utils.h"
#include "SVGParser.h"
#include "SoundStream.h"
#include <iostream>

#include "Player.h"
#include "Camera.h"
#include "UImanager.h"
#include "Timer.h"
#include "Platform.h"
#include "Water.h"
#include "Door.h"
#include "CoinCollectable.h"
#include "MoneyBag.h"
#include "BallCollectable.h"
#include "KeyCollectable.h"
#include "Enemy.h"
#include "EnemyManager.h"

Game::Game(const Window& window)
	:BaseGame{ window }
{
	Initialize();
	
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	Vector2f START_POS{ 100.f, 70.f };
	//Vector2f UPPERFLOOR_POS{ 1300.f, 350.f };
	//Vector2f DEVIL_POS{ 2750.f, 70.f };
	Vector2f PLATFORM_POS{ 6200.f, 80.f };
	//Vector2f BOSS_POS{ 7050.f, 100.f };

	InitLevel();
	m_pPlayer = new Player(START_POS);
	m_pCamera = new Camera(GetViewPort().width, GetViewPort().height);
	m_pEnemyManager = new EnemyManager(m_pPlayer->GetPosition(), m_Vertices);
	m_pTimer = new Timer(2, 0);
	m_pScoreUI = new UImanager(m_pTimer);

	m_pMusic = new SoundStream("Sounds/Streams/backgroundMusic.mp3");

	m_pMusic->Play(true);
	SoundStream::SetVolume(SoundStream::GetVolume() - 100);

}

void Game::Cleanup()
{
	DeleteLevel();
	delete m_pDoor;
	delete m_pPlayer;
	delete m_pCamera;
	delete m_pEnemyManager;
	delete m_pTimer;
	delete m_pScoreUI;
	delete m_pMusic;

}

void Game::Update(float elapsedSec)
{
	UpdateLevel(elapsedSec);

	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	m_pPlayer->Update(elapsedSec, pStates, m_Vertices, m_LadderVertices, m_pPlatform, m_pWaters,
		m_pCollectables, m_pEnemyManager->GetpEnemiesVector(), m_pScoreUI, m_pTimer->TimeEnded());

	m_pEnemyManager->Update(elapsedSec, GetViewPort().width, m_pPlayer, m_pScoreUI, m_pCollectables, m_pDoor);

	m_pScoreUI->UpdateTimerUI(elapsedSec, m_pTimer, m_pPlayer->GetIsPlayerDead());

	// ball collectable
	if (m_pPlayer->GetPosition().x > 6300.f && !m_BallCollectableInitialized)
	{
		m_pCollectables.push_back(new BallCollectable(Vector2f{ 6570.f, 500.f }));
		m_BallCollectableInitialized = true;
	}


	//std::cout << "x: " << m_pPlayer->GetPosition().x << "|  y: " << m_pPlayer->GetPosition().y << std::endl;
}

void Game::Draw() const
{
	ClearBackground();

	const float LEVEL_WIDTH{ 7719.f };
	const float LEVEL_HEIGHT{ 448.f };

	m_pCamera->Aim(LEVEL_WIDTH, LEVEL_HEIGHT, Vector2f{ m_pPlayer->GetPosition().x + 40.f,  m_pPlayer->GetPosition().y });

		DrawBackground();

		m_pPlayer->Draw();
		m_pScoreUI->DrawUI(m_pPlayer->GetPosition(), GetViewPort().width, LEVEL_WIDTH, m_pPlayer->GetIsPlayerDead(), m_pTimer->TimeEnded());

		DrawWaters();
		m_pEnemyManager->Draw();

	m_pCamera->Reset();
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	switch ( e.keysym.sym )
	{
	case SDLK_r:
		Cleanup();    
		Initialize(); 
		break;
	}
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}

}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}


void Game::InitLevel()
{
	m_pBackground = new Texture("BG/map.png");
	SVGParser::GetVerticesFromSvgFile("BG/BasicMap.svg", m_Vertices);
	SVGParser::GetVerticesFromSvgFile("BG/ladder.svg", m_LadderVertices);
	m_pDoor = new Door();

	m_pPlatform = new Platform(Vector2f{ 3705.f, 10.f });

	m_pWaters.reserve(5);
	m_pWaters.push_back(new Water(Vector2f{ 3552.f, 0.f }, 305.f));
	m_pWaters.push_back(new Water(Vector2f{ 4203.f, 0.f }, 68.f));
	m_pWaters.push_back(new Water(Vector2f{ 4342.f, 0.f }, 68.f));
	m_pWaters.push_back(new Water(Vector2f{ 5306.f, 0.f }, 68.f));
	m_pWaters.push_back(new Water(Vector2f{ 5995.f, 0.f }, 68.f));

	m_BallCollectableInitialized = false;
	m_pCollectables.reserve(10);
	m_pCollectables.push_back(new CoinCollectable(Vector2f{ 800.f, 70.f }));
	m_pCollectables.push_back(new CoinCollectable(Vector2f{ 1320.f, 240.f }));
	m_pCollectables.push_back(new CoinCollectable(Vector2f{ 1720.f, 240.f }));
	m_pCollectables.push_back(new CoinCollectable(Vector2f{ 2345.f, 240.f }));
	m_pCollectables.push_back(new CoinCollectable(Vector2f{ 6680.f, 76.f }));
	m_pCollectables.push_back(new MoneyBag(Vector2f{ 3020.f, 70.f }));
	m_pCollectables.push_back(new MoneyBag(Vector2f{ 5190.f, 70.f }));
	
}

void Game::DrawBackground() const
{
	m_pBackground->Draw();
	m_pPlatform->Draw();
	m_pDoor->Draw();

	for (Collectable* collectable : m_pCollectables)
	{
		collectable->Draw();
	}

	/*utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
	for (std::vector<Vector2f> vector : m_Vertices)
	{
		utils::DrawPolygon(vector);
	}

	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	for (std::vector<Vector2f> ladder : m_LadderVertices)
	{
		utils::DrawPolygon(ladder);
	}*/
}

void Game::DrawWaters() const
{
	for (Water* water : m_pWaters)
	{
		water->Draw();
	}
}


void Game::UpdateLevel(float elapsedSec)
{
	m_pPlatform->Update(elapsedSec);

	for (Water* water : m_pWaters)
	{
		water->Update(elapsedSec);
	}

	for (Collectable* collectable : m_pCollectables)
	{
		collectable->Update(elapsedSec);
	}
}

void Game::DeleteLevel() noexcept
{
	delete m_pBackground;
	delete m_pPlatform;

	for (Water* water : m_pWaters)
	{
		delete water;
	}
	m_pWaters.clear();

	for (Collectable* collectable : m_pCollectables)
	{
		delete collectable;
	}
	m_pCollectables.clear();

	m_Vertices.clear();
	m_LadderVertices.clear();
}
