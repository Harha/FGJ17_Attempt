#include "sprite.h"
#include "game.h"
#include "display.h"
#include "resmanager.h"
#include "macros.h"

Sprite::Sprite(SDL_Texture * sprSheet, int32_t srcX, int32_t srcY, int32_t srcW, int32_t srcH) :
	m_sprSheet(sprSheet),
	m_sprAnimFrames(),
	m_sprAnimRepeat(false),
	m_sprAnimTime(0),
	m_sprAnimRate(0),
	m_sprAnimFrame(0),
	m_sprAnimFrameMax(1)
{
	SDL_Rect srcRect = { srcX, srcY, srcW, srcH };
	m_sprAnimFrames.push_back(srcRect);
}

Sprite::Sprite(SDL_Texture * sprSheet, std::vector<SprAnimFrame> sprAnimFrames, bool sprAnimRepeat, int32_t sprAnimRate) :
	m_sprSheet(sprSheet),
	m_sprAnimFrames(),
	m_sprAnimRepeat(sprAnimRepeat),
	m_sprAnimTime(0),
	m_sprAnimRate(sprAnimRate),
	m_sprAnimFrame(0),
	m_sprAnimFrameMax(sprAnimFrames.size())
{
	for (auto af : sprAnimFrames)
	{
		m_sprAnimFrames.push_back(SDL_Rect{ af.srcX * af.srcW, af.srcY * af.srcH, af.srcW, af.srcH });
	}
}

void Sprite::update(double t, double dt)
{
	if (!m_sprAnimRepeat && m_sprAnimFrame >= m_sprAnimFrameMax - 1)
		return;

	m_sprAnimFrame = static_cast<int32_t>(m_sprAnimTime * m_sprAnimRate) % m_sprAnimFrameMax;
	m_sprAnimTime += dt;
}

void Sprite::render(Display * const display, vec2 position)
{
	SDL_Rect sourceRect = m_sprAnimFrames[m_sprAnimFrame];
	SDL_Rect destinationRect = { static_cast<int32_t>(std::round(position.x)), static_cast<int32_t>(std::round(position.y)),
								 sourceRect.w, sourceRect.h };
	display->drawImage(m_sprSheet, &sourceRect, &destinationRect, position, true);
}

void Sprite::renderOutline(Display * const display, vec2 position)
{
	display->drawRectangle(position, vec2(position.x + m_sprAnimFrames[m_sprAnimFrame].w,
		position.y + m_sprAnimFrames[m_sprAnimFrame].h));
}

void Sprite::setSprAnimTime(double time)
{
	m_sprAnimTime = time;
}

void Sprite::setSprAnimRate(int32_t rate)
{
	m_sprAnimRate = rate;
}

void Sprite::setSprAnimFrame(int32_t frame)
{
	if (frame >= 0 && frame < m_sprAnimFrameMax)
		m_sprAnimFrame = frame;
}

SDL_Rect Sprite::getSrcRect() const
{
	return m_sprAnimFrames[m_sprAnimFrame];
}

double Sprite::getSprAnimTime() const
{
	return m_sprAnimTime;
}

int32_t Sprite::getSprAnimFrame() const
{
	return m_sprAnimFrame;
}

int32_t Sprite::getSrcX() const
{
	return m_sprAnimFrames[m_sprAnimFrame].x;
}

int32_t Sprite::getSrcY() const
{
	return m_sprAnimFrames[m_sprAnimFrame].y;
}

int32_t Sprite::getWidth() const
{
	return m_sprAnimFrames[m_sprAnimFrame].w;
}

int32_t Sprite::getHeight() const
{
	return m_sprAnimFrames[m_sprAnimFrame].h;
}

vec2 Sprite::getDimensions() const
{
	return vec2(static_cast<float>(getWidth()), static_cast<float>(getHeight()));
}
