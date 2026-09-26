#include "pch.h"
#include "Player.h"
#include "Texture.h"
#include <iostream>
#include "utils.h"

#include "HUDmanager.h"
#include "Health.h"
#include "Platform.h"
#include "Water.h"
#include "Collectable.h"
#include "PlayerBullet.h"
#include "KeyCollectable.h"

#include "SoundEffect.h"

Player::Player(const Vector2f& position)
	: m_Position{ position },
	m_PlayerState{ State::none },
	m_pTexture{ new Texture("Player/HeroArmor.png") },
	m_AccTime{},
	m_CurrentFrameNr{ },
	m_FRAME_WIDTH{ m_pTexture->GetWidth() / 5 },
	m_FRAME_HEIGHT{ m_pTexture->GetHeight() / 5 },
	m_SrcRectX { },
	m_SrcRectY { },
	m_VelocityY{},
	m_posXonLadder{},
	m_pHP{ new Health(2)},
	m_TakeDmgAnimTime{},
	m_MaxTimeTakeDmgAnim{2.f},
	m_pJumpEffect{new SoundEffect("Sounds/Effects/jump.wav")},
	m_pShootEffect{ new SoundEffect("Sounds/Effects/shoot.wav") },
	m_pCollectablePickedUp{ new SoundEffect("Sounds/Effects/collectablePickedUp.wav") }
{
	InitBoolStates();

	m_pBullets.reserve(10);
}

void Player::InitBoolStates() noexcept
{
	m_IsFlipSprite				  = false;
	m_IsWalking					  = false;
	m_IsJumping					  = false;
	m_IsOnGround				  = false;
	m_IsCrouching				  = false;
	m_IsShooting				  = false;
	m_ShootKeyPressedLastFrame	  = false;
	m_JumpKeyPressedLastFrame	  = false;
	m_IsAbleToClimb				  = false;
	m_IsClimbing				  = false;
	m_ClimbKeyPressedLastFrame	  = false;
	m_TakeDmg					  = false;
	m_TakeDmgAnimationEnded		  = true;
	m_IsAbleToMove				  = true;
	m_CollidesWithWater           = false; 
	m_IsAbleToTakeDmg			  = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Player::~Player() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;

	for (PlayerBullet* PlayerBullet : m_pBullets)
	{
		if (PlayerBullet != nullptr)
		{
			delete PlayerBullet;
			PlayerBullet = nullptr;
		}
	}
	m_pBullets.clear();

	delete m_pHP;
	m_pHP = nullptr;

	delete m_pShootEffect;
	m_pShootEffect = nullptr;

	delete m_pJumpEffect; 
	m_pJumpEffect = nullptr;

	delete m_pCollectablePickedUp;
	m_pCollectablePickedUp = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Player::Draw() const 
{
	Rectf dst{ GetPosition().x, GetPosition().y, m_FRAME_WIDTH*2, m_FRAME_HEIGHT*2 };

	glPushMatrix();
		if (m_IsFlipSprite)
		{
			FlipSprite();
		}

		// Draw the player sprite
		m_pTexture->Draw(dst, GetCurrFrameRect());

	glPopMatrix();

	for (PlayerBullet* PlayerBullet : m_pBullets)
	{
		if (PlayerBullet != nullptr)
		{
			PlayerBullet->Draw();
		}
	}

	//// draw position
	//utils::SetColor(Color4f(0.f, 1.f, 0.f, 1.f));
	//utils::FillEllipse(m_Position, 5.f, 5.f);

	//// Draw collision rectangle 
	//utils::SetColor(Color4f(1.f, 0.f, 1.f, 1.f));
	//utils::DrawRect(m_CollisionRect);
}

void Player::FlipSprite() const noexcept
{
	glTranslatef(m_Position.x + m_FRAME_WIDTH*2 / 2.f, m_Position.y, 0);
	glScalef(-1.0f, 1.0f, 1.0f);
	glTranslatef(-(m_Position.x + m_FRAME_WIDTH*2 / 2.0f), -m_Position.y, 0);
}

const Rectf Player::GetCurrFrameRect() const noexcept
{
	return Rectf{ m_SrcRectX, m_SrcRectY, m_FRAME_WIDTH, m_FRAME_HEIGHT };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update(float elapsedSec, const Uint8* pStates, const std::vector<std::vector<Vector2f>>& levelVertices,
					const std::vector<std::vector<Vector2f>>& ladderVertices, const Platform* platform,
					const std::vector<Water*>& waters, std::vector<Collectable*>& collectables, 
					std::vector<Enemy*>& enemies, HUDmanager* HUD, bool timeEnded)
{
	m_AccTime += elapsedSec;
	m_pCollectablePickedUp->SetVolume(20);
	m_pJumpEffect->SetVolume(m_pJumpEffect->GetVolume() - 50);

	if (!timeEnded)
	{
		UpdateCollisionRect();

		HandleCollision(elapsedSec, levelVertices, ladderVertices, platform, waters, collectables, enemies, HUD);

		ApplyGravity(elapsedSec);

		ResetStatesEveryElapsedSec();

		if (m_IsAbleToMove)
		{
			Move(elapsedSec, pStates, levelVertices);
		}

		CheckIfTakenDMG();

		StateManager();
	}
	else 
	{
		SwitchToSecondTexture();

		m_PlayerState = State::dead;
		m_IsAbleToTakeDmg = false;
	}


	UpdateCurrentFrame();

}

void Player::UpdateCollisionRect() noexcept
{
	if (!m_CollidesWithWater || !m_pHP->IsDead())
	{
		if (m_IsCrouching || m_PlayerState == State::gettingOff)
		{
			m_CollisionRect = Rectf{ m_Position.x + 5.f, m_Position.y - 1.f, m_FRAME_WIDTH * 2 - 10.f, m_FRAME_HEIGHT * 2 - 15.f };
		}
		else if (m_PlayerState == State::gettingOff2)
		{
			m_CollisionRect = Rectf{ m_Position.x + 5.f, m_Position.y - 1.f, m_FRAME_WIDTH * 2 - 10.f, m_FRAME_HEIGHT * 2 - 30.f };
		}
		else
		{
			m_CollisionRect = Rectf{ m_Position.x + 5.f, m_Position.y - 1.f, m_FRAME_WIDTH * 2 - 10.f, m_FRAME_HEIGHT * 2 };
		}
	}
}

void Player::HandleCollision(float elapsedSec, const std::vector<std::vector<Vector2f>>& levelVertices,
							 const std::vector<std::vector<Vector2f>>& ladderVertices, const Platform* platform,
							 const std::vector<Water*>& waters, std::vector<Collectable*>& collectables,
							 std::vector<Enemy*>& enemies, HUDmanager* HUD)
{
	utils::HitInfo hitinfo{};
	m_IsOnGround = false;

	Vector2f velocity{ 0.f, m_VelocityY };

	/////// set rays

	/////HORIZONTAL = left/right collision
	// Check collision with bottom of the rect
	Vector2f bottomStart{ m_CollisionRect.left + 1.f, m_CollisionRect.bottom + 5.f };
	Vector2f bottomEnd{ m_CollisionRect.left + m_CollisionRect.width, m_CollisionRect.bottom + 5.f };

	// Check collision with vertical mid of the rect
	Vector2f midHStart{ m_CollisionRect.left + 1.f, m_CollisionRect.bottom + m_CollisionRect.height / 2 };
	Vector2f midHEnd{ m_CollisionRect.left + m_CollisionRect.width, m_CollisionRect.bottom + m_CollisionRect.height / 2 };

	// Check collision with top of the rect
	Vector2f topStart{ m_CollisionRect.left + 1.f, m_CollisionRect.bottom + m_CollisionRect.height - 5.f };
	Vector2f topEnd{ m_CollisionRect.left + m_CollisionRect.width, m_CollisionRect.bottom + m_CollisionRect.height - 5.f };


	///VERTICAL = top/bot collision
	// Check collision with left side of the rect
	Vector2f leftStart{ m_CollisionRect.left + 5.f, m_CollisionRect.bottom };
	Vector2f leftEnd{ m_CollisionRect.left + 5.f, m_CollisionRect.bottom + m_CollisionRect.height };

	// Check collision with vertical mid of the rect
	Vector2f midVStart{ m_CollisionRect.left + m_CollisionRect.width / 2, m_CollisionRect.bottom };
	Vector2f midVEnd{ m_CollisionRect.left + m_CollisionRect.width / 2, m_CollisionRect.bottom + m_CollisionRect.height };

	// Check collision with right side of the rect
	Vector2f rightStart{ m_CollisionRect.left + m_CollisionRect.width - 5.f, m_CollisionRect.bottom };
	Vector2f rightEnd{ m_CollisionRect.left + m_CollisionRect.width - 5.f, m_CollisionRect.bottom + m_CollisionRect.height };


	////////////////////////////////////////////////////////////////////////////////////////////////////////// floor
	for (int index{}; index < levelVertices.size(); ++index)
	{

		// check for bot/top collision
		if (utils::Raycast(levelVertices[index], leftStart, leftEnd, hitinfo)
			|| utils::Raycast(levelVertices[index], midVStart, midVEnd, hitinfo)
			|| utils::Raycast(levelVertices[index], rightStart, rightEnd, hitinfo))
		{

			if (velocity.DotProduct(hitinfo.normal) < 0)
			{
				// floor
				if (velocity.y < 0)
				{
					m_Position.y = hitinfo.intersectPoint.y + 1.f;
					m_VelocityY = 0.f;
					m_IsOnGround = true;
					m_IsJumping = false;
			
					if (m_PlayerState == State::jumpingUp || m_PlayerState == State::jumpingSide)
					{
						m_PlayerState = State::none;
					}

					if (m_PlayerState == State::climbing)
					{
						m_PlayerState = State::gettingOff;
					}
				}
			}
		}

		// check for left/right collision
		if (utils::Raycast(levelVertices[index], bottomStart, bottomEnd, hitinfo)
			|| utils::Raycast(levelVertices[index], midHStart, midHEnd, hitinfo)
			|| utils::Raycast(levelVertices[index], topStart, topEnd, hitinfo))
		{
			// left
			if (hitinfo.intersectPoint.x >= m_CollisionRect.left &&
				hitinfo.intersectPoint.x <= m_CollisionRect.left + m_CollisionRect.width / 2)
			{
				m_Position.x = hitinfo.intersectPoint.x - 6.f;
			}
			// right
			else if (hitinfo.intersectPoint.x >= m_CollisionRect.left + m_CollisionRect.width / 2 &&
					 hitinfo.intersectPoint.x <= m_CollisionRect.left + m_CollisionRect.width)
			{
				m_Position.x = hitinfo.intersectPoint.x - m_CollisionRect.width - 5.f;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////// ladder
	m_IsAbleToClimb = false;

	for (const std::vector<Vector2f>& ladder : ladderVertices)
	{
		if (utils::Raycast(ladder, bottomStart, bottomEnd, hitinfo)
			|| utils::Raycast(ladder, midHStart, midHEnd, hitinfo)
			|| utils::Raycast(ladder, topStart, topEnd, hitinfo))
		{
			if (hitinfo.intersectPoint.x >= 1480.f && hitinfo.intersectPoint.x <= 1620.f)
			{
				m_posXonLadder = 1520.f;
			}
			else if (hitinfo.intersectPoint.x >= 1890.f && hitinfo.intersectPoint.x <= 2040.f)
			{
				m_posXonLadder = 1934.f;
			}
			else if (hitinfo.intersectPoint.x >= 2235.f && hitinfo.intersectPoint.x <= 2390.f)
			{
				m_posXonLadder = 2273.f;
			}

			m_IsAbleToClimb = true;
			break;
		}
	}

	if (!m_IsAbleToClimb)
	{
		m_IsClimbing = false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////// platform

	std::vector<Vector2f> platformVerts = platform->GetVertices();

	if (!m_CollidesWithWater)
	{
		if (utils::Raycast(platformVerts, leftStart, leftEnd, hitinfo)
			|| utils::Raycast(platformVerts, midVStart, midVEnd, hitinfo)
			|| utils::Raycast(platformVerts, rightStart, rightEnd, hitinfo))
		{
			if (velocity.DotProduct(hitinfo.normal) < 0)
			{
				if (velocity.y < 0) 
				{
					if (hitinfo.intersectPoint.y >= m_CollisionRect.bottom &&
						hitinfo.intersectPoint.y <= m_CollisionRect.bottom + m_CollisionRect.height / 2)
					{
						m_Position.y = hitinfo.intersectPoint.y + 1.f;
						m_IsOnGround = true;
						m_VelocityY = 0.f;

						m_IsJumping = false;

						// Move with platform
						m_Position.x += platform->GetDeltaX();

						if (m_PlayerState == State::jumpingUp || m_PlayerState == State::jumpingSide)
						{
							m_PlayerState = State::none;
						}
					}
				}
			}
		}
	}

	CheckLifeTimeBullets(elapsedSec, levelVertices, enemies);

	CheckCollectablesCollision(collectables, HUD);

	CheckWaterCollision(waters);

}

void Player::CheckLifeTimeBullets(float elapsedSec, const std::vector< std::vector<Vector2f> >& levelVertices,
									std::vector<Enemy*>& enemies)
{
	for (int index{}; index < m_pBullets.size(); ++index)
	{
		PlayerBullet* PlayerBullet = m_pBullets[index];
		if (PlayerBullet != nullptr)
		{
			PlayerBullet->Update(elapsedSec, levelVertices, enemies);

			if (PlayerBullet->BulletLifeTimeEnded() || PlayerBullet->CollidesWithLevel() || PlayerBullet->CollidesWithEnemy())
			{
				delete PlayerBullet;
				m_pBullets.erase(m_pBullets.begin() + index);

				continue;
			}
		}
	}
}

void Player::CheckCollectablesCollision(std::vector<Collectable*>& collectables, HUDmanager* HUD)
{
	for (int index{}; index < collectables.size(); ++index)
	{
		Collectable* pCollectable = collectables[index];

		if (utils::IsOverlapping(pCollectable->GetCollisionRect(), m_CollisionRect))
		{
			HUD->AddPoints(pCollectable->GetPoints());
			m_pCollectablePickedUp->Play(0);

			if (KeyCollectable* key = dynamic_cast<KeyCollectable*>(pCollectable))
			{
				key->OpenDoor();
				HUD->SetVictory();
				m_DoorIsOpen = true;
			}

			delete pCollectable;
			collectables.erase(collectables.begin() + index);

			// Step back to check the new item at this index
			--index;
		}
	}
}

void Player::CheckWaterCollision(const std::vector<Water*>& waters)
{
	for (int index{}; index < waters.size(); ++index)
	{
		Water* pWater = waters[index];

		if (!m_CollidesWithWater)
		{
			if (utils::IsOverlapping(pWater->GetCollisionRect(), GetCollisionRect()))
			{
				m_pHP->TakeDMG(m_pHP->GetHealth());
				m_IsAbleToMove = false;
				m_Position.y = m_Position.y-35.f;
				m_CurrentFrameNr = 3;
				m_CollidesWithWater = true;
			}
		}
	}
}

void Player::ApplyGravity(float elapsedSec) noexcept
{
	if (!m_IsClimbing && !m_pHP->IsDead() && m_IsAbleToMove)
	{
		const float GRAVITY{ -980.f };
		m_VelocityY += GRAVITY * elapsedSec;

		m_Position.y += m_VelocityY * elapsedSec;
	}
}

void Player::ResetStatesEveryElapsedSec() noexcept
{
	m_IsWalking = false;
	m_IsCrouching = false;
	m_ClimbKeyPressedLastFrame = false;
}

void Player::Move(float elapsedSec, const Uint8* pStates, const std::vector<std::vector<Vector2f>>& vertices)
{
	// takes care of movement
	const float JUMP_SPEED{ 400.f };
	const float SPEED{ 160.f };
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////// take dmg animation
	if (m_TakeDmg && m_TakeDmgAnimationEnded && m_pHP->GetHealth()>=1 && !m_pHP->IsDead())
	{
		m_Position.y += 70.f;
		m_Position.x -= 70.f;
		m_TakeDmg = false;
		m_TakeDmgAnimationEnded = false;

		m_CurrentFrameNr = 0;
	}

	if (!m_TakeDmgAnimationEnded)
	{
		m_TakeDmgAnimTime += elapsedSec;
	}

	///////////////////////////////////////////////////////////////// climb up

	if (pStates[SDL_SCANCODE_W] && m_IsAbleToClimb)
	{
		m_IsClimbing = true;
		m_IsOnGround = false;
		m_ClimbKeyPressedLastFrame = true;

		m_Position.y += SPEED * elapsedSec;
		m_Position.x = m_posXonLadder;
	}

	///////////////////////////////////////////////////////////////// crouching and climbing down
	if (pStates[SDL_SCANCODE_S])
	{
		if (!m_IsAbleToClimb)
		{
			m_IsCrouching = true;
		}
		else
		{
			m_IsClimbing = true;
			m_ClimbKeyPressedLastFrame = true;

			m_Position.x = m_posXonLadder;
			m_Position.y -= SPEED*elapsedSec;
		}
	}

	///////////////////////////////////////////////////////////////// shooting
	bool shootKeyPressedNow = pStates[SDL_SCANCODE_P];

	if (shootKeyPressedNow && !m_ShootKeyPressedLastFrame && !m_IsShooting && m_pBullets.size() < 10 && !m_IsClimbing)
	{
		m_IsShooting = true;
		m_pShootEffect->SetVolume(m_pShootEffect->GetVolume() - 70);
		m_pShootEffect->Play(0);


		if (m_IsCrouching)
		{
			m_CurrentFrameNr = 3;

			if (m_IsFlipSprite)
			{
				m_pBullets.push_back(new PlayerBullet(Vector2f{ m_Position.x - 20.f, m_Position.y + 30.f}, -1.f));
			}
			else
			{
				m_pBullets.push_back(new PlayerBullet(Vector2f{ m_Position.x + 20.f, m_Position.y + 30.f}, 1.f));
			}
		}
		else
		{
			m_CurrentFrameNr = 0;

			if (m_IsFlipSprite)
			{
				m_pBullets.push_back(new PlayerBullet(Vector2f{ m_Position.x - 20.f, m_Position.y + 45.f}, -1.f));
			}
			else
			{
				m_pBullets.push_back(new PlayerBullet(Vector2f{ m_Position.x + 20.f, m_Position.y + 45.f}, 1.f));
			}
		}
	}

	m_ShootKeyPressedLastFrame = shootKeyPressedNow;

	///////////////////////////////////////////////////////////////// movement left right
	if (pStates[SDL_SCANCODE_A] && !m_IsClimbing && !pStates[SDL_SCANCODE_D])
	{
		m_IsFlipSprite = true;

		if (!m_IsCrouching)
		{
			m_Position.x -= SPEED * elapsedSec;

			m_IsWalking = true;
		}

	}
	if (pStates[SDL_SCANCODE_D] && !m_IsClimbing && !pStates[SDL_SCANCODE_A])
	{
		m_IsFlipSprite = false;

		if (!m_IsCrouching)
		{
			m_Position.x += SPEED * elapsedSec;

			m_IsWalking = true;
		}
	}

	///////////////////////////////////////////////////////////////// jump
	bool jumpKeyPressedNow = pStates[SDL_SCANCODE_SPACE];

	if (jumpKeyPressedNow && !m_JumpKeyPressedLastFrame && m_IsOnGround && !m_IsJumping)
	{
		
		m_pJumpEffect->Play(0);
		m_VelocityY = JUMP_SPEED;
		m_IsOnGround = false;
		m_IsJumping = true;


		if (!m_TakeDmgAnimationEnded)
		{
			if (m_IsWalking)
			{
				m_CurrentFrameNr = 2;
			}
			else
			{
				m_CurrentFrameNr = 0;
			}
		}
		else
		{
			if (m_IsWalking)
			{
				m_CurrentFrameNr = 1;
			}
			else
			{
				m_CurrentFrameNr = 0;
			}
		}
	}

	m_JumpKeyPressedLastFrame = jumpKeyPressedNow;
}

void Player::CheckIfTakenDMG()
{
	if (m_TakeDmgAnimationEnded && m_TakeDmgAnimTime >= m_MaxTimeTakeDmgAnim && !m_pHP->IsDead())
	{
		SwitchToSecondTexture();
		m_IsAbleToTakeDmg = true;
	}
}

void Player::SwitchToSecondTexture()
{
	delete m_pTexture;
	m_pTexture = new Texture("Player/HeroNaked.png");
}

bool Player::GetIsPlayerDead() const noexcept
{
	return m_pHP->IsDead();
}

const std::vector<PlayerBullet*>& Player::GetBulletVector()
{
	return m_pBullets;
}

void Player::StateManager() noexcept
{
	if (m_CollidesWithWater)
	{
		m_PlayerState = State::waterDead;
	}
	else if (m_pHP->IsDead())
	{
		m_IsAbleToMove = false;
		m_PlayerState = State::dead;
	}
	else if (!m_TakeDmgAnimationEnded)
	{
		m_PlayerState = State::takeDMG;
	}
	else if (m_IsClimbing && m_Position.y > 220.f)
	{
		m_PlayerState = State::gettingOff2;
	}
	else if (m_IsClimbing && m_Position.y > 205.f)
	{
		m_PlayerState = State::gettingOff;
	}
	else if (m_IsClimbing && m_ClimbKeyPressedLastFrame)
	{
		m_PlayerState = State::climbing;
	}
	else if (m_IsClimbing && !m_ClimbKeyPressedLastFrame)
	{
		m_PlayerState = State::climbingIdle;
	}
	else if (m_IsCrouching && m_IsShooting)
	{
		m_PlayerState = State::shootingDOWN;
	}
	else if (m_IsShooting)
	{
		m_PlayerState = State::shootingUP;
	}
	else if (m_IsJumping && m_IsWalking && !m_IsOnGround)
	{
		m_PlayerState = State::jumpingSide;

	}
	else if (m_IsJumping && !m_IsOnGround)
	{
		m_PlayerState = State::jumpingUp;

	}
	else if (m_IsCrouching)
	{
		m_PlayerState = State::crouching;
	}
	else if (m_IsWalking)
	{
		m_PlayerState = State::walking;
	}
	else
	{
		m_PlayerState = State::none;
	}
}

void Player::UpdateCurrentFrame() noexcept
{
	const float WALK_TIME{0.1f};
	const float FRAME_TIME{0.18f};
	const float DEATH_TIME{0.25f};
	const float CLIMBING_TIME{ 0.3f };

	switch (m_PlayerState)
	{
	case State::none:
	case State::jumpingUp:
	case State::jumpingSide:
	case State::crouching:
	case State::climbingIdle:
	case State::gettingOff:
	case State::gettingOff2:
	case State::waterDead:
		m_AccTime = 0;
		m_CurrentFrameNr = 0;
		break;

	case State::walking:
		if (m_AccTime >= WALK_TIME)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 5;
		}
		break;

	case State::shootingDOWN:
		if (m_AccTime >= FRAME_TIME)
		{
			m_AccTime = 0;
			if (m_CurrentFrameNr >= 4) 
			{
				m_IsShooting = false;
				m_PlayerState = State::crouching;
				m_CurrentFrameNr = 0;
			}
			else
			{
				m_CurrentFrameNr++; 
			}
		}
		break;

	case State::shootingUP:
		if (m_AccTime >= FRAME_TIME)
		{
			m_AccTime = 0;

			if (m_CurrentFrameNr >= 1) 
			{
				m_IsShooting = false;

				m_CurrentFrameNr = 0;
			}
			else
			{
				m_CurrentFrameNr++; 
			}
		}
		break;

	case State::climbing:
		if (m_AccTime >= FRAME_TIME)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 2;
		}
		break;

	case State::takeDMG:
		if (m_TakeDmgAnimTime >= m_MaxTimeTakeDmgAnim)
		{
			m_TakeDmgAnimationEnded = true;
		}
		else if (m_AccTime >= FRAME_TIME)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 2) % 4;
		}
		break;

	case State::dead:
		if (m_CurrentFrameNr == 4)
		{
			m_CurrentFrameNr = 4;
			break;
		}
		if (m_AccTime >= DEATH_TIME)
		{
			m_AccTime = 0;
			m_CurrentFrameNr = (m_CurrentFrameNr + 1) % 5;
		}
		break;

	}

	UpdateSpriteFrameAccordingToState();
}

void Player::UpdateSpriteFrameAccordingToState() noexcept
{
	switch (m_PlayerState)
	{
	case State::none:
		m_SrcRectX = 0;
		m_SrcRectY = 0;
		break;

	case State::walking:
		m_SrcRectX = m_FRAME_WIDTH * m_CurrentFrameNr;
		m_SrcRectY = 0;
		break;

	case State::jumpingUp:
		m_SrcRectX = 0;
		m_SrcRectY = 1 * m_FRAME_HEIGHT;
		break;

	case State::jumpingSide:
		m_SrcRectX = 1 * m_FRAME_WIDTH;
		m_SrcRectY = 1 * m_FRAME_HEIGHT;
		break;

	case State::crouching:
		m_SrcRectX = 2 * m_FRAME_WIDTH;
		m_SrcRectY = 2 * m_FRAME_HEIGHT;
		break;

	case State::shootingUP:
		m_SrcRectX = m_FRAME_WIDTH * m_CurrentFrameNr;
		m_SrcRectY = 2 * m_FRAME_HEIGHT;
	break;

	case State::shootingDOWN:
		m_SrcRectX = m_FRAME_WIDTH * m_CurrentFrameNr;
		m_SrcRectY = 2 * m_FRAME_HEIGHT;
		break;

	case State::climbing:
		m_SrcRectX = m_FRAME_WIDTH * m_CurrentFrameNr;
		m_SrcRectY = 4 * m_FRAME_HEIGHT;
		break;

	case State::climbingIdle:
		m_SrcRectX = 0.f;
		m_SrcRectY = 4 * m_FRAME_HEIGHT;
		break;

	case State::gettingOff:
		m_SrcRectX = 2 * m_FRAME_WIDTH;
		m_SrcRectY = 4 * m_FRAME_HEIGHT;
		break;

	case State::gettingOff2:
		m_SrcRectX = 3 * m_FRAME_WIDTH;
		m_SrcRectY = 4 * m_FRAME_HEIGHT;
		break;

	case State::waterDead:
		m_SrcRectX = 3 * m_FRAME_WIDTH;
		m_SrcRectY = 0.f;
		break;

	case State::takeDMG:
		m_SrcRectX = m_FRAME_WIDTH * m_CurrentFrameNr;
		m_SrcRectY = 3 * m_FRAME_HEIGHT;
		break;

	case State::dead:
		m_SrcRectX = m_FRAME_WIDTH * m_CurrentFrameNr;
		m_SrcRectY = 3 * m_FRAME_HEIGHT;
		break;
	}
}

const Vector2f& Player::GetPosition() const noexcept
{
	return m_Position;
}

const Rectf& Player::GetCollisionRect() const noexcept
{
	return m_CollisionRect;
}

void Player::TakeDmg(int amount) noexcept
{
	if (m_IsAbleToTakeDmg)
	{
		m_pHP->TakeDMG(amount);
		m_TakeDmg = true;
		m_CurrentFrameNr = 0;
		m_IsAbleToTakeDmg = false;
	}
}
