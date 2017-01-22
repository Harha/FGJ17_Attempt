#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include "vec2.h"
#include "grid.h"
#include "aabb.h"

class Game;
class Display;
class Image;
class Tile;
class TmxMap;
class Entity;

class Level
{
public:
	Level(Game * const game, const std::string & tmxFilePath);
	~Level();
	void update(double t, double dt);
	void render(Display * const display);
	void setGravity(const vec2 & v);
	void moveCamera(const vec2 & v);
	void setCamera(const vec2 & v);
	Game * const getGame() const;
	AABB getAABB() const;
	vec2 getGravity() const;
	vec2 getCamera() const;
	Entity * const getPlayer();
	Grid<Tile *> & getTileGrid();
	std::vector<Entity *> & getEntityVector();
	std::vector<Image *> & getBgImgVector();
private:
	Game * const m_game;
	TmxMap * m_tmxMap;
	uint32_t m_tileWidth;
	uint32_t m_tileHeight;
	uint32_t m_width;
	uint32_t m_height;
	AABB m_aabb;
	vec2 m_gravity;
	vec2 m_camera;
	Entity * m_player;
	Grid<Tile *> m_tileGrid;
	std::vector<Entity *> m_entityVector;
	std::vector<Image *> m_bgImgVector;
};

#endif // LEVEL_H
