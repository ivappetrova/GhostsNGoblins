#pragma once
#include "Enemy.h"
class Texture;
class Player;
class UImanager;

class Zombie final: public Enemy
{
public:
	explicit Zombie(const Vector2f& spawnLocation, const float playerPosXInEnemyInitTime);
	virtual ~Zombie() noexcept;

	virtual void Draw() const override;
	virtual void Update(float elapsedSec, Player* player, UImanager* UI) override;
	virtual void TakeDamage(int amount) override;

	bool WentDown() const noexcept;

	// Rule of 5
	Zombie(const Zombie& theOneIcopy) = delete;
	Zombie(Zombie&& theOneImove) = delete;
	Zombie& operator= (const Zombie& theOneIcopy) = delete;
	Zombie& operator= (Zombie&& theOneImove) = delete;

private:
	/////// functions

	//overriden
	virtual void UpdateCollisionRect() override;
	virtual void CheckPlayerCollision(Player* player) override;

	//init
	void SetDirection(const float playerPosXInEnemyInitTime);

	// deletes the old enemy texture to load the flame death effect
	void OnDeath(UImanager* UI);

	// draw
	void FlipSprite() const noexcept;

	//update
	void UpdateCurrentFrameNr();
	void UpdateSrcRect() noexcept;
	void TransitionToWalking();
	void Move(float elapsedSec);
	void TransitionToGoingDown();
	void UpdateDstRect();
	void CheckIfWentDownFinished();

	// data members
	Texture* m_pTexture;
	const float m_FRAME_WIDTH;
	int m_CurrentFrameNr;
	float m_AccTime;

	float m_TimeAlive;
	float m_DirectionX;
	const float m_SPEED;


	Rectf m_DstRect;
	Rectf m_SrcRect;

	bool m_IsAbleToMove;
	bool m_WentDown;


	
	enum class State
	{
		spawning, walking, goingDown, dead
	};

	State m_EnemyState;
	
};

