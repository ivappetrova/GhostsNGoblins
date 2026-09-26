#include "pch.h"
#include "Door.h"
#include "Texture.h"
#include "SoundEffect.h"

Door::Door():
	m_pTexture{ new Texture("BG/door.png")},
	m_IsOpen{false},
	m_pDoorOpenEffect{new SoundEffect("Sounds/Effects/doorOpen.wav")}
{
}

Door::~Door() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;

	delete m_pDoorOpenEffect;
	m_pDoorOpenEffect = nullptr;
}

void Door::Draw() const
{
	float FRAME_WIDTH{ m_pTexture->GetWidth() / 2 };

	Rectf dst{ 7470.f, 75.f, FRAME_WIDTH * 2.5f, m_pTexture->GetHeight() * 2.1f };
	Rectf src{};

	if (!m_IsOpen)
	{
		src = Rectf{0.f, m_pTexture->GetHeight(), FRAME_WIDTH, m_pTexture->GetHeight()};
	}
	else
	{
		src = Rectf{ FRAME_WIDTH, m_pTexture->GetHeight(), FRAME_WIDTH, m_pTexture->GetHeight() };
	}

	m_pTexture->Draw(dst, src);
}

void Door::OpenDoor()
{
	m_IsOpen = true;

	m_pDoorOpenEffect->SetVolume(m_pDoorOpenEffect->GetVolume()-50);
	m_pDoorOpenEffect->Play(0);
}



