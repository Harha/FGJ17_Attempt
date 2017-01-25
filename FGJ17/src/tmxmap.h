#ifndef TMXMAP_H
#define TMXMAP_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "tmxtile.h"
#include "tmxobject.h"
#include "tile.h"

class Game;
class Display;

struct TmxObject
{
	uint32_t id;
	std::string name;
	std::string type;
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
	TmxObjectPropertiesData objectproperties;
};

struct TmxObjectgroupData
{
	std::string name;
	std::vector<TmxObject> objects;
};

struct TmxImgLayerData
{
	std::string name;
	std::string source;
};

struct TmxLayerData
{
	std::string name;
	uint32_t width;
	uint32_t height;
	TmxTilePropertiesData layerproperties;
	std::vector<Tile> tiles;
};

struct TmxTilesetData
{
	uint32_t firstgid;
	std::string name;
	std::string source;
	uint32_t width;
	uint32_t height;
	uint32_t tilewidth;
	uint32_t tileheight;
	std::map<int, TmxTilePropertiesData> tileproperties;
};

struct TmxMapData
{
	std::string version;
	std::string orientation;
	std::string renderorder;
	uint32_t width;
	uint32_t height;
	uint32_t tilewidth;
	uint32_t tileheight;
	TmxTilesetData tileset;
	std::vector<TmxLayerData> layer;
	std::vector<TmxImgLayerData> imglayer;
	std::vector<TmxObjectgroupData> objectgroup;
};

class TmxMap
{
public:
	TmxMap(Game * const game, const std::string & filePath);
	void render(Display * const display);
	TmxMapData & getMapData();
private:
	std::string m_filePath;
	TmxMapData m_mapData;
};

#endif // TMXMAP_H
