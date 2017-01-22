#include "level.h"
#include "game.h"
#include "resmanager.h"
#include "display.h"
#include "image.h"
#include "tile.h"
#include "tmxmap.h"
#include "entity.h"
#include "player.h"
#include "macros.h"

Level::Level(Game * const game, const std::string & tmxFilePath) :
	m_game(game),
	m_tmxMap(new TmxMap(*m_game, tmxFilePath)),
	m_tileWidth(m_tmxMap->getMapData().tilewidth),
	m_tileHeight(m_tmxMap->getMapData().tileheight),
	m_width(m_tmxMap->getMapData().width * m_tileWidth),
	m_height(m_tmxMap->getMapData().height * m_tileHeight),
	m_aabb(vec2(0, 0), vec2(static_cast<float>(m_width), static_cast<float>(m_height))),
	m_gravity(vec2(0, -9.81f * 1.0f)),
	m_camera(vec2(0, 0)),
	m_player(nullptr),
	m_tileGrid(32),
	m_entityVector(),
	m_bgImgVector()
{
	// Parse tile layers
	for (TmxLayerData & l : m_tmxMap->getMapData().layer)
	{
		for (Tile & t : l.tiles)
		{
			m_tileGrid.insertData(t.getPosition(), &t);
		}
	}

	// Parse object groups
	for (TmxObjectgroupData & ogd : m_tmxMap->getMapData().objectgroup)
	{
		// Parse entity objects
		if (ogd.name == "ENTITIES")
		{
			for (TmxObject & o : ogd.objects)
			{
				LOG("Level: Parsed entity name: " << o.name << ", type: " << o.type);

				// Player entity
				if (o.type == "PLAYER")
				{
					m_entityVector.push_back(new Player(m_game, vec2(o.x, o.y)));
				}
			}
		}
	}

	// Player is always the first entity
	m_player = m_entityVector.front();

	// Build bgimagevector
	for (TmxImgLayerData &l : m_tmxMap->getMapData().imglayer)
	{
		m_bgImgVector.push_back(new Image(m_game->getResMan()->loadTexture(l.source)));
	}
}

Level::~Level()
{
	for (auto i : m_bgImgVector)
	{
		DELETE_SP(i);
	}

	for (auto e : m_entityVector)
	{
		DELETE_SP(e);
	}

	DELETE_SP(m_tmxMap);
}

void Level::update(double t, double dt)
{
	// Update entities
	for (Entity * e : m_entityVector)
	{
		e->update(*this, t, dt);
	}

	// Update camera
	m_camera = m_player->getAABB().getCenterP();
}

void Level::render(Display * const display)
{
	// Display offset for rendering, centered
	vec2 offset(
		m_camera.negate() +
		vec2(static_cast<float>(display->getWidth()) * 0.5f, static_cast<float>(-display->getHeight()) * 0.5f) / static_cast<float>(display->getScale())
	);
	display->setOffset(offset);

	// Render background images
	for (Image * i : m_bgImgVector)
	{
		vec2 offset_parallax(offset.x * 0.5f, offset.y * 0.5f);

		display->setOffset(offset_parallax);
		i->renderRepeat(display, vec2(0, 0), m_width, m_height);
		display->setOffset(offset);
	}

	// Render tiles, background pass
	std::vector<Tile *> tilesToRenderBg;
	std::vector<Tile *> tilesToRenderFg;
	if (m_tileGrid.getNearestData(m_camera, 4, tilesToRenderBg))
	{
		for (Tile * t : tilesToRenderBg)
		{
			if (t->getLayer() != TL_BACKGROUND)
			{
				tilesToRenderFg.push_back(t);
			}
			else
			{
				t->render(display);
			}
		}
	}

	// Render entities
	for (Entity * e : m_entityVector)
	{
		e->render(display);
	}

	// Render tiles, foreground
	for (Tile * t : tilesToRenderFg)
	{
		t->render(display);
	}

	// Render level AABB
	SDL_SetRenderDrawColor(display->getRenderer(), 255, 255, 255, 255);
	m_aabb.render(display);
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 0, 0, 0);

	// Reset display offset
	display->setOffset(vec2(0, 0));
}

void Level::setGravity(const vec2 & v)
{
	m_gravity = v;
}

void Level::moveCamera(const vec2 & v)
{
	m_camera += v;
}

void Level::setCamera(const vec2 & v)
{
	m_camera = v;
}

Game * const Level::getGame() const
{
	return m_game;
}

AABB Level::getAABB() const
{
	return m_aabb;
}

vec2 Level::getGravity() const
{
	return m_gravity;
}

vec2 Level::getCamera() const
{
	return m_camera;
}

Entity * const Level::getPlayer()
{
	return m_player;
}

Grid<Tile *> & Level::getTileGrid()
{
	return m_tileGrid;
}

std::vector<Entity *> & Level::getEntityVector()
{
	return m_entityVector;
}

std::vector<Image *> & Level::getBgImgVector()
{
	return m_bgImgVector;
}