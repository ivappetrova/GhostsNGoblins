#pragma once

class Texture;
class SoundEffect;

class Door final
{
public:
	explicit Door();
	~Door() noexcept;

	void Draw() const;
	void OpenDoor();

	// Rule of 5
	Door(const Door& theOneIcopy) = delete;
	Door(Door&& theOneImove) = delete;
	Door& operator= (const Door& theOneIcopy) = delete;
	Door& operator= (Door&& theOneImove) = delete;

private:
	Texture* m_pTexture;
	bool m_IsOpen;

	SoundEffect* m_pDoorOpenEffect;
};

