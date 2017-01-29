#include "playstate.h"
#include <SDL.h>
#include "game.h"
#include "display.h"
#include "entity.h"

PlayState::PlayState(Game * const game, Level * level) :
	GameState(game),
	m_level(level)
{

}

PlayState::~PlayState()
{

}

void PlayState::update(double t, double dt)
{
	m_level->update(t, dt);
}

void PlayState::render(Display * const display)
{
	m_level->render(display);

	// Draw debug information
	if (m_game->getRunState() == GRS_RUNNING_DBG)
	{
		// Prepare font
		TTF_Font * font = m_game->getGameFont()[0];
		SDL_Color textcolor{ 255, 255, 255, 255 };

		// Draw the text
		display->drawText(font, "FPS: " + std::to_string(1000.0 / m_game->getDeltaReTime()), textcolor, vec2(2, 2));
		display->drawText(font, "UPS: " + std::to_string(1000.0 / m_game->getDeltaUpTime()), textcolor, vec2(2, 18));
		display->drawText(font, "Game STATE: " + std::to_string(m_game->getRunState()), textcolor, vec2(2, 34));
		display->drawText(font, "Player STATE: " + std::to_string(m_level->getPlayer()->getState()), textcolor, vec2(2, 50));
		display->drawText(font, "Player Tile collisions: " + std::to_string(m_level->getPlayer()->getCurrentTileCollisions().size()), textcolor, vec2(2, 66));
		display->drawText(font, "Player Entity collisions: " + std::to_string(m_level->getPlayer()->getCurrentEntityCollisions().size()), textcolor, vec2(2, 82));
		display->drawText(font, "Entities: " + std::to_string(m_level->getEntityVector().size()), textcolor, vec2(2, 98));
		display->drawText(font, "Player s: " + m_level->getPlayer()->getPosition().toString(), textcolor, vec2(2, 114));
		display->drawText(font, "Player v: " + m_level->getPlayer()->getVelocity().toString(), textcolor, vec2(2, 130));
		display->drawText(font, "Player MoveDirX: " + std::to_string(m_level->getPlayer()->getMoveDirX()), textcolor, vec2(2, 146));
		display->drawText(font, "Player MoveDirY: " + std::to_string(m_level->getPlayer()->getMoveDirY()), textcolor, vec2(2, 162));
	}
}
