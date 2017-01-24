#include "entity.h"
#include <fstream>
#include <SDL.h>
#include "game.h"
#include "display.h"
#include "level.h"
#include "tile.h"

Entity::Entity(
	Game * const game,
	const std::string & name,
	const vec2 & spawn
) :
	m_game(game),
	m_json(),
	m_name(name),
	m_spriteSheet(),
	m_currentSprite("NULL"),
	m_initAABB(vec2(0, 0), vec2(16, 16)),
	m_physAABB(),
	m_spawn(spawn),
	m_position(spawn),
	m_velocity(),
	m_airFriction(0.05f),
	m_grndFriction(0.25f),
	m_input{ false, false, false, false, false, false },
	m_state(ENTITY_FLYING),
	m_moveDirX(ENTITY_LEFT),
	m_moveDirY(ENTITY_DOWN)
{
	// Load entity JSON file
	std::string jsonFilePath("./data/entities/" + m_name + ".json");
	std::ifstream jsonFile(jsonFilePath, std::ifstream::binary);

	// Throw if loading JSON data failed
	if (jsonFile.is_open() == false)
	{
		throw std::exception(std::string("Error: Can't find JSON data for given entity. Filepath: " + jsonFilePath).c_str());
	}

	// Assign the contents of JSON file to entity's JSON object
	jsonFile >> m_json;
	jsonFile.close();

	// Get entity JSON object
	json & json_entity = m_json["entity"];

	// Parse entity AABB
	json & json_aabb = json_entity["aabb"];
	m_initAABB = AABB(
		vec2(json_aabb["minX"].get<float>(), json_aabb["minY"].get<float>()),
		vec2(json_aabb["maxX"].get<float>(), json_aabb["maxY"].get<float>())
	);
}

void Entity::update(Level & lvl, double t, double dt)
{
	// Teleport to spawn coordinate if we fell out of map
	if (!lvl.getAABB().collidesYUp(m_physAABB))
	{
		m_position = m_spawn;
	}

	// Update v & s
	m_velocity -= m_velocity * ((m_state == ENTITY_FLYING) ? m_airFriction : m_grndFriction);
	m_position = m_position + m_velocity * static_cast<float>(dt);

	// Update physical AABB
	m_physAABB = m_initAABB + m_position;

	// Gravity
	m_velocity += lvl.getGravity();

	// Movement dir -Y
	if (m_velocity.y <= 0.0)
	{
		m_moveDirY = ENTITY_DOWN;
	}

	// Jumping
	if (m_input.keyUp && m_state == ENTITY_GROUNDED)
	{
		m_velocity += vec2(0, 512) * (m_input.keyB ? 1.25f : 1.0f);
		m_state = ENTITY_FLYING;
		m_moveDirY = ENTITY_UP;
	}

	// Movement
	if (m_input.keyRight)
	{
		m_velocity += vec2((m_state == ENTITY_GROUNDED) ? 25.0f : 7.5f, 0) * (m_input.keyB ? 1.5f : 1.0f);
		m_moveDirX = ENTITY_RIGHT;
	}

	if (m_input.keyLeft)
	{
		m_velocity += vec2((m_state == ENTITY_GROUNDED) ? -25.0f : -7.5f, 0) * (m_input.keyB ? 1.5f : 1.0f);
		m_moveDirX = ENTITY_LEFT;
	}

	// Collision
	// TODO: Fix entity getting stuck at corners
	std::vector<Tile *> nearbyTiles;
	if (lvl.getTileGrid().getNearestData(m_physAABB.getCenterP(), 1, nearbyTiles))
	{
		AABB aabb_vx(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_vx = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), 0.0f);
		AABB aabb_vy(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_vy = aabb_vy + vec2(0.0f, m_velocity.y * static_cast<float>(dt));
		AABB aabb_v(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_v = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), m_velocity.y * static_cast<float>(dt));

		for (Tile * t : nearbyTiles)
		{
			if (!t->hasPropertyWithValue(TPN_TYPE, TPV_SOLID))
				continue;

			bool inAir = false;

			if (m_physAABB.collidesY(t->getAABB()))
			{
				if (aabb_vx.collidesX(t->getAABB()))
				{
					m_velocity.x = 0.0f;
				}
				else if (m_velocity.y != 0.0f)
				{
					inAir = true;
				}
			}

			if (m_physAABB.collidesX(t->getAABB()))
			{
				if (aabb_vy.collidesY(t->getAABB()))
				{
					if (aabb_vy.collidesYDown(t->getAABB()) && m_velocity.y < 0.0f)
					{
						m_state = ENTITY_GROUNDED;
					}

					m_velocity.y = 0.0f;
				}
				else
				{
					inAir = inAir && true;
				}
			}

			m_state = inAir ? ENTITY_FLYING : m_state;
		}
	}
}

void Entity::render(Display * const display)
{
	getCurrentSprite().render(display, m_position);

	float dt = 1e-2f;

	AABB aabb_vx(m_physAABB.getMinP(), m_physAABB.getMaxP());
	aabb_vx = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), 0.0f);
	AABB aabb_vy(m_physAABB.getMinP(), m_physAABB.getMaxP());
	aabb_vy = aabb_vy + vec2(0.0f, m_velocity.y * static_cast<float>(dt));
	AABB aabb_v(m_physAABB.getMinP(), m_physAABB.getMaxP());
	aabb_v = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), m_velocity.y * static_cast<float>(dt));

	SDL_SetRenderDrawColor(display->getRenderer(), 0, 255, 0, 255);
	aabb_vx.render(display);
	SDL_SetRenderDrawColor(display->getRenderer(), 255, 0, 0, 255);
	aabb_vy.render(display);
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 255, 255, 255);
	aabb_v.render(display);
}

void Entity::renderAABB(Display * const display)
{
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 255, 255, 255);
	m_physAABB.render(display);
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 0, 0, 0);
}

void Entity::setCurrentSprite(const std::string & key, double sprAnimTime, int32_t sprAnimFrame)
{
	if (m_spriteSheet.find(key) != m_spriteSheet.end())
	{
		m_currentSprite = key;

		if (sprAnimTime >= 0.0)
			m_spriteSheet.at(key).setSprAnimTime(sprAnimTime);

		if (sprAnimFrame >= 0.0)
			m_spriteSheet.at(key).setSprAnimFrame(sprAnimFrame);
	}
	else
	{
		ERR("Entity: Entity::setSprite error! Unknown sprite key (" << key << ").");
	}
}

std::string Entity::getName() const
{
	return m_name;
}

std::map<std::string, Sprite> Entity::getSpriteSheet() const
{
	return m_spriteSheet;
}

Sprite & Entity::getCurrentSprite()
{
	return m_spriteSheet.at(m_currentSprite);
}

std::string Entity::getCurrentSpriteKey() const
{
	return m_currentSprite;
}

AABB Entity::getInitAABB() const
{
	return m_initAABB;
}

AABB Entity::getPhysAABB() const
{
	return m_physAABB;
}

vec2 Entity::getSpawn() const
{
	return m_spawn;
}

vec2 Entity::getPosition() const
{
	return m_position;
}

vec2 Entity::getVelocity() const
{
	return m_velocity;
}

EntityInput & Entity::getInput()
{
	return m_input;
}

float Entity::getAirFriction() const
{
	return m_airFriction;
}

float Entity::getGrndFriction() const
{
	return m_grndFriction;
}

EntityState Entity::getState() const
{
	return m_state;
}

EntityMoveDirX Entity::getMoveDirX() const
{
	return m_moveDirX;
}

EntityMoveDirY Entity::getMoveDirY() const
{
	return m_moveDirY;
}