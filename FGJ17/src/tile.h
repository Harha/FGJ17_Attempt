#ifndef TILE_H
#define TILE_H

#include <string>
#include <cstring>
#include <algorithm>
#include "macros.h"
#include "tools.h"
#include "vec2.h"
#include "tmxtile.h"
#include "aabb.h"
#include "sprite.h"

class Game;

enum TileLayer : uint8_t
{
	TL_BACKGROUND = 0,
	TL_FOREGROUND = 1
};

enum TilePropertyName : uint8_t
{
	TPN_TYPE = 0,
	TPN_TARGET = 1,
	TPN_TYPE_CUSTOM = 2,
	TPN_TARGET_CUSTOM = 3,
	TPN_GATE = 4
};

enum TilePropertyValue : uint8_t
{
	TPV_NORMAL = 0,
	TPV_SOLID = 1,
	TPV_COMPUTER = 2,
	TPV_BUTTON = 3,
	TPV_RED = 4,
	TPV_BLUE = 5,
	TPV_GREEN = 6,
	TPV_YELLOW = 7,
	TPV_PURPLE = 8
};

typedef std::pair<TilePropertyName, TilePropertyValue> TileProperty;
typedef std::vector<TileProperty> TileProperties;

class Tile
{
public:
	Tile(
		Game * const game,
		Sprite sprite,
		vec2 position,
		TileLayer layer,
		TileProperties properties
	);
	void render(Display * const display);
	void renderAABB(Display * const display);
	void setPosition(const vec2 & position);
	void setLayer(TileLayer l);
	vec2 getPosition() const;
	AABB getAABB() const;
	TileLayer getLayer() const;
	TileProperties getProperties() const;
	bool hasPropertyWithValue(TilePropertyName prop_name, TilePropertyValue prop_value) const;

	static TileLayer strToLayer(const std::string & str)
	{
		// Transform str to uppercase
		std::string strUpper = str;
		std::transform(strUpper.begin(), strUpper.end(), strUpper.begin(), ::toupper);

		// Convert strUpper to c-string & take only 3 first characters
		char strLayer[3 + 1]; // + NULL (\0)
		strncpy_s(strLayer, 3 + 1, strUpper.c_str(), 3);

		// Convert the prefix to tile layer
		switch (cstr2int(strLayer))
		{
		case cstr2int("BG_"):
			return TL_BACKGROUND;
		case cstr2int("FG_"):
		default:
			return TL_FOREGROUND;
		}
	}

	static TileProperties strToProperties(const TmxTilePropertiesData & tmxProps)
	{
		// Initialize tile properties vector
		TileProperties props;

		for (auto & tmxProp : tmxProps)
		{
			// Initialize tile property object
			TileProperty prop;

			// Get name & transform to uppercase
			std::string prop_name = tmxProp.first;
			std::transform(prop_name.begin(), prop_name.end(), prop_name.begin(), ::toupper);

			// Convert string name to tile property name
			switch (cstr2int(prop_name.c_str()))
			{
			case cstr2int("TYPE"):
				prop.first = TPN_TYPE;
				break;
			case cstr2int("TARGET"):
				prop.first = TPN_TARGET;
				break;
			case cstr2int("TYPE_CUSTOM"):
				prop.first = TPN_TYPE_CUSTOM;
				break;
			case cstr2int("TARGET_CUSTOM"):
				prop.first = TPN_TARGET_CUSTOM;
				break;
			case cstr2int("GATE"):
				prop.first = TPN_GATE;
				break;
			default:
				LOG_ERROR("Tile: Unknown tile property name: %s! Parsing properties interrupted.", prop_name.c_str());
				return props;
			}

			// Get value & transform to uppercase
			std::string prop_value = tmxProp.second;
			std::transform(prop_value.begin(), prop_value.end(), prop_value.begin(), ::toupper);

			// Convert string value to tile property value
			switch (cstr2int(prop_value.c_str()))
			{
			case cstr2int("NORMAL"):
				prop.second = TPV_NORMAL;
				break;
			case cstr2int("SOLID"):
				prop.second = TPV_SOLID;
				break;
			case cstr2int("COMPUTER"):
				prop.second = TPV_COMPUTER;
				break;
			case cstr2int("BUTTON"):
				prop.second = TPV_BUTTON;
				break;
			case cstr2int("RED"):
				prop.second = TPV_RED;
				break;
			case cstr2int("BLUE"):
				prop.second = TPV_BLUE;
				break;
			case cstr2int("GREEN"):
				prop.second = TPV_GREEN;
				break;
			case cstr2int("YELLOW"):
				prop.second = TPV_YELLOW;
				break;
			case cstr2int("PURPLE"):
				prop.second = TPV_PURPLE;
				break;
			default:
				LOG_ERROR("Tile: Unknown tile property value: %s! Parsing properties interrupted.", prop_value.c_str());
				return props;
			}

			LOG_INFO("Tile: Parsed tile property name: %10s, value: %10s", prop_name.c_str(), prop_value.c_str());

			// Push to tile properties vector
			props.push_back(prop);
		}

		return props;
	}

private:
	Sprite m_sprite;
	vec2 m_position;
	AABB m_aabb;
	TileLayer m_layer;
	TileProperties m_properties;
};

#endif // TILE_H
