#include "entity.h"
#include <SDL.h>
#include "game.h"
#include "display.h"
#include "level.h"
#include "tile.h"

Entity::Entity(
	Game * const game,
	const std::string & sprite,
	const AABB & aabb,
	const vec2 & position
) :
	m_game(game),
	m_sprites(),
	m_sprite(sprite),
	m_aabb(aabb),
	m_state(ENTITY_GROUNDED),
	m_moveDirX(ENTITY_LEFT),
	m_moveDirY(ENTITY_DOWN),
	m_spawn(position),
	m_position(position),
	m_velocity(0, 0),
	m_airFriction(0.05f),
	m_grndFriction(0.25f),
	m_input{ false, false, false, false, false, false }
{

}

void Entity::update(Level & lvl, double t, double dt)
{
	// Teleport to spawn coordinate if we fell out of map
	if (!lvl.getAABB().collidesYUp(m_aabb))
	{
		m_position = m_spawn;
	}

	// Update v & s
	m_velocity -= m_velocity * ((m_state == ENTITY_FLYING) ? m_airFriction : m_grndFriction);
	m_position = m_position + m_velocity * static_cast<float>(dt);

	// Update AABB
	m_aabb.setMinP(m_position);
	m_aabb.setMaxP(m_position + getSprite().getDimensions() * 0.5f + getSprite().getDimensions() * 0.5f);

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
	std::vector<Tile *> nearbyTiles;
	if (lvl.getTileGrid().getNearestData(m_aabb.getCenterP(), 1, nearbyTiles))
	{
		AABB aabb_vx(m_aabb.getMinP(), m_aabb.getMaxP());
		aabb_vx = aabb_vx + vec2(m_velocity.x * static_cast<float>(dt), 0.0f);
		AABB aabb_vy(m_aabb.getMinP(), m_aabb.getMaxP());
		aabb_vy = aabb_vy + vec2(0.0f, m_velocity.y * static_cast<float>(dt));

		for (Tile * t : nearbyTiles)
		{
			if (t->getType() != TT_SOLID)
				continue;

			bool inAir = false;

			if (m_aabb.collidesY(t->getAABB()))
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

			if (m_aabb.collidesX(t->getAABB()))
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
	getSprite().render(display, m_position);
}

void Entity::renderAABB(Display * const display)
{
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 255, 255, 255);
	m_aabb.render(display);
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 0, 0, 0);
}

void Entity::setSprite(const std::string & key, double sprAnimTime, int32_t sprAnimFrame)
{
	if (m_sprites.find(key) != m_sprites.end())
	{
		m_sprite = key;

		if (sprAnimTime >= 0.0)
			m_sprites.at(key).setSprAnimTime(sprAnimTime);

		if (sprAnimFrame >= 0.0)
			m_sprites.at(key).setSprAnimFrame(sprAnimFrame);
	}
	else
	{
		ERR("Entity: Entity::setSprite error! Unknown sprite key (" << key << ").");
	}
}

Sprite & Entity::getSprite()
{
	return m_sprites.at(m_sprite);
}

std::string Entity::getSpriteKey() const
{
	return m_sprite;
}

AABB Entity::getAABB() const
{
	return m_aabb;
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
