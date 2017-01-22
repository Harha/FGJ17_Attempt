#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include "vec2.h"

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Color SDL_Color;
typedef struct _TTF_Font TTF_Font;

class Display
{
public:
	Display(std::string title = "null", int32_t width = 640,
		int32_t height = 480, int32_t scale = 1);
	~Display();

	void clear();
	void render();
	void drawImage(SDL_Texture * texture, SDL_Rect * sourceRect, SDL_Rect * destRect,
		const vec2 & destPos, bool clip = false);
	void drawImageRepeat(SDL_Texture * texture, SDL_Rect * sourceRect, SDL_Rect * destRect,
		const vec2 & destPos, int32_t w, int32_t h, bool clip = false);
	void drawText(TTF_Font * font, const std::string & text, SDL_Color color,
		const vec2 & destPos);
	void drawRectangle(const vec2 & tl, const vec2 & br);
	void setState(uint32_t flags);
	void setTitle(const std::string & title);
	void setOffset(const vec2 & offset);
	std::string getTitle() const;
	int32_t getWidth() const;
	int32_t getHeight() const;
	int32_t getScale() const;
	vec2 getOffset() const;
	SDL_Renderer * const getRenderer() const;
private:
	std::string m_title;
	int32_t m_width;
	int32_t m_height;
	int32_t m_scale;
	vec2 m_offset;
	SDL_Window * m_window;
	SDL_Renderer * m_renderer;
};

#endif // DISPLAY_H
