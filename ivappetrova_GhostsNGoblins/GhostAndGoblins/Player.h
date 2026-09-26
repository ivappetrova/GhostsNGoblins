#pragma once
#include <vector>

class Texture;
class SoundEffect;

class HUDmanager;
class Health;
class Water;
class Platform;
class Collectable;
class Enemy;
class PlayerBullet; 

class Player final
{
public:
	explicit Player(const Vector2f& position);
	~Player() noexcept;

	void Draw() const;
	void Update(float elapsedSec, const Uint8* pStates, const std::vector<std::vector<Vector2f>>& levelVertices,
				const std::vector<std::vector<Vector2f>>& ladderVertices, const Platform* platform, 
				const std::vector<Water*>& waters, std::vector<Collectable*>& collectables,
				std::vector<Enemy*>& enemies, HUDmanager* HUD, bool timeEnded);
	void TakeDmg(int amount) noexcept;

	// getters
	const Vector2f& GetPosition() const noexcept;
	const Rectf& GetCollisionRect() const noexcept;
	const std::vector<PlayerBullet*>& GetBulletVector();
	bool GetIsPlayerDead() const noexcept;

	// Rule of 5
	Player(const Player& theOneIcopy) = delete;
	Player(Player&& theOneImove) = delete;
	Player& operator= (const Player& theOneIcopy) = delete;
	Player& operator= (Player&& theOneImove) = delete;

private:

	enum class State
	{
		none, walking, jumpingUp, jumpingSide, crouching ,shootingUP, shootingDOWN, 
		climbing, climbingIdle, gettingOff, gettingOff2, waterDead, takeDMG, dead
	};

	/////////////////////////////////////// FUNCTIONS
	void InitBoolStates()noexcept;
	// for Draw
	void FlipSprite()const noexcept;
	const Rectf GetCurrFrameRect() const noexcept;
	// for Update
	void UpdateCollisionRect() noexcept;
	void HandleCollision(float elapsedSec, const std::vector<std::vector<Vector2f>>& levelVertices,
						const std::vector<std::vector<Vector2f>>& ladderVertices, const Platform* platform,
						const std::vector<Water*>& waters, std::vector<Collectable*>& collectables, 
						std::vector<Enemy*>& enemies, HUDmanager* HUD);
	void CheckLifeTimeBullets(float elapsedSec, const std::vector< std::vector<Vector2f> >& levelVertices,
						      std::vector<Enemy*>& enemies);
	void CheckCollectablesCollision(std::vector<Collectable*>& collectables, HUDmanager* HUD);
	void CheckWaterCollision(const std::vector<Water*>& waters);
	void ApplyGravity(float elapsedSec) noexcept;
	void ResetStatesEveryElapsedSec() noexcept;
	void Move(float elapsedSec, const Uint8* pStates, const std::vector< std::vector<Vector2f> >& vertices);
	void CheckIfTakenDMG();
	void SwitchToSecondTexture();
	void StateManager() noexcept;
	void UpdateSpriteFrameAccordingToState() noexcept;
	void UpdateCurrentFrame() noexcept;


	/////////////////////////////////////// DATA MEMBERS
	State m_PlayerState;
	
	//sprite management
	const Texture* m_pTexture;
	Vector2f m_Position;
	float m_AccTime;
	float m_LastStateChangeTime;
	int m_CurrentFrameNr;
	const float m_FRAME_WIDTH;
	const float m_FRAME_HEIGHT;
	float m_SrcRectX;
	float m_SrcRectY;

	//collision
	float m_VelocityY;
	Rectf m_CollisionRect;
	float m_posXonLadder;

	// states
	bool m_IsFlipSprite;
	bool m_IsWalking;
	bool m_IsJumping;
	bool m_IsOnGround;
	bool m_IsCrouching;
	bool m_IsShooting;
	bool m_ShootKeyPressedLastFrame;
	bool m_JumpKeyPressedLastFrame;
	bool m_IsAbleToClimb;
	bool m_IsClimbing;
	bool m_ClimbKeyPressedLastFrame;
	bool m_TakeDmg;
	bool m_TakeDmgAnimationEnded;
	bool m_IsAbleToMove;
	bool m_CollidesWithWater;
	bool m_IsAbleToTakeDmg;
	bool m_CanJump;

	bool m_DoorIsOpen;

	// take dmg + invincible
	float m_TakeDmgAnimTime;
	float m_MaxTimeTakeDmgAnim;

	// COMPOSITION
	std::vector<PlayerBullet*> m_pBullets;
	Health* m_pHP;

	// effects
	SoundEffect* m_pJumpEffect;
	SoundEffect* m_pShootEffect;
	SoundEffect* m_pCollectablePickedUp;
};

