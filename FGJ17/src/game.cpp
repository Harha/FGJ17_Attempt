#include "game.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include "display.h"
#include "resmanager.h"
#include "gamestate.h"
#include "playstate.h"
#include "macros.h"
#include "tools.h"

Game::Game(Display * const display, double timeStep, uint32_t tickRate, uint32_t frameRate) :
	m_display(display),
	m_resMan(new ResManager(this)),
	m_gameStates(),
	m_inputKeys(SDL_GetKeyboardState(NULL)),
	m_startTime(0),
	m_timeStep(timeStep),
	m_ticks(0),
	m_tickTime(1000.0 / static_cast<double>(tickRate)),
	m_frameTime(1000.0 / static_cast<double>(frameRate)),
	m_deltaUpTime(m_tickTime),
	m_deltaReTime(m_frameTime),
	m_runState(GRS_INITIALIZED)
{
	// Load some resources
	GAMEFONT = m_resMan->loadFont("./data/fonts/UpheavalPro.ttf", 16);
	GAMEMUSIC = m_resMan->loadMusic("./data/music/Prime & Some1 - Turtle Race!.mod");

	// Pust the initial gamestate into stack
	m_gameStates.push(new PlayState(this, "./data/levels/level_0-0.tmx"));

	// Setup some settings
	setMusicVolume(5);
}

Game::~Game()
{
	while (!m_gameStates.empty())
	{
		DELETE_SP(m_gameStates.top());
		m_gameStates.pop();
	}

	DELETE_SP(m_resMan);
}

GameRunState Game::run()
{
	if (m_runState != GRS_INITIALIZED)
	{
		ERR("Game: Game::run() called before game was initialized properly.");
		return m_runState;
	}

	// Initialize SDL related stuff
	SDL_Event event;
	playMusic(GAMEMUSIC);

	// Physics/Timing related stuff
	m_ticks = m_startTime = getTicksInMs();
	double lastUpdate = m_ticks, lastRender = m_ticks;
	double accumulator = 0.0;

	LOG("Game: Game::run() called, starting game main loop.");

	m_runState = GRS_RUNNING_DBG;
	while (m_runState == GRS_RUNNING || m_runState == GRS_RUNNING_DBG)
	{
		m_ticks = getTicksInMs();

		if (m_ticks >= lastUpdate + m_tickTime)
		{
			m_deltaUpTime = m_ticks - lastUpdate;
			accumulator += m_deltaUpTime;

			while (accumulator >= m_timeStep)
			{
				update();
				accumulator -= m_tickTime;
			}

			lastUpdate = m_ticks;
		}

		if (m_ticks >= lastRender + m_frameTime)
		{
			m_deltaReTime = m_ticks - lastRender;
			render();
			lastRender = m_ticks;
		}

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				m_runState = GRS_STOPPED;
				break;
			}

			if (m_inputKeys[SDL_SCANCODE_ESCAPE])
			{
				m_runState = GRS_STOPPED;
			}
		}

		// SDL_Delay(1);
	}

	return m_runState;
}

void Game::update()
{
	if (!m_gameStates.empty())
	{
		m_gameStates.top()->update(getCurrentTimeInMs() * 1e-3f, m_timeStep);
	}
}

void Game::render()
{
	m_display->clear();

	if (!m_gameStates.empty())
	{
		m_gameStates.top()->render(m_display);
	}

	m_display->render();
}

void Game::playMusic(Mix_Music * music, int32_t loops)
{
	Mix_PlayMusic(music, loops);
}

void Game::setMusicVolume(int32_t volume)
{
	Mix_VolumeMusic(volume);
}

Display * const Game::getDisplay() const
{
	return m_display;
}

ResManager * const Game::getResMan() const
{
	return m_resMan;
}

const uint8_t * const Game::getInputKeys() const
{
	return m_inputKeys;
}

double Game::getCurrentTimeInMs() const
{
	return getTicksInMs() - m_startTime;
}

double Game::getDeltaUpTime() const
{
	return m_deltaUpTime;
}

double Game::getDeltaReTime() const
{
	return m_deltaReTime;
}

double Game::getTicksInMs() const
{
	return static_cast<double>((SDL_GetPerformanceCounter() * 1000) / SDL_GetPerformanceFrequency());
}

GameRunState Game::getRunState() const
{
	return m_runState;
}