#include "player.h"
#include <SDL.h>
#include <vector>
#include <map>
#include "game.h"
#include "resmanager.h"
#include "sprite.h"
#include "aabb.h"
#include "level.h"

// Player sprite animation frames
SprAnimFrame PLR_RUN_L[4] = {
	SprAnimFrame(0, 0, 16, 16), SprAnimFrame(1, 0, 16, 16),
	SprAnimFrame(0, 0, 16, 16), SprAnimFrame(2, 0, 16, 16)
};

SprAnimFrame PLR_RUN_R[4] = {
	SprAnimFrame(0, 1, 16, 16), SprAnimFrame(1, 1, 16, 16),
	SprAnimFrame(0, 1, 16, 16), SprAnimFrame(2, 1, 16, 16)
};

SprAnimFrame PLR_JMP_L[3] = {
	SprAnimFrame(4, 0, 16, 16), SprAnimFrame(5, 0, 16, 16),
	SprAnimFrame(6, 0, 16, 16)
};

SprAnimFrame PLR_JMP_R[3] = {
	SprAnimFrame(4, 1, 16, 16), SprAnimFrame(5, 1, 16, 16),
	SprAnimFrame(6, 1, 16, 16)
};

std::vector<SprAnimFrame> PLR_RUN_L_V(PLR_RUN_L, PLR_RUN_L + sizeof(PLR_RUN_L) / sizeof(PLR_RUN_L[0]));
std::vector<SprAnimFrame> PLR_RUN_R_V(PLR_RUN_R, PLR_RUN_R + sizeof(PLR_RUN_R) / sizeof(PLR_RUN_R[0]));
std::vector<SprAnimFrame> PLR_JMP_L_V(PLR_JMP_L, PLR_JMP_L + sizeof(PLR_JMP_L) / sizeof(PLR_JMP_L[0]));
std::vector<SprAnimFrame> PLR_JMP_R_V(PLR_JMP_R, PLR_JMP_R + sizeof(PLR_JMP_R) / sizeof(PLR_JMP_R[0]));

// Player class
Player::Player(
	Game * const game,
	const vec2 & spawn
) :
	Entity(
		game,
		"player",
		spawn
	)
{
	m_spriteSheet.emplace("RUN_LEFT", Sprite(game->getResMan()->loadTexture("./data/images/cavestory.png"), PLR_RUN_L_V, true, 10));
	m_spriteSheet.emplace("RUN_RIGHT", Sprite(game->getResMan()->loadTexture("./data/images/cavestory.png"), PLR_RUN_R_V, true, 10));
	m_spriteSheet.emplace("JMP_LEFT", Sprite(game->getResMan()->loadTexture("./data/images/cavestory.png"), PLR_JMP_L_V, false, 2));
	m_spriteSheet.emplace("JMP_RIGHT", Sprite(game->getResMan()->loadTexture("./data/images/cavestory.png"), PLR_JMP_R_V, false, 2));
	m_currentSprite = "RUN_LEFT";
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

	// Sprite animation
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

	if (m_state == ENTITY_FLYING)
	{
		if (m_moveDirX == ENTITY_RIGHT)
			setCurrentSprite("JMP_RIGHT", getCurrentSprite().getSprAnimTime(), getCurrentSprite().getSprAnimFrame());
		else if (m_moveDirX == ENTITY_LEFT)
			setCurrentSprite("JMP_LEFT", getCurrentSprite().getSprAnimTime(), getCurrentSprite().getSprAnimFrame());

		if (m_moveDirY == ENTITY_DOWN)
			getCurrentSprite().setSprAnimFrame(2);
	}

	getCurrentSprite().update(t, dt);

	// Normal entity updates
	Entity::update(lvl, t, dt);
}
