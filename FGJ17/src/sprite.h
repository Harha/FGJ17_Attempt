#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>
#include <cstdint>
#include <string>
#include <vector>
#include "vec2.h"

class Display;

struct SprAnimFrame
{
	int32_t srcX;
	int32_t srcY;
	int32_t srcW;
	int32_t srcH;

	SprAnimFrame(int32_t x = 0, int32_t y = 0, int32_t w = 16, int32_t h = 16) :
		srcX(x),
		srcY(y),
		srcW(w),
		srcH(h)
	{

	}
};

class Sprite
{
public:
	Sprite(SDL_Texture * sprSheet, int32_t srcX, int32_t srcY, int32_t srcW, int32_t srcH);
	Sprite(SDL_Texture * sprSheet, std::vector<SprAnimFrame> sprAnimFrames, bool sprAnimRepeat, int32_t sprAnimRate);
	void update(double t, double dt);
	void render(Display * const display, vec2 position = vec2(0, 0));
	void renderOutline(Display * const display, vec2 position = vec2(0, 0));
	void setSprAnimTime(double time);
	void setSprAnimRate(int32_t rate);
	void setSprAnimFrame(int32_t frame);
	SDL_Rect getSrcRect() const;
	double getSprAnimTime() const;
	int32_t getSprAnimFrame() const;
	int32_t getSrcX() const;
	int32_t getSrcY() const;
	int32_t getWidth() const;
	int32_t getHeight() const;
	vec2 getDimensions() const;
private:
	SDL_Texture * m_sprSheet;
	std::vector<SDL_Rect> m_sprAnimFrames;
	bool m_sprAnimRepeat;
	double m_sprAnimTime;
	int32_t m_sprAnimRate;
	int32_t m_sprAnimFrame;
	int32_t m_sprAnimFrameMax;
};

#endif // SPRITE_H
