#include "display.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <cassert>
#include "macros.h"

Display::Display(std::string title, int32_t width, int32_t height, int32_t scale) :
	m_title(title),
	m_width(width),
	m_height(height),
	m_scale(scale),
	m_offset(0, 0),
	m_window(NULL),
	m_renderer(NULL)
{
	m_window = SDL_CreateWindow(
		m_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_width,
		m_height,
		SDL_WINDOW_SHOWN
	);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
}

Display::~Display()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
}

void Display::clear()
{
	SDL_RenderClear(m_renderer);
}

void Display::render()
{
	SDL_RenderPresent(m_renderer);
}

void Display::drawImage(SDL_Texture * texture, SDL_Rect * sourceRect,
	SDL_Rect * destRect, const vec2 & destPos, bool clip)
{
	destRect->x = static_cast<int32_t>(std::round((destPos.x + m_offset.x) * m_scale));
	destRect->y = static_cast<int32_t>(std::round((destPos.y + m_offset.y) * -m_scale));
	destRect->w = static_cast<int32_t>(std::round(destRect->w * m_scale));
	destRect->h = static_cast<int32_t>(std::round(destRect->h * m_scale));
	destRect->y -= destRect->h;

	if (clip)
	{
		if (destRect->x < -destRect->w || destRect->x > m_width ||
			destRect->y < -destRect->h || destRect->y > m_height)
			return;
	}

	SDL_RenderCopy(m_renderer, texture, sourceRect, destRect);
}

void Display::drawImageRepeat(SDL_Texture * texture, SDL_Rect * sourceRect,
	SDL_Rect * destRect, const vec2 & destPos, int32_t w, int32_t h, bool clip)
{
	destRect->x = static_cast<int32_t>(std::round((destPos.x + m_offset.x) * m_scale));
	destRect->y = static_cast<int32_t>(std::round((destPos.y + m_offset.y) * -m_scale));
	destRect->w = static_cast<int32_t>(std::round(destRect->w * m_scale));
	destRect->h = static_cast<int32_t>(std::round(destRect->h * m_scale));
	destRect->y -= destRect->h;

	int32_t ix = sourceRect->x;
	int32_t iy = sourceRect->y;
	int32_t iw = sourceRect->w;
	int32_t ih = sourceRect->h;

	for (int32_t y = 0; y < h; y += ih)
	{
		int32_t ty = destRect->y + y * -m_scale;

		for (int32_t x = 0; x < w; x += iw)
		{
			int32_t tx = destRect->x + x * m_scale;

			SDL_Rect destRecti{ tx, ty, destRect->w, destRect->h };

			if (tx < -destRecti.w || tx > m_width ||
				ty < -destRecti.h || ty > m_height)
				continue;

			SDL_RenderCopy(m_renderer, texture, sourceRect, &destRecti);
		}
	}
}

void Display::drawText(TTF_Font * font, const std::string & text,
	SDL_Color color, const vec2 & destPos)
{
	assert(font);

	SDL_Surface * surface = TTF_RenderText_Blended(font, text.c_str(), color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(m_renderer, surface);

	SDL_Rect destRect;
	destRect.x = static_cast<int32_t>(destPos.x);
	destRect.y = static_cast<int32_t>(destPos.y);
	SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
	SDL_RenderCopy(m_renderer, texture, NULL, &destRect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void Display::drawRectangle(const vec2 & tl, const vec2 & br)
{
	SDL_Rect destRect;
	destRect.x = static_cast<int32_t>(std::round((tl.x + m_offset.x) * m_scale));
	destRect.y = static_cast<int32_t>(std::round((tl.y + m_offset.y) * -m_scale));
	destRect.w = static_cast<int32_t>(std::round(std::abs(tl.x - br.x) * m_scale));
	destRect.h = static_cast<int32_t>(std::round(std::abs(tl.y - br.y) * m_scale));
	destRect.y -= destRect.h;

	SDL_RenderDrawRect(m_renderer, &destRect);
}

void Display::setState(uint32_t flags)
{
	SDL_SetWindowFullscreen(m_window, flags);
}

void Display::setTitle(const std::string & title)
{
	m_title = title;
	SDL_SetWindowTitle(m_window, m_title.c_str());
}

void Display::setOffset(const vec2 & offset)
{
	m_offset = vec2(offset.x, offset.y);
}

std::string Display::getTitle() const
{
	return m_title;
}

int32_t Display::getWidth() const
{
	return m_width;
}

int32_t Display::getHeight() const
{
	return m_height;
}

int32_t Display::getScale() const
{
	return m_scale;
}

vec2 Display::getOffset() const
{
	return m_offset;
}

SDL_Renderer * const Display::getRenderer() const
{
	return m_renderer;
}
