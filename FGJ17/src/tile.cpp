#include "tile.h"
#include "game.h"
#include "display.h"


Tile::Tile(
	Game * const game,
	Sprite sprite,
	vec2 position,
	TileLayer layer,
	TileProperties properties
) :
	m_sprite(sprite),
	m_position(position),
	m_aabb(
		m_position - m_sprite.getDimensions() * 0.5f + m_sprite.getDimensions() * 0.5f,
		m_position + m_sprite.getDimensions() * 0.5f + m_sprite.getDimensions() * 0.5f
	),
	m_layer(layer),
	m_properties(properties)
{

}

void Tile::render(Display * const display)
{
	m_sprite.render(display, m_position);
}

void Tile::renderAABB(Display * const display)
{
	SDL_SetRenderDrawColor(display->getRenderer(), 255, 0, 0, 255);
	m_sprite.renderOutline(display, m_position);
}

void Tile::setPosition(const vec2 &position)
{
	m_position = position;
}

void Tile::setLayer(TileLayer l)
{
	m_layer = l;
}

vec2 Tile::getPosition() const
{
	return m_position;
}

AABB Tile::getAABB() const
{
	return m_aabb;
}

TileLayer Tile::getLayer() const
{
	return m_layer;
}

TileProperties Tile::getProperties() const
{
	return m_properties;
}

bool Tile::hasPropertyWithValue(TilePropertyName prop_name, TilePropertyValue prop_value) const
{
	for (auto & prop : m_properties)
	{
		if (prop.first == prop_name)
		{
			switch (prop.second.type)
			{
			case TPV_STRING:
				if (prop.second.value_str == prop_value.value_str)
					return true;
				break;
			case TPV_NUMBER:
				if (prop.second.value_num == prop_value.value_num)
					return true;
				break;
			}
		}
	}

	return false;
}