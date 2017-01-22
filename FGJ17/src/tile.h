#ifndef TILE_H
#define TILE_H

#include <cstring>
#include "sprite.h"
#include "vec2.h"
#include "aabb.h"
#include "macros.h"

class Game;

enum TileLayer
{
	TL_BACKGROUND = 0,
	TL_FOREGROUND = 1
};

enum TileType
{
	TT_NORMAL = 0,
	TT_SOLID = 1
};

class Tile
{
public:
	Tile(Game &game, Sprite sprite, vec2 position = vec2(0, 0), TileLayer layer = TL_BACKGROUND, TileType type = TT_NORMAL);
	void render(Display * const display);
	void renderAABB(Display * const display);
	void setPosition(const vec2 &position);
	void setLayer(TileLayer l);
	void setType(TileType t);
	vec2 getPosition() const;
	AABB getAABB() const;
	TileLayer getLayer() const;
	TileType getType() const;

	static TileLayer strToLayer(const std::string &str)
	{
		if (!strncasecmp(str.c_str(), "bg_", 3))
		{
			return TL_BACKGROUND;
		}
		else if (!strncasecmp(str.c_str(), "fg_", 3))
		{
			return TL_FOREGROUND;
		}
		else
		{
			return TL_BACKGROUND;
		}
	}

	static TileType strToType(const std::string &str)
	{
		if (!strcasecmp(str.c_str(), "NORMAL"))
		{
			return TT_NORMAL;
		}
		else if (!strcasecmp(str.c_str(), "SOLID"))
		{
			return TT_SOLID;
		}
		else
		{
			return TT_NORMAL;
		}
	}

private:
	Sprite m_sprite;
	vec2 m_position;
	AABB m_aabb;
	TileLayer m_layer;
	TileType m_type;
};

#endif // TILE_H
