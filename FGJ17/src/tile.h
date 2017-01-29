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
	TPN_ID = 1,
	TPN_TARGET = 2,
	TPN_TYPE_CUSTOM = 3,
	TPN_ID_CUSTOM = 4,
	TPN_TARGET_CUSTOM = 5
};

enum TilePropertyValue_t : uint8_t
{
	TPV_NULL = 0,
	TPV_STRING = 1,
	TPV_NUMBER = 2
};

struct TilePropertyValue
{
	TilePropertyValue_t type;
	std::string value_str;
	int32_t value_num;

	TilePropertyValue(
		TilePropertyValue_t type = TPV_NULL,
		std::string value_str = "NULL",
		int32_t value_num = NULL
	) :
		type(type),
		value_str(value_str),
		value_num(value_num)
	{

	}
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
			case cstr2int("ID"):
				prop.first = TPN_ID;
				break;
			case cstr2int("TARGET"):
				prop.first = TPN_TARGET;
				break;
			case cstr2int("TYPE_CUSTOM"):
				prop.first = TPN_TYPE_CUSTOM;
				break;
			case cstr2int("ID_CUSTOM"):
				prop.first = TPN_ID_CUSTOM;
				break;
			case cstr2int("TARGET_CUSTOM"):
				prop.first = TPN_TARGET_CUSTOM;
				break;
			default:
				LOG_ERROR("Tile: Unknown tile property name: %s! Parsing properties interrupted.", prop_name.c_str());
				return props;
			}

			// Convert string value to tile property value properly
			if (prop.first == TPN_ID || prop.first == TPN_TARGET)
			{
				// Get value as integer
				int32_t prop_value = std::stoi(tmxProp.second);
				prop.second = TilePropertyValue(TPV_NUMBER, "NULL", prop_value);
			}
			else
			{
				// Get value & transform to uppercase
				std::string prop_value = tmxProp.second;
				std::transform(prop_value.begin(), prop_value.end(), prop_value.begin(), ::toupper);

				switch (cstr2int(prop_value.c_str()))
				{
				case cstr2int("NORMAL"):
					prop.second = TilePropertyValue(TPV_STRING, "NORMAL", NULL);
					break;
				case cstr2int("SOLID"):
					prop.second = TilePropertyValue(TPV_STRING, "SOLID", NULL);
					break;
				case cstr2int("TRIGGER"):
					prop.second = TilePropertyValue(TPV_STRING, "TRIGGER", NULL);
					break;
				case cstr2int("BUTTON"):
					prop.second = TilePropertyValue(TPV_STRING, "BUTTON", NULL);
					break;
				case cstr2int("GATE"):
					prop.second = TilePropertyValue(TPV_STRING, "GATE", NULL);
					break;
				case cstr2int("COMPUTER"):
					prop.second = TilePropertyValue(TPV_STRING, "COMPUTER", NULL);
					break;
				default:
					LOG_ERROR("Tile: Unknown tile property value: %s! Parsing properties interrupted.", prop_value.c_str());
					return props;
				}
			}

			LOG_INFO("Tile: Parsed tile property name: %10s, value type: %1d, value str: %10s, value num: %4d", prop_name.c_str(), prop.second.type, prop.second.value_str.c_str(), prop.second.value_num);

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
