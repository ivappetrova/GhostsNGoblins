#include "pch.h"
#include <iostream>
#include <vector>
#include "SoundStream.h"
#include "EnemyManager.h"

#include "Enemy.h"
#include "Zombie.h"
#include "Plant.h"
#include "Bird.h"
#include "Devil.h"
#include "ShieldBearer.h"
#include "Boss.h"
#include "Player.h"
#include "HUDmanager.h"
#include "KeyCollectable.h"

EnemyManager::EnemyManager(const Vector2f& playerPos, const std::vector<std::vector<Vector2f>>& levelVertices)
	: m_SpawnCooldown{ 3.0f },
	m_SpawnTimer{ 0.0f },
	m_pWinMusic{ new SoundStream("Sounds/Streams/winMusic.mp3") }
{	
	m_pEnemies.reserve(30);

	this->SpawnZombie(Vector2f{ 40.f, 70.f }, playerPos);
	this->SpawnZombie(Vector2f{ 450.f, 70.f },playerPos);

	m_pEnemies.push_back(new Plant(Vector2f{ 1737.f, 230.f }, playerPos));
	m_pEnemies.push_back(new Plant(Vector2f{ 2385.f, 230.f }, playerPos));
	m_pEnemies.push_back(new Plant(Vector2f{ 4990.f, 47.f }, playerPos));
	m_pEnemies.push_back(new Plant(Vector2f{ 6679.f, 56.f }, playerPos));

	m_pEnemies.push_back(new Bird(Vector2f{ 1610.f, 105.f }));
	m_pEnemies.push_back(new Bird(Vector2f{ 1850.f, 280.f }));
	m_pEnemies.push_back(new Bird(Vector2f{ 2530.f, 105.f }));
	m_pEnemies.push_back(new Bird(Vector2f{ 2710.f, 105.f }));
	m_pEnemies.push_back(new Bird(Vector2f{ 3255.f, 110.f }));

	m_pEnemies.push_back(new Devil(Vector2f{ 3060.f, 75.f }, playerPos.x, levelVertices));

	m_pEnemies.push_back(new ShieldBearer(Vector2f{4500.f, 250.f }));
	m_pEnemies.push_back(new ShieldBearer(Vector2f{ 4600.f, 280.f }));
	m_pEnemies.push_back(new ShieldBearer(Vector2f{ 4700.f, 210.f }));
	m_pEnemies.push_back(new ShieldBearer(Vector2f{ 4800.f, 180.f }));
	m_pEnemies.push_back(new ShieldBearer(Vector2f{ 5000.f, 180.f }));
	m_pEnemies.push_back(new ShieldBearer(Vector2f{ 5100.f, 180.f }));


	m_pEnemies.push_back(new Boss(Vector2f{ 7400.f, 70.f }, playerPos.x, levelVertices));

}

EnemyManager::~EnemyManager()
{
	for (Enemy* enemy : m_pEnemies)
	{
		delete enemy;
	}
	m_pEnemies.clear();

	delete m_pWinMusic;
	m_pWinMusic = nullptr;

}

void EnemyManager::Draw() const
{
	for (Enemy* enemy : m_pEnemies)
	{
		enemy->Draw();
	}
}

void EnemyManager::Update(float elapsedSec, const float windowWidth, Player* player, HUDmanager* HUD, std::vector<Collectable*>& collectablesVector, Door* door)
{
	m_SpawnTimer += elapsedSec;

	for (int index{}; index < m_pEnemies.size(); )
	{
		if (m_pEnemies[index] == nullptr)
		{
			m_pEnemies.erase(m_pEnemies.begin() + index);
			// if nullptr, skip the rest of the checks and go to next iteration with the same index because the elements shifted
			continue;
		}

		//update if not nullptr
		m_pEnemies[index]->Update(elapsedSec, player, HUD);

		// check if dead and should delete
		bool shouldDelete = false;

		// check specially for zombie because it has an extra state
		if (Zombie* zombie = dynamic_cast<Zombie*>(m_pEnemies[index]))
		{
			if ((zombie->IsDead() && zombie->HasFinishedDeathAnim()) || zombie->WentDown())
			{
				Collectable* drop = zombie->DropCollectable();
				if (drop != nullptr)
				{
					collectablesVector.push_back(drop);
				}
				shouldDelete = true;
			}
		}
		else if (m_pEnemies[index]->IsDead() && m_pEnemies[index]->HasFinishedDeathAnim())
		{
			if (Boss* boss = dynamic_cast<Boss*>(m_pEnemies[index]))
			{
				collectablesVector.push_back(new KeyCollectable(door));
				HUD->TakeKey();
				m_pWinMusic->Play(false);
			}
			else
			{
				Collectable* drop = m_pEnemies[index]->DropCollectable();
				if (drop != nullptr)
				{
					collectablesVector.push_back(drop);
				}
			}
			shouldDelete = true;
		}


	
		if (shouldDelete)
		{
			delete m_pEnemies[index];
			m_pEnemies.erase(m_pEnemies.begin() + index);
		}
		else
		{
			++index;
		}
	}

	if ((player->GetPosition().x - windowWidth / 2.f) < 2400.f && m_SpawnTimer >= m_SpawnCooldown)
	{
		SpawnZombie(windowWidth, player->GetPosition());
		m_SpawnTimer = 0.f;
	}
}

void EnemyManager::SpawnZombie(const Vector2f& position, const Vector2f& playerPos)
{
	m_pEnemies.push_back(new Zombie(position, playerPos.x));
}

void EnemyManager::SpawnZombie(const float windowWidth, const Vector2f& playerPos)
{
	int spawnY = GetRandomZombiePosY(windowWidth, playerPos.y);
	int spawnX = GetRandomZombiePosX(windowWidth, playerPos.x, spawnY);

	m_pEnemies.push_back(new Zombie(Vector2f{ static_cast<float>(spawnX), static_cast<float>(spawnY) }, playerPos.x));
}

int EnemyManager::GetRandomZombiePosY(const float windowWidth, const float playerY)
{
	if (playerY > 100.f)
	{
		int randomPick = rand() % 2;

		if (randomPick == 0)
		{
			return 70;
		}
		else
		{
			return 240;
		}
	}
	else
	{
		return 70;
	}
}

int EnemyManager::GetRandomZombiePosX(const float windowWidth, const float playerX, const int spawnY)
{
	int leftZoneStart  ;
	int leftZoneEnd    ;
	int rightZoneStart ;
	int rightZoneEnd   ;

	if (spawnY > 100.f)
	{
		// 2 spawn zones:
		leftZoneStart = 1325;
		leftZoneEnd = static_cast<int>(playerX - 50.f);
		rightZoneStart = static_cast<int>(playerX + 120.f);
		rightZoneEnd = 2320;
	}
	else
	{
		// 2 spawn zones:
		leftZoneStart = static_cast<int>(playerX - windowWidth / 2 + 20.f);
		leftZoneEnd = static_cast<int>(playerX - 50.f);
		rightZoneStart = static_cast<int>(playerX + 120.f);
		rightZoneEnd = static_cast<int>(playerX + windowWidth / 2 + 20.f);

		//// fix bounds
		if (leftZoneStart <= 20)
		{
			leftZoneStart = 22;
		}
	}

	// Calculate lengths of both zones
	int leftZoneLength = leftZoneEnd - leftZoneStart;
	int rightZoneLength = rightZoneEnd - rightZoneStart;

	// Randomly pick either left or right zone weighted by their length
	int totalLength = leftZoneLength + rightZoneLength;

	if (totalLength <= 0)
	{
		return static_cast<int>(playerX);
	}

	int randomPick = rand() % totalLength;

	if (randomPick < leftZoneLength)
	{
		// Spawn in left zone
		return leftZoneStart + randomPick;
	}
	else
	{
		// Spawn in right zone
		return rightZoneStart + (randomPick - leftZoneLength);
	}
}

std::vector<Enemy*>& EnemyManager::GetpEnemiesVector()
{
	return m_pEnemies;
}

