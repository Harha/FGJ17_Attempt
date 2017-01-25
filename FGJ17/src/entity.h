#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include "3rdparty/json.hpp"
#include "sprite.h"
#include "vec2.h"
#include "aabb.h"

using json = nlohmann::json;

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
	Entity(
		Game * const game,
		const std::string & name,
		const vec2 & spawn
	);
	virtual void update(Level & lvl, double t, double dt);
	virtual void render(Display * const display);
	virtual void renderAABB(Display * const display);
	void setCurrentSprite(const std::string & key, double sprAnimTime, int32_t sprAnimFrame);
	std::string getName() const;
	std::map<std::string, Sprite> getSpriteSheet() const;
	Sprite & getCurrentSprite();
	std::string getCurrentSpriteKey() const;
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
protected:
	Game * const m_game;
	json m_json;
	std::string m_name;
	std::map<std::string, Sprite> m_spriteSheet;
	std::string m_currentSprite;
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
};

#endif // ENTITY_H
