#include "level.h"
#include <fstream>
#include "macros.h"
#include "tools.h"
#include "game.h"
#include "resmanager.h"
#include "display.h"
#include "image.h"
#include "tile.h"
#include "tmxmap.h"
#include "entity.h"
#include "player.h"
#include "box.h"

Level::Level(
	Game * const game,
	const std::string & name,
	TmxMap * const tmxMap
) :
	m_game(game),
	m_json(),
	m_name(name),
	m_tmxMap(tmxMap),
	m_tileWidth(m_tmxMap->getMapData().tilewidth),
	m_tileHeight(m_tmxMap->getMapData().tileheight),
	m_width(m_tmxMap->getMapData().width * m_tileWidth),
	m_height(m_tmxMap->getMapData().height * m_tileHeight),
	m_aabb(vec2(0, 0), vec2(static_cast<float>(m_width), static_cast<float>(m_height))),
	m_gravity(),
	m_camera(),
	m_player(nullptr),
	m_tileGrid(nullptr),
	m_entityGrid(nullptr),
	m_entityVector(),
	m_bgImgVector()
{
	// Load level JSON file
	std::string jsonFilePath("./data/levels/" + m_name + ".json");
	std::ifstream jsonFile(jsonFilePath, std::ifstream::binary);

	// Throw if loading JSON data failed
	if (jsonFile.is_open() == false)
	{
		throw std::exception(std::string("Error: Can't find JSON data for given level. Filepath: " + jsonFilePath).c_str());
	}

	// Assign the contents of JSON file to level's JSON object
	jsonFile >> m_json;
	jsonFile.close();

	// Get level JSON object
	json & json_level = m_json["level"];

	// Parse level gravity
	m_gravity = vec2(
		json_level["gravity"]["x"].get<float>(),
		json_level["gravity"]["y"].get<float>()
	);

	// Parse level tile grid
	m_tileGrid = new Grid<Tile *>(json_level["tileGrid"]["cellDivisor"].get<int32_t>());

	// Parse level entity grid
	m_entityGrid = new Grid<Entity *>(json_level["entityGrid"]["cellDivisor"].get<int32_t>());

	// Parse all tile layers
	for (auto & l : m_tmxMap->getMapData().layer)
	{
		// Parse tiles per layer
		for (auto & t : l.tiles)
		{
			m_tileGrid->insertData(t.getAABB().getCenterP(), &t);
		}
	}

	// Parse all object groups
	for (auto & ogd : m_tmxMap->getMapData().objectgroup)
	{
		// Parse all entities per object group data
		if (ogd.name == "ENTITIES")
		{
			for (auto & o : ogd.objects)
			{
				LOG_INFO("Level: Parsed entity name: %10s, type: %10s", o.name.c_str(), o.type.c_str());

				// Entity position
				vec2 entity_pos(static_cast<float>(o.x), static_cast<float>(o.y));

				// Entity properties
				EntityProperties entity_props = Entity::strToProperties(o.objectproperties);

				// Player entity
				switch (cstr2int(o.type.c_str()))
				{
				case cstr2int("PLAYER"):
					m_entityVector.push_back(new Player(m_game, entity_pos, entity_props));
					break;
				case cstr2int("BOX"):
					m_entityVector.push_back(new Box(m_game, entity_pos, entity_props));
					break;
				}
			}
		}
	}

	// Player is always the first entity
	m_player = m_entityVector.back();

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
}

void Level::update(double t, double dt)
{
	// Rebuild entity grid
	m_entityGrid->clearData();
	for (Entity * e : m_entityVector)
	{
		m_entityGrid->insertData(e->getPhysAABB().getCenterP(), e);
	}

	// Update entities
	for (Entity * e : m_entityVector)
	{
		e->update(*this, t, dt);
	}

	// Update camera
	m_camera = m_player->getPhysAABB().getCenterP();
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
	/*for (Image * i : m_bgImgVector)
	{
		vec2 offset_parallax(offset.x * 0.5f, offset.y * 0.5f);

		display->setOffset(offset_parallax);
		i->renderRepeat(display, vec2(0, 0), m_width, m_height);
		display->setOffset(offset);
	}*/

	// Render tiles, background pass
	std::vector<Tile *> tilesToRenderBg;
	std::vector<Tile *> tilesToRenderFg;
	if (m_tileGrid->getNearestData(m_camera, m_game->getRenderDistance(), tilesToRenderBg))
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
	std::vector<Entity *> entitiesToRender;
	if (m_entityGrid->getNearestData(m_camera, m_game->getRenderDistance(), entitiesToRender))
	{
		for (Entity * e : entitiesToRender)
		{
			e->render(display);
			e->renderAABB(display);
		}
	}

	// Render tiles, foreground
	for (Tile * t : tilesToRenderFg)
	{
		t->render(display);
	}

	// Render level AABB
	/*SDL_SetRenderDrawColor(display->getRenderer(), 255, 255, 255, 255);
	m_aabb.render(display);
	SDL_SetRenderDrawColor(display->getRenderer(), 0, 0, 0, 0);*/

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

Grid<Tile *> * Level::getTileGrid()
{
	return m_tileGrid;
}

Grid<Entity *> * Level::getEntityGrid()
{
	return m_entityGrid;
}

std::vector<Entity *> & Level::getEntityVector()
{
	return m_entityVector;
}

std::vector<Image *> & Level::getBgImgVector()
{
	return m_bgImgVector;
}