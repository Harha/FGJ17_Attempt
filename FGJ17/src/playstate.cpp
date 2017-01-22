#include "playstate.h"
#include <SDL.h>
#include "game.h"
#include "display.h"
#include "entity.h"

PlayState::PlayState(Game * const game, const std::string & lvlPath) :
	GameState(game),
	m_level(game, lvlPath)
{

}

PlayState::~PlayState()
{

}

void PlayState::update(double t, double dt)
{
	m_level.update(t, dt);
}

void PlayState::render(Display * const display)
{
	m_level.render(display);

	// Draw debug information
	if (m_game->getRunState() == GRS_RUNNING_DBG)
	{
		SDL_Color textcolor{ 255, 255, 255, 255 };
		display->drawText(m_game->GAMEFONT, "FPS: " + std::to_string(1000.0 / m_game->getDeltaReTime()), textcolor, vec2(2, 2));
		display->drawText(m_game->GAMEFONT, "UPS: " + std::to_string(1000.0 / m_game->getDeltaUpTime()), textcolor, vec2(2, 18));
		display->drawText(m_game->GAMEFONT, "Game STATE: " + std::to_string(m_game->getRunState()), textcolor, vec2(2, 34));
		display->drawText(m_game->GAMEFONT, "Player STATE: " + std::to_string(m_level.getPlayer()->getState()), textcolor, vec2(2, 50));
		display->drawText(m_game->GAMEFONT, "Entities: " + std::to_string(m_level.getEntityVector().size()), textcolor, vec2(2, 66));
		display->drawText(m_game->GAMEFONT, "Player s: " + m_level.getPlayer()->getPosition().toString(), textcolor, vec2(2, 82));
		display->drawText(m_game->GAMEFONT, "Player v: " + m_level.getPlayer()->getVelocity().toString(), textcolor, vec2(2, 98));
		display->drawText(m_game->GAMEFONT, "Player MoveDirX: " + std::to_string(m_level.getPlayer()->getMoveDirX()), textcolor, vec2(2, 114));
		display->drawText(m_game->GAMEFONT, "Player MoveDirY: " + std::to_string(m_level.getPlayer()->getMoveDirY()), textcolor, vec2(2, 130));
	}
}
