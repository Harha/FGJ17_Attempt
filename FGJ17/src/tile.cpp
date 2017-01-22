#include "tile.h"
#include "game.h"
#include "display.h"


Tile::Tile(Game &game, Sprite sprite, vec2 position, TileLayer layer, TileType type) :
	m_sprite(sprite),
	m_position(position),
	m_aabb(m_position - m_sprite.getDimensions() * 0.5f + m_sprite.getDimensions() * 0.5f,
		m_position + m_sprite.getDimensions() * 0.5f + m_sprite.getDimensions() * 0.5f),
	m_layer(layer),
	m_type(type)
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

TileType Tile::getType() const
{
	return m_type;
}
