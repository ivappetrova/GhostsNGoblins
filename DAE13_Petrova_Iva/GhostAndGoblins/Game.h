#pragma once
#include "BaseGame.h"
#include <vector>

class Texture;
class SoundStream;

//my classes
class Player;
class Camera;
class UImanager;
class Timer;
class EnemyManager;
class Platform;
class Water;
class Door;
class Collectable;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:
	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	/////////////////////////////////////////// My stuff:
	Player* m_pPlayer;
	Camera* m_pCamera;
	Timer* m_pTimer;
	UImanager* m_pScoreUI;

	// Enemies
	EnemyManager* m_pEnemyManager;

	// Level
	Texture* m_pBackground;
	std::vector< std::vector<Vector2f> > m_Vertices;
	std::vector< std::vector<Vector2f> > m_LadderVertices;
	Platform* m_pPlatform;
	std::vector<Water*>m_pWaters;
	Door* m_pDoor;
	std::vector<Collectable*>m_pCollectables;
	bool m_BallCollectableInitialized;

	void InitLevel();
	void DrawBackground() const;
	void DrawWaters() const; // foreground
	void UpdateLevel(float elapsedSec);
	void DeleteLevel() noexcept;

	// Music
	SoundStream* m_pMusic;

};