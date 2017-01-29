#include "entity.h"
#include <fstream>
#include <SDL.h>
#include "game.h"
#include "resmanager.h"
#include "display.h"
#include "level.h"
#include "tile.h"

Entity::Entity(
	Game * const game,
	const std::string & name,
	const vec2 & spawn,
	EntityProperties properties
) :
	m_game(game),
	m_json(),
	m_name(name),
	m_spriteSheet(),
	m_currentSprite("NULL"),
	m_currentTileCollisions(),
	m_currentEntityCollisions(),
	m_initAABB(),
	m_physAABB(),
	m_spawn(spawn),
	m_position(spawn),
	m_velocity(),
	m_airFriction(0.05f),
	m_grndFriction(0.25f),
	m_input{ false, false, false, false, false, false },
	m_state(ENTITY_FLYING),
	m_moveDirX(ENTITY_STATIONARY_X),
	m_moveDirY(ENTITY_STATIONARY_Y),
	m_properties(properties)
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

	// Parse entity spritesheet & current sprite
	std::string spriteSheet = json_entity["spriteSheet"].get<std::string>();
	m_currentSprite = json_entity["currentSprite"].get<std::string>();

	// Parse entity sprites and sprite animation frames
	json & json_sprites = json_entity["sprites"];
	size_t n_sprites = json_sprites.size();
	for (size_t i = 0; i < n_sprites; i++)
	{
		// Get sprite JSON object
		json & json_sprite = json_sprites[i];

		// Get sprite properties
		std::string name = json_sprite["name"].get<std::string>();
		bool animRepeat = json_sprite["animRepeat"].get<bool>();
		int32_t animRate = json_sprite["animRate"].get<int32_t>();

		// Sprite animation frames list
		std::vector<SprAnimFrame> animFrames;
		size_t n_frames = json_sprites[i]["animFrames"].size();
		for (size_t j = 0; j < n_frames; j++)
		{
			animFrames.push_back(SprAnimFrame(
				json_sprite["animFrames"][j]["x"].get<int32_t>(),
				json_sprite["animFrames"][j]["y"].get<int32_t>(),
				json_sprite["animFrames"][j]["w"].get<int32_t>(),
				json_sprite["animFrames"][j]["h"].get<int32_t>()
			));
		}

		// Insert the new sprite into our spriteSheet
		m_spriteSheet.emplace(name, Sprite(game->getResMan()->loadTexture(spriteSheet), animFrames, animRepeat, animRate));
	}
}

void Entity::update(Level & lvl, double t, double dt)
{
	// Teleport to spawn coordinate if we fell out of map
	if (!lvl.getAABB().collidesYUp(m_physAABB))
		m_position = m_spawn;

	// Movement dir X
	if (m_velocity.x <= EPSILON && m_velocity.x >= -EPSILON)
		m_moveDirX = ENTITY_STATIONARY_X;

	// Movement dir Y
	if (m_velocity.y < -EPSILON)
		m_moveDirY = ENTITY_DOWN;
	else if (m_velocity.y <= EPSILON && m_velocity.y >= -EPSILON)
		m_moveDirY = ENTITY_STATIONARY_Y;

	// Update v & s
	m_velocity -= m_velocity * ((m_state == ENTITY_FLYING) ? m_airFriction : m_grndFriction);
	m_position = m_position + m_velocity * static_cast<float>(dt);

	// Update physical AABB
	m_physAABB = m_initAABB + m_position;

	// Gravity
	m_velocity += lvl.getGravity();

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

	// Clear current collision vectors
	m_currentTileCollisions.clear();
	m_currentEntityCollisions.clear();

	// Collision against tiles
	// TODO: Fix entity getting stuck at corners
	std::vector<Tile *> nearbyTiles;
	if (lvl.getTileGrid()->getNearestData(m_physAABB.getCenterP(), m_game->getPhysicsDistance(), nearbyTiles))
	{
		AABB aabb_vx(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_vx = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), 0.0f);
		AABB aabb_vy(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_vy = aabb_vy + vec2(0.0f, m_velocity.y * static_cast<float>(dt));
		AABB aabb_v(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_v = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), m_velocity.y * static_cast<float>(dt));

		for (Tile * t : nearbyTiles)
		{
			// Update current tile collisions vector against this entity
			if (m_physAABB.collides(t->getAABB()))
				m_currentTileCollisions.push_back(t);

			// Physics collisions below
			if (!t->hasPropertyWithValue(TPN_TYPE, TilePropertyValue(TPV_STRING, "SOLID", NULL)))
				continue;

			bool inAir = false;

			if (m_physAABB.collidesY(t->getAABB()))
			{
				if (aabb_vx.collidesX(t->getAABB()))
				{
					m_velocity.x -= m_velocity.x;
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

					m_velocity.y -= m_velocity.y;
				}
				else
				{
					inAir = inAir && true;
				}
			}

			m_state = inAir ? ENTITY_FLYING : m_state;
		}
	}

	// Collision against entities
	std::vector<Entity *> nearbyEntities;
	if (lvl.getEntityGrid()->getNearestData(m_physAABB.getCenterP(), m_game->getPhysicsDistance(), nearbyEntities))
	{
		AABB aabb_vx(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_vx = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), 0.0f);
		AABB aabb_vy(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_vy = aabb_vy + vec2(0.0f, m_velocity.y * static_cast<float>(dt));
		AABB aabb_v(m_physAABB.getMinP(), m_physAABB.getMaxP());
		aabb_v = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), m_velocity.y * static_cast<float>(dt));

		for (Entity * e : nearbyEntities)
		{
			// Skip instance of self
			if (e == this)
				continue;

			// Update current entity collisions vector against this entity
			if (m_physAABB.collides(e->getPhysAABB()))
				m_currentEntityCollisions.push_back(e);

			// Physics collisions below
			if (!e->hasPropertyWithValue(EPN_TYPE, EntityPropertyValue(EPV_STRING, "SOLID", NULL)))
				continue;

			bool inAir = false;

			if (m_physAABB.collidesY(e->getPhysAABB()))
			{
				if (aabb_vx.collidesX(e->getPhysAABB()))
				{
					e->applyForce(vec2(m_velocity.x * 0.5f, 0.0f));
					m_velocity.x *= 0.5f;
				}
				else if (m_velocity.y != 0.0f)
				{
					inAir = true;
				}
			}

			if (m_physAABB.collidesX(e->getPhysAABB()))
			{
				if (aabb_vy.collidesY(e->getPhysAABB()))
				{
					if (aabb_vy.collidesYDown(e->getPhysAABB()) && m_velocity.y < 0.0f)
					{
						m_state = ENTITY_GROUNDED;
					}

					m_velocity.y -= m_velocity.y;
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

void Entity::applyForce(const vec2 & F)
{
	m_velocity += F;
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

std::vector<Tile *> Entity::getCurrentTileCollisions() const
{
	return m_currentTileCollisions;
}

std::vector<Entity *> Entity::getCurrentEntityCollisions() const
{
	return m_currentEntityCollisions;
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

bool Entity::hasPropertyWithValue(EntityPropertyName prop_name, EntityPropertyValue prop_value) const
{
	for (auto & prop : m_properties)
	{
		if (prop.first == prop_name)
		{
			switch (prop.second.type)
			{
			case EPV_STRING:
				if (prop.second.value_str == prop_value.value_str)
					return true;
				break;
			case EPV_NUMBER:
				if (prop.second.value_num == prop_value.value_num)
					return true;
				break;
			}
		}
	}

	return false;
}