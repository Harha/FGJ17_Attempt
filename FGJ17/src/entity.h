#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include "3rdparty/json.hpp"
#include "macros.h"
#include "sprite.h"
#include "tmxobject.h"
#include "vec2.h"
#include "aabb.h"

using json = nlohmann::json;

class Game;
class Display;
class Level;
class Tile;

struct EntityInput
{
	bool keyUp;
	bool keyDown;
	bool keyLeft;
	bool keyRight;
	bool keyA;
	bool keyB;
};

enum EntityPropertyName : uint8_t
{
	EPN_TYPE = 0,
	EPN_ID = 1,
	EPN_TARGET = 2,
	EPN_TYPE_CUSTOM = 3,
	EPN_ID_CUSTOM = 4,
	EPN_TARGET_CUSTOM = 5
};

enum EntityPropertyValue_t : uint8_t
{
	EPV_NULL = 0,
	EPV_STRING = 1,
	EPV_NUMBER = 2
};

struct EntityPropertyValue
{
	EntityPropertyValue_t type;
	std::string value_str;
	int32_t value_num;

	EntityPropertyValue(
		EntityPropertyValue_t type = EPV_NULL,
		std::string value_str = "NULL",
		int32_t value_num = NULL
	) :
		type(type),
		value_str(value_str),
		value_num(value_num)
	{

	}
};

enum EntityState : uint8_t
{
	ENTITY_GROUNDED = 0,
	ENTITY_FLYING = 1
};

enum EntityMoveDirX : uint8_t
{
	ENTITY_LEFT = 0,
	ENTITY_RIGHT = 1,
	ENTITY_STATIONARY_X = 2
};

enum EntityMoveDirY : uint8_t
{
	ENTITY_UP = 0,
	ENTITY_DOWN = 1,
	ENTITY_STATIONARY_Y = 2
};

typedef std::pair<EntityPropertyName, EntityPropertyValue> EntityProperty;
typedef std::vector<EntityProperty> EntityProperties;

class Entity
{
public:
	Entity(
		Game * const game,
		const std::string & name,
		const vec2 & spawn,
		EntityProperties properties
	);
	virtual void update(Level & lvl, double t, double dt);
	virtual void render(Display * const display);
	virtual void renderAABB(Display * const display);
	void setCurrentSprite(const std::string & key, double sprAnimTime, int32_t sprAnimFrame);
	void applyForce(const vec2 & F);
	std::string getName() const;
	std::map<std::string, Sprite> getSpriteSheet() const;
	Sprite & getCurrentSprite();
	std::string getCurrentSpriteKey() const;
	std::vector<Tile *> getCurrentTileCollisions() const;
	std::vector<Entity *> getCurrentEntityCollisions() const;
	AABB getInitAABB() const;
	AABB getPhysAABB() const;
	vec2 getSpawn() const;
	vec2 getPosition() const;
	vec2 getVelocity() const;
	float getAirFriction() const;
	float getGrndFriction() const;
	EntityInput & getInput();
	EntityState getState() const;
	EntityMoveDirX getMoveDirX() const;
	EntityMoveDirY getMoveDirY() const;
	bool hasPropertyWithValue(EntityPropertyName prop_name, EntityPropertyValue prop_value) const;

	static EntityProperties strToProperties(const TmxObjectPropertiesData & tmxProps)
	{
		// Initialize entity properties vector
		EntityProperties props;

		for (auto & tmxProp : tmxProps)
		{
			// Initialize entity property object
			EntityProperty prop;

			// Get name & transform to uppercase
			std::string prop_name = tmxProp.first;
			std::transform(prop_name.begin(), prop_name.end(), prop_name.begin(), ::toupper);

			// Convert string name to entity property name
			switch (cstr2int(prop_name.c_str()))
			{
			case cstr2int("TYPE"):
				prop.first = EPN_TYPE;
				break;
			case cstr2int("ID"):
				prop.first = EPN_ID;
				break;
			case cstr2int("TARGET"):
				prop.first = EPN_TARGET;
				break;
			case cstr2int("TYPE_CUSTOM"):
				prop.first = EPN_TYPE_CUSTOM;
				break;
			case cstr2int("ID_CUSTOM"):
				prop.first = EPN_ID_CUSTOM;
				break;
			case cstr2int("TARGET_CUSTOM"):
				prop.first = EPN_TARGET_CUSTOM;
				break;
			default:
				LOG_ERROR("Entity: Unknown entity property name: %s! Parsing properties interrupted.", prop_name.c_str());
				return props;
			}

			// Convert string value to entity property value properly
			if (prop.first == EPN_ID || prop.first == EPN_TARGET)
			{
				// Get value as integer
				int32_t prop_value = std::stoi(tmxProp.second);
				prop.second = EntityPropertyValue(EPV_NUMBER, "NULL", prop_value);
			}
			else
			{
				// Get value & transform to uppercase
				std::string prop_value = tmxProp.second;
				std::transform(prop_value.begin(), prop_value.end(), prop_value.begin(), ::toupper);

				switch (cstr2int(prop_value.c_str()))
				{
				case cstr2int("NORMAL"):
					prop.second = EntityPropertyValue(EPV_STRING, "NORMAL", NULL);
					break;
				case cstr2int("SOLID"):
					prop.second = EntityPropertyValue(EPV_STRING, "SOLID", NULL);
					break;
				case cstr2int("TRIGGER"):
					prop.second = EntityPropertyValue(EPV_STRING, "TRIGGER", NULL);
					break;
				case cstr2int("BUTTON"):
					prop.second = EntityPropertyValue(EPV_STRING, "BUTTON", NULL);
					break;
				case cstr2int("GATE"):
					prop.second = EntityPropertyValue(EPV_STRING, "GATE", NULL);
					break;
				case cstr2int("COMPUTER"):
					prop.second = EntityPropertyValue(EPV_STRING, "COMPUTER", NULL);
					break;
				default:
					LOG_ERROR("Entity: Unknown entity property value: %s! Parsing properties interrupted.", prop_value.c_str());
					return props;
				}
			}

			LOG_INFO("Entity: Parsed entity property name: %10s, value type: %1d, value str: %10s, value num: %4d", prop_name.c_str(), prop.second.type, prop.second.value_str.c_str(), prop.second.value_num);

			// Push to entity properties vector
			props.push_back(prop);
		}

		return props;
	}
protected:
	Game * const m_game;
	json m_json;
	std::string m_name;
	std::map<std::string, Sprite> m_spriteSheet;
	std::string m_currentSprite;
	std::vector<Tile *> m_currentTileCollisions;
	std::vector<Entity *> m_currentEntityCollisions;
	AABB m_initAABB;
	AABB m_physAABB;
	vec2 m_spawn;
	vec2 m_position;
	vec2 m_velocity;
	float m_airFriction;
	float m_grndFriction;
	EntityInput m_input;
	EntityState m_state;
	EntityMoveDirX m_moveDirX;
	EntityMoveDirY m_moveDirY;
	EntityProperties m_properties;
};

#endif // ENTITY_H
