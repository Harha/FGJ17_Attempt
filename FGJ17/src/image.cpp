#include "image.h"
#include "game.h"
#include "display.h"
#include "resmanager.h"

Image::Image(SDL_Texture * texture, int32_t width, int32_t height) :
	m_srcImage(texture),
	m_srcRect{ 0, 0, width, height }
{
	if (m_srcRect.w <= 0 || m_srcRect.h <= 0)
		SDL_QueryTexture(m_srcImage, NULL, NULL, &m_srcRect.w, &m_srcRect.h);
}

void Image::render(Display * const display, vec2 position)
{
	SDL_Rect sourceRect = { m_srcRect.x, m_srcRect.y, m_srcRect.w, m_srcRect.h };
	SDL_Rect destinationRect = {
		static_cast<int32_t>(std::round(position.x)),
		static_cast<int32_t>(std::round(position.y)),
		m_srcRect.w, m_srcRect.h
	};
	display->drawImage(m_srcImage, &sourceRect, &destinationRect, position, true);
}

void Image::renderRepeat(Display * const display, vec2 position, int32_t width, int32_t height)
{
	SDL_Rect sourceRect = { m_srcRect.x, m_srcRect.y, m_srcRect.w, m_srcRect.h };
	SDL_Rect destinationRect = {
		static_cast<int32_t>(std::round(position.x)),
		static_cast<int32_t>(std::round(position.y)),
		m_srcRect.w, m_srcRect.h
	};
	display->drawImageRepeat(m_srcImage, &sourceRect, &destinationRect, position, width, height, true);
}

void Image::renderOutline(Display * const display, vec2 position)
{
	display->drawRectangle(position, vec2(position.x + m_srcRect.w,
		position.y + m_srcRect.h));
}

void Image::setSrcRect(const vec2 &pos, const vec2 &dim)
{
	m_srcRect.x = static_cast<int32_t>(pos.x);
	m_srcRect.y = static_cast<int32_t>(pos.y);
	m_srcRect.w = static_cast<int32_t>(dim.x);
	m_srcRect.h = static_cast<int32_t>(dim.y);
}

SDL_Rect Image::getSrcRect() const
{
	return m_srcRect;
}

int32_t Image::getSrcX() const
{
	return m_srcRect.x;
}

int32_t Image::getSrcY() const
{
	return m_srcRect.y;
}

int32_t Image::getWidth() const
{
	return m_srcRect.w;
}

int32_t Image::getHeight() const
{
	return m_srcRect.h;
}

vec2 Image::getDimensions() const
{
	return vec2(static_cast<float>(getWidth()), static_cast<float>(getHeight()));
}
