#include "box.h"
#include "game.h"
#include "tile.h"

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
	),
	m_triggered(false)
{
	// Choose box color
	for (auto & prop : m_properties)
	{
		if (prop.first == EPN_ID)
		{
			switch (prop.second.value_num)
			{
			case 0:
				m_currentSprite = "BOX_RED";
				break;
			case 1:
				m_currentSprite = "BOX_BLUE";
				break;
			case 2:
				m_currentSprite = "BOX_GREEN";
				break;
			case 3:
				m_currentSprite = "BOX_YELLOW";
				break;
			case 4:
				m_currentSprite = "BOX_PURPLE";
				break;
			}
		}
	}
}

void Box::update(Level & lvl, double t, double dt)
{
	// Trigger state of current frame
	bool triggered = false;

	// Check for triggers around us
	for (Tile * t : m_currentTileCollisions)
	{
		if (t->hasPropertyWithValue(TPN_TYPE, TilePropertyValue(TPV_STRING, "TRIGGER", NULL)))
		{
			triggered = true;

			// This is a one shot signal
			if (m_triggered == false)
				LOG_INFO("Box: Box::update Detected collision with a trigger tile!");

			m_triggered = true;
		}
	}

	// Reset trigger information if no triggers nearby
	if (m_triggered == true && triggered == false)
		m_triggered = false;

	// Normal entity updates
	Entity::update(lvl, t, dt);
}