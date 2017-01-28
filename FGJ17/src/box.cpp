#include "box.h"
#include "game.h"

// Box class
Box::Box(
	Game * const game,
	const vec2 & spawn,
	EntityProperties properties
) :
	Entity(
		game,
		"box",
		spawn,
		properties
	)
{
	// Choose box color
	for (auto & prop : m_properties)
	{
		if (prop.first == EPN_COLOR)
		{
			switch (prop.second)
			{
			case EPV_RED:
				m_currentSprite = "BOX_RED";
				break;
			case EPV_BLUE:
				m_currentSprite = "BOX_BLUE";
				break;
			case EPV_GREEN:
				m_currentSprite = "BOX_GREEN";
				break;
			case EPV_YELLOW:
				m_currentSprite = "BOX_YELLOW";
				break;
			case EPV_PURPLE:
				m_currentSprite = "BOX_PURPLE";
				break;
			}
		}
	}
}

void Box::update(Level & lvl, double t, double dt)
{
	// Normal entity updates
	Entity::update(lvl, t, dt);
}