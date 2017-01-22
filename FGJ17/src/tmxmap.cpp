#include "tmxmap.h"
#include <cstring>
#include "3rdparty/pugixml.hpp"
#include "3rdparty/pugiconfig.hpp"
#include "game.h"
#include "resmanager.h"
#include "display.h"
#include "macros.h"

TmxMap::TmxMap(Game & game, const std::string & filePath) :
	m_filePath(filePath),
	m_mapData
{
	"NULL",								// version
	"NULL",								// orientation
	"NULL",								// renderorder
	0,									// width
	0,									// height
	0,									// tilewidth
	0,									// tileheight
	std::vector<TmxTilesetData>(),		// tilesets
	std::vector<TmxLayerData>(),		// layers
	std::vector<TmxImgLayerData>(),		// imglayers
	std::vector<TmxObjectgroupData>()	// objectgroups
}
{
	pugi::xml_document xml_doc;
	pugi::xml_parse_result xml_res = xml_doc.load_file(m_filePath.c_str());

	if (xml_res.status != pugi::xml_parse_status::status_ok)
	{
		ERR("TmxMap: Error loading/parsing file (" << m_filePath << ")!");
		return;
	}

	// .tmx <map> info start
	pugi::xml_node child_map = xml_doc.child("map");

	m_mapData.version = child_map.attribute("version").value();
	m_mapData.orientation = child_map.attribute("orientation").value();
	m_mapData.renderorder = child_map.attribute("renderorder").value();
	uint32_t map_width = m_mapData.width = std::stoi(child_map.attribute("width").value());
	uint32_t map_height = m_mapData.height = std::stoi(child_map.attribute("height").value());
	uint32_t tile_width = m_mapData.tilewidth = std::stoi(child_map.attribute("tilewidth").value());
	uint32_t tile_height = m_mapData.tileheight = std::stoi(child_map.attribute("tileheight").value());

	// .tmx <tileset> info start
	pugi::xml_node child_tileset = child_map.child("tileset");
	m_mapData.tileset.push_back(TmxTilesetData());

	m_mapData.tileset.back().firstgid = std::stoi(child_tileset.attribute("firstgid").value());
	m_mapData.tileset.back().name = child_tileset.attribute("name").value();
	m_mapData.tileset.back().source = child_tileset.child("image").attribute("source").value();
	m_mapData.tileset.back().width = std::stoi(child_tileset.child("image").attribute("width").value());
	m_mapData.tileset.back().height = std::stoi(child_tileset.child("image").attribute("height").value());
	m_mapData.tileset.back().tilewidth = std::stoi(child_tileset.attribute("tilewidth").value());
	m_mapData.tileset.back().tileheight = std::stoi(child_tileset.attribute("tileheight").value());

	// .tmx <layer> info
	for (pugi::xml_node layer = child_map.child("layer"); layer; layer = layer.next_sibling("layer"))
	{
		m_mapData.layer.push_back(TmxLayerData());

		std::string layer_name = m_mapData.layer.back().name = layer.attribute("name").value();
		uint32_t layer_width = m_mapData.layer.back().width = std::stoi(layer.attribute("width").value());
		uint32_t layer_height = m_mapData.layer.back().height = std::stoi(layer.attribute("height").value());

		std::string layer_prop_type;
		pugi::xml_node child_layer_prop = layer.child("properties");
		for (pugi::xml_node prop = child_layer_prop.first_child(); prop; prop = prop.next_sibling())
		{
			if (prop.attribute("name").value() == std::string("type"))
			{
				layer_prop_type = m_mapData.layer.back().type = prop.attribute("value").value();
			}
		}

		pugi::xml_node child_layer_data = layer.child("data");
		uint32_t tile_index = 0;
		for (pugi::xml_node tile = child_layer_data.first_child(); tile; tile = tile.next_sibling())
		{
			uint32_t tile_gid = std::stoi(tile.attribute("gid").value());

			if (tile_gid != 0)
			{
				// Decrement tile gid
				tile_gid--;

				// Calculate tile coordinates and index
				const int32_t tile_x = m_mapData.tileset.back().tilewidth * (tile_index % layer_width);
				const int32_t tile_y = (m_mapData.tileset.back().tileheight * (layer_height - static_cast<int32_t>(std::ceil(tile_index / layer_width)))) - tile_height;
				const uint32_t tileset_x = m_mapData.tileset.back().tilewidth * (tile_gid % (m_mapData.tileset.back().width / m_mapData.tileset.back().tilewidth));
				const uint32_t tileset_y = m_mapData.tileset.back().tileheight * static_cast<uint32_t>(std::ceil(tile_gid / (m_mapData.tileset.back().width / m_mapData.tileset.back().tilewidth)));

				// Insert the new tile into our layer's data
				m_mapData.layer.back().tiles.push_back(
					Tile(
						game,
						Sprite(game.getResMan()->loadTexture(m_mapData.tileset.back().source), tileset_x, tileset_y, m_mapData.tileset.back().tilewidth, m_mapData.tileset.back().tileheight),
						vec2(static_cast<float>(tile_x), static_cast<float>(tile_y)),
						Tile::strToLayer(layer_name),
						Tile::strToType(layer_prop_type)
					)
				);
			}

			tile_index++;
		}
	}

	// .tmx <imagelayer> info
	for (pugi::xml_node ilayer = child_map.child("imagelayer"); ilayer; ilayer = ilayer.next_sibling("imagelayer"))
	{
		m_mapData.imglayer.push_back(TmxImgLayerData());

		m_mapData.imglayer.back().name = ilayer.attribute("name").value();

		uint32_t ilayer_index = 0;
		for (pugi::xml_node image = ilayer.child("image"); image; image = image.next_sibling("image"))
		{
			std::string ilayer_src = m_mapData.imglayer.back().source = image.attribute("source").value();

			ilayer_index++;
		}
	}

	// .tmx <objectgroup> info
	for (pugi::xml_node ogroup = child_map.child("objectgroup"); ogroup; ogroup = ogroup.next_sibling("objectgroup"))
	{
		m_mapData.objectgroup.push_back(TmxObjectgroupData());

		m_mapData.objectgroup.back().name = ogroup.attribute("name").value();

		uint32_t object_index = 0;
		for (pugi::xml_node object = ogroup.child("object"); object; object = object.next_sibling("object"))
		{
			uint32_t object_id = std::stoi(object.attribute("id").value());
			std::string object_name = object.attribute("name").value();
			std::string object_type = object.attribute("type").value();
			int32_t object_x = std::stoi(object.attribute("x").value());
			int32_t object_y = (map_height * tile_height) - std::stoi(object.attribute("y").value());
			int32_t object_w = std::stoi(object.attribute("width").value());
			int32_t object_h = std::stoi(object.attribute("height").value());

			m_mapData.objectgroup.back().objects.push_back(TmxObject{ object_id, object_name, object_type, object_x, object_y, object_w, object_h });

			object_index++;
		}
	}

	LOG("TmxMap: File (" << m_filePath << ") loaded. Layers: " << m_mapData.layer.size());
}

void TmxMap::render(Display * const display)
{
	for (auto & l : m_mapData.layer)
	{
		for (auto & t : l.tiles)
		{
			t.render(display);
		}
	}
}

TmxMapData &TmxMap::getMapData()
{
	return m_mapData;
}
