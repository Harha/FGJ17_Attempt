#ifndef ENTITY_H
#define ENTITY_H

#include <map>
#include <string>
#include "sprite.h"
#include "vec2.h"
#include "aabb.h"

class Game;
class Display;
class Level;

struct EntityInput
{
	bool keyUp;
	bool keyDown;
	bool keyLeft;
	bool keyRight;
	bool keyA;
	bool keyB;

	bool operator==(const EntityInput & i) const
	{
		if (keyUp == i.keyUp &&
			keyDown == i.keyDown &&
			keyLeft == i.keyLeft &&
			keyRight == i.keyRight &&
			keyA == i.keyA &&
			keyB == i.keyB)
		{
			return true;
		}

		return false;
	}

	bool operator!=(const EntityInput & i) const
	{
		return !(*this == i);
	}

};

enum EntityState
{
	ENTITY_GROUNDED = 0,
	ENTITY_FLYING = 1
};

enum EntityMoveDirX
{
	ENTITY_LEFT = 0,
	ENTITY_RIGHT = 1
};

enum EntityMoveDirY
{
	ENTITY_UP = 0,
	ENTITY_DOWN = 1
};

class Entity
{
public:
	Entity(Game * const game, const std::string & sprite, const AABB & aabb, const vec2 & position);
	virtual void update(Level & lvl, double t, double dt);
	virtual void render(Display * const display);
	virtual void renderAABB(Display * const display);
	void setSprite(const std::string & key, double sprAnimTime, int32_t sprAnimFrame);
	Sprite & getSprite();
	std::string getSpriteKey() const;
	AABB getAABB() const;
	EntityState getState() const;
	EntityMoveDirX getMoveDirX() const;
	EntityMoveDirY getMoveDirY() const;
	vec2 getSpawn() const;
	vec2 getPosition() const;
	vec2 getVelocity() const;
	float getAirFriction() const;
	float getGrndFriction() const;
	EntityInput & getInput();
protected:
	Game * const m_game;
	std::map<std::string, Sprite> m_sprites;
	std::string m_sprite;
	AABB m_aabb;
	EntityState m_state;
	EntityMoveDirX m_moveDirX;
	EntityMoveDirY m_moveDirY;
	vec2 m_spawn;
	vec2 m_position;
	vec2 m_velocity;
	float m_airFriction;
	float m_grndFriction;
	EntityInput m_input;
};

#endif // ENTITY_H
