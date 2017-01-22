#include "game.h"
#include <fstream>
#include <exception>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "macros.h"
#include "tools.h"
#include "display.h"
#include "resmanager.h"
#include "gamestate.h"
#include "playstate.h"

Game::Game(const std::string & cfgFilePath) :
	// Game state
	m_config(),
	m_runState(GRS_INITIALIZED),
	m_gameStates(),
	m_inputKeys(SDL_GetKeyboardState(NULL)),

	// Resources
	m_resMan(new ResManager(this)),
	m_gameFont(),
	m_gameMusic(),

	// Physics
	m_startTime(0),
	m_timeStep(1e-2),
	m_ticks(0),
	m_tickTime(1000.0 / 80.0),
	m_deltaUpTime(m_tickTime),

	// Graphics
	m_display(nullptr),
	m_deltaReTime(m_frameTime),
	m_frameTime(1000.0 / 128.0)

{
	// Load config.json file
	std::ifstream cfgFile("./data/config.json", std::ifstream::binary);

	// Exit if loading config file failed
	if (cfgFile.is_open() == false)
	{
		throw std::exception(std::string("Error: Can't find config.json from given filepath. Filepath: " + cfgFilePath).c_str());
	}

	// Assign the contents of config.json to game config json object
	cfgFile >> m_config;
	cfgFile.close();

	// Create our display
	m_display = new Display(
		m_config["game"]["window"]["title"].get<std::string>(),
		m_config["game"]["window"]["width"].get<int32_t>(),
		m_config["game"]["window"]["height"].get<int32_t>(),
		m_config["game"]["window"]["scale"].get<int32_t>()
	);

	// Set to fullscreen if config says so
	if (m_config["game"]["window"]["fullscreen"].get<bool>() == true)
		m_display->setState(SDL_WINDOW_FULLSCREEN);

	// Configure physics
	m_timeStep = m_config["game"]["physics"]["timeStep"].get<double>();
	m_tickTime = 1000.0 / m_config["game"]["physics"]["tickRate"].get<double>();
	m_deltaUpTime = m_tickTime;

	// Configure graphics
	m_frameTime = 1000.0 / m_config["game"]["graphics"]["frameRate"].get<double>();
	m_deltaReTime = m_frameTime;

	// Load fonts
	size_t n_fonts = m_config["game"]["fonts"].size();
	for (size_t i = 0; i < n_fonts; i++)
	{
		json font = m_config["game"]["fonts"][i];
		m_gameFont.push_back(m_resMan->loadFont("./data/fonts/" + font["fileName"].get<std::string>(), font["fontSize"].get<uint32_t>()));
	}

	// Load musicz
	size_t n_music = m_config["game"]["music"].size();
	for (size_t i = 0; i < n_music; i++)
	{
		m_gameMusic.push_back(m_resMan->loadMusic("./data/music/" + m_config["game"]["music"][i].get<std::string>()));
	}

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
	setMusicSong(m_gameMusic.back());

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

void Game::setMusicSong(Mix_Music * music, int32_t loops)
{
	Mix_PlayMusic(music, loops);
}

void Game::setMusicVolume(int32_t volume)
{
	Mix_VolumeMusic(volume);
}

// Game state
json & Game::getConfig()
{
	return m_config;
}

GameRunState Game::getRunState() const
{
	return m_runState;
}

const uint8_t * const Game::getInputKeys() const
{
	return m_inputKeys;
}

// Resources
ResManager * const Game::getResMan() const
{
	return m_resMan;
}

std::vector<TTF_Font *> Game::getGameFont() const
{
	return m_gameFont;
}

std::vector<Mix_Music *> Game::getGameMusic() const
{
	return m_gameMusic;
}

// Physics
double Game::getCurrentTimeInMs() const
{
	return getTicksInMs() - m_startTime;
}

double Game::getDeltaUpTime() const
{
	return m_deltaUpTime;
}

double Game::getTicksInMs() const
{
	return static_cast<double>((SDL_GetPerformanceCounter() * 1000) / SDL_GetPerformanceFrequency());
}

// Graphics
Display * const Game::getDisplay() const
{
	return m_display;
}

double Game::getDeltaReTime() const
{
	return m_deltaReTime;
}