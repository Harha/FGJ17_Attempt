#include "player.h"
#include <SDL.h>
#include <vector>
#include <map>
#include "game.h"
#include "resmanager.h"
#include "sprite.h"
#include "aabb.h"
#include "level.h"

// Player class
Player::Player(
	Game * const game,
	const vec2 & spawn,
	EntityProperties properties
) :
	Entity(
		game,
		"player",
		spawn,
		properties
	)
{

}

void Player::update(Level & lvl, double t, double dt)
{
	// Player input
	if (m_game->getInputKeys()[SDL_SCANCODE_UP])
		m_input.keyUp = true;
	else
		m_input.keyUp = false;

	if (m_game->getInputKeys()[SDL_SCANCODE_RIGHT])
		m_input.keyRight = true;
	else
		m_input.keyRight = false;

	if (m_game->getInputKeys()[SDL_SCANCODE_LEFT])
		m_input.keyLeft = true;
	else
		m_input.keyLeft = false;

	if (m_game->getInputKeys()[SDL_SCANCODE_Z])
		m_input.keyB = true;
	else
		m_input.keyB = false;

	// Sprite animation while grounded
	if (m_state == ENTITY_GROUNDED)
	{
		if (m_moveDirX == ENTITY_RIGHT)
			setCurrentSprite("RUN_RIGHT", getCurrentSprite().getSprAnimTime(), -1);
		else if (m_moveDirX == ENTITY_LEFT)
			setCurrentSprite("RUN_LEFT", getCurrentSprite().getSprAnimTime(), -1);

		getCurrentSprite().setSprAnimRate(static_cast<int32_t>(std::abs(m_velocity.x) * 0.075));

		if (m_input.keyUp)
		{
			getCurrentSprite().setSprAnimTime(0.0);
			getCurrentSprite().setSprAnimFrame(0);
		}
	}

	// Sprite animation while on air
	if (m_state == ENTITY_FLYING)
	{
		if (m_moveDirX == ENTITY_RIGHT)
			setCurrentSprite("JMP_RIGHT", getCurrentSprite().getSprAnimTime(), getCurrentSprite().getSprAnimFrame());
		else if (m_moveDirX == ENTITY_LEFT)
			setCurrentSprite("JMP_LEFT", getCurrentSprite().getSprAnimTime(), getCurrentSprite().getSprAnimFrame());

		if (m_moveDirY == ENTITY_DOWN)
			getCurrentSprite().setSprAnimFrame(2);
	}

	// Update the current sprite
	getCurrentSprite().update(t, dt);

	// Normal entity updates
	Entity::update(lvl, t, dt);
}
