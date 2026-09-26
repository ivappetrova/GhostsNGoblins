#pragma once
#include <vector>

class SoundStream;

class Enemy;
class Player;
class HUDmanager;
class Boss;
class Door;
class Collectable;

class EnemyManager final
{
public:
	explicit EnemyManager(const Vector2f& playerPos, const std::vector<std::vector<Vector2f>>& levelVertices);
	~EnemyManager() noexcept;

	void Update(float elapsedSec, const float windowWidth, Player* player, HUDmanager* HUD, std::vector<Collectable*>& collectablesVector, Door* door);
	void Draw() const;
	void SpawnZombie(const Vector2f& position, const Vector2f& playerPos);
	void SpawnZombie(const float windowWidth, const Vector2f& playerPos);
	int GetRandomZombiePosX(const float windowWidth, const float playerX, const int spawnY);
	int GetRandomZombiePosY(const float windowWidth, const float playerY);
	std::vector<Enemy*>& GetpEnemiesVector();

private:
	std::vector<Enemy*> m_pEnemies;
	float m_SpawnCooldown;
	float m_SpawnTimer;

	SoundStream* m_pWinMusic;
};
