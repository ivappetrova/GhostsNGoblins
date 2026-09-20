#include "pch.h"
#include "Platform.h"
#include "Texture.h"
#include "SVGParser.h"
#include "utils.h"

Platform::Platform(const Vector2f& position) :
	m_Position{ position },
	m_pTexture{ new Texture("BG/platform.png") },
	m_SPEED{20.f},
	m_WIDTH{70.f},
	m_HEIGHT{35.f},
	m_TickCounter{},
	m_FRAMERATE{50.f},
	m_AMPLITUDE{110.f}, 
	m_PERIOD_IN_SECONDS{1.f},
	m_WaveMovementInPercent{ m_TickCounter /(m_FRAMERATE * m_PERIOD_IN_SECONDS) },
	m_DeviationX{ m_AMPLITUDE * float(cos(2 * M_PI * m_WaveMovementInPercent)) },
	m_PosWaveX{ m_Position.x - m_WIDTH/2 - m_DeviationX },
	m_PosWaveY{ m_Position.y + m_HEIGHT/2 },
	m_DstRect { m_PosWaveX, m_PosWaveY, m_WIDTH, m_HEIGHT },
	m_PreviousPosX{ m_PosWaveX },
	m_DeltaX{}

{

	UpdateCollisionVertices();

}

Platform::~Platform() noexcept
{
	delete m_pTexture;
	m_pTexture = nullptr;
}

void Platform::Draw() const
{
	
	m_pTexture->Draw(m_DstRect);

	/*utils::SetColor(Color4f{ 0.f, 1.f, 1.f, 1.f });
	utils::DrawRect(m_DstRect);*/
	
}

void Platform::Update(float elapsedSec)
{
	m_PreviousPosX = m_PosWaveX;

	m_TickCounter += elapsedSec;

	
	m_WaveMovementInPercent={ m_TickCounter / (m_FRAMERATE * m_PERIOD_IN_SECONDS / m_SPEED) };
	m_DeviationX = m_AMPLITUDE * float(cos(2 * M_PI * m_WaveMovementInPercent));
	m_PosWaveX = (m_Position.x - m_WIDTH/2 - m_DeviationX) ;
	
	m_DstRect = Rectf{ m_PosWaveX, m_PosWaveY, m_WIDTH, m_HEIGHT };
	
	m_DeltaX = m_PosWaveX - m_PreviousPosX;

	UpdateCollisionVertices();
}

const std::vector<Vector2f>& Platform::GetVertices() const
{
	return m_Vertices;
}

float Platform::GetDeltaX() const noexcept
{
	return m_DeltaX;
}

void Platform::UpdateCollisionVertices()
{
	m_Vertices.clear();
	m_Vertices.push_back(Vector2f{ m_PosWaveX, m_PosWaveY + m_HEIGHT });
	m_Vertices.push_back(Vector2f{ m_PosWaveX + m_WIDTH, m_PosWaveY + m_HEIGHT });

}

