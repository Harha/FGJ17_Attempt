#ifndef IMAGE_H
#define IMAGE_H

#include <SDL.h>
#include <cstdint>
#include <string>
#include "vec2.h"

class Display;

class Image
{
public:
	Image(SDL_Texture * texture, int32_t width = 0, int32_t height = 0);
	void render(Display * const display, vec2 position = vec2(0, 0));
	void renderRepeat(Display * const display, vec2 position = vec2(0, 0), int32_t width = 0, int32_t height = 0);
	void renderOutline(Display * const display, vec2 position = vec2(0, 0));
	void setSrcRect(const vec2 & pos, const vec2 & dim);
	SDL_Rect getSrcRect() const;
	int32_t getSrcX() const;
	int32_t getSrcY() const;
	int32_t getWidth() const;
	int32_t getHeight() const;
	vec2 getDimensions() const;
private:
	SDL_Texture * m_srcImage;
	SDL_Rect m_srcRect;
};

#endif // IMAGE_H
