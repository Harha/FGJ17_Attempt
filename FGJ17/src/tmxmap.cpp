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
	{},									// tileset
	std::vector<TmxLayerData>(),		// layers
	std::vector<TmxImgLayerData>(),		// imglayers
	std::vector<TmxObjectgroupData>()	// objectgroups
}
{
	// Load & parse the XML document
	pugi::xml_document xml_doc;
	pugi::xml_parse_result xml_res = xml_doc.load_file(m_filePath.c_str());

	// Do not continue on error loading/parsing the file
	if (xml_res.status != pugi::xml_parse_status::status_ok)
	{
		ERR("TmxMap: Error loading/parsing file (" << m_filePath << ")!");
		return;
	}

	// .tmx <map> info start
	auto & child_map = xml_doc.child("map");

	m_mapData.version = child_map.attribute("version").value();
	m_mapData.orientation = child_map.attribute("orientation").value();
	m_mapData.renderorder = child_map.attribute("renderorder").value();
	uint32_t map_width = m_mapData.width = std::stoi(child_map.attribute("width").value());
	uint32_t map_height = m_mapData.height = std::stoi(child_map.attribute("height").value());
	uint32_t tile_width = m_mapData.tilewidth = std::stoi(child_map.attribute("tilewidth").value());
	uint32_t tile_height = m_mapData.tileheight = std::stoi(child_map.attribute("tileheight").value());

	// .tmx <tileset> info start
	auto & child_tileset = child_map.child("tileset");

	m_mapData.tileset.firstgid = std::stoi(child_tileset.attribute("firstgid").value());
	m_mapData.tileset.name = child_tileset.attribute("name").value();
	m_mapData.tileset.source = child_tileset.child("image").attribute("source").value();
	m_mapData.tileset.width = std::stoi(child_tileset.child("image").attribute("width").value());
	m_mapData.tileset.height = std::stoi(child_tileset.child("image").attribute("height").value());
	m_mapData.tileset.tilewidth = std::stoi(child_tileset.attribute("tilewidth").value());
	m_mapData.tileset.tileheight = std::stoi(child_tileset.attribute("tileheight").value());

	// .tmx <tileset> tiles
	for (auto & tile = child_tileset.child("tile"); tile; tile = tile.next_sibling("tile"))
	{
		// Get tile properties
		uint32_t tile_id = std::stoi(tile.attribute("id").value());

		auto & child_tile_prop = tile.child("properties");
		for (auto & prop = child_tile_prop.first_child(); prop; prop = prop.next_sibling())
		{
			TmxTilePropertyData tile_property = std::make_pair(prop.attribute("name").value(), prop.attribute("value").value());
			m_mapData.tileset.tileproperties[tile_id].push_back(tile_property);
		}
	}

	// .tmx <layer> info start
	for (auto & layer = child_map.child("layer"); layer; layer = layer.next_sibling("layer"))
	{
		// Initialize new tile layer
		m_mapData.layer.push_back(TmxLayerData());

		// Get layer basic properties
		std::string layer_name = m_mapData.layer.back().name = layer.attribute("name").value();
		uint32_t layer_width = m_mapData.layer.back().width = std::stoi(layer.attribute("width").value());
		uint32_t layer_height = m_mapData.layer.back().height = std::stoi(layer.attribute("height").value());

		// Get layer properties
		auto & child_layer_prop = layer.child("properties");
		for (auto & prop = child_layer_prop.first_child(); prop; prop = prop.next_sibling())
		{
			TmxTilePropertyData layer_property = std::make_pair(prop.attribute("name").value(), prop.attribute("value").value());
			m_mapData.layer.back().layerproperties.push_back(layer_property);
		}

		// Get layer properties
		auto layer_properties = m_mapData.layer.back().layerproperties;

		// Parse all layer tiles
		uint32_t tile_index = 0;
		auto & child_layer_data = layer.child("data");
		for (auto & tile = child_layer_data.first_child(); tile; tile = tile.next_sibling())
		{
			// Get tile gid
			uint32_t tile_gid = std::stoi(tile.attribute("gid").value());

			if (tile_gid != 0)
			{
				// Decrement tile gid by one
				tile_gid--;

				// Calculate tile coordinates and indices
				int32_t tile_x = m_mapData.tileset.tilewidth * (tile_index % layer_width);
				int32_t tile_y = (m_mapData.tileset.tileheight * (layer_height - static_cast<int32_t>(std::ceil(tile_index / layer_width)))) - tile_height;
				uint32_t tileset_x = m_mapData.tileset.tilewidth * (tile_gid % (m_mapData.tileset.width / m_mapData.tileset.tilewidth));
				uint32_t tileset_y = m_mapData.tileset.tileheight * static_cast<uint32_t>(std::ceil(tile_gid / (m_mapData.tileset.width / m_mapData.tileset.tilewidth)));

				// Fuse individual tile properties and the tile layer properties together
				auto tile_properties = m_mapData.tileset.tileproperties[tile_gid];
				tile_properties.insert(tile_properties.end(), layer_properties.begin(), layer_properties.end());

				// Insert the new tile into our layer's data
				m_mapData.layer.back().tiles.push_back(Tile(
					game,
					Sprite(
						game.getResMan()->loadTexture(m_mapData.tileset.source),
						tileset_x,
						tileset_y,
						m_mapData.tileset.tilewidth,
						m_mapData.tileset.tileheight
					),
					vec2(static_cast<float>(tile_x), static_cast<float>(tile_y)),
					Tile::strToLayer(layer_name),
					Tile::strToProperties(tile_properties)
				));
			}

			// Increment tile index by one
			tile_index++;
		}
	}

	// .tmx <imagelayer> info
	for (auto & ilayer = child_map.child("imagelayer"); ilayer; ilayer = ilayer.next_sibling("imagelayer"))
	{
		// Initialize new image layer
		m_mapData.imglayer.push_back(TmxImgLayerData());

		// Get layer basic properties
		m_mapData.imglayer.back().name = ilayer.attribute("name").value();

		// Parse all layer images
		uint32_t ilayer_index = 0;
		for (auto & image = ilayer.child("image"); image; image = image.next_sibling("image"))
		{
			// Get image properties
			std::string ilayer_src = m_mapData.imglayer.back().source = image.attribute("source").value();

			// Increment image index by one
			ilayer_index++;
		}
	}

	// .tmx <objectgroup> info
	for (auto & ogroup = child_map.child("objectgroup"); ogroup; ogroup = ogroup.next_sibling("objectgroup"))
	{
		// Initialize new object group
		m_mapData.objectgroup.push_back(TmxObjectgroupData());

		// Get layer basic properties
		m_mapData.objectgroup.back().name = ogroup.attribute("name").value();

		// Parse all layer objects
		uint32_t object_index = 0;
		for (auto & object = ogroup.child("object"); object; object = object.next_sibling("object"))
		{
			// Calculate object properties and so on
			uint32_t object_id = std::stoi(object.attribute("id").value());
			std::string object_name = object.attribute("name").value();
			std::string object_type = object.attribute("type").value();
			int32_t object_x = std::stoi(object.attribute("x").value());
			int32_t object_y = (map_height * tile_height) - std::stoi(object.attribute("y").value());
			int32_t object_width = std::stoi(object.attribute("width").value());
			int32_t object_height = std::stoi(object.attribute("height").value());

			// Insert the object into our layer's data
			m_mapData.objectgroup.back().objects.push_back(TmxObject{ object_id, object_name, object_type, object_x, object_y, object_width, object_height });

			// Increment object index by one
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
