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
	m_gameLevels(),

	// Physics
	m_startTime(0),
	m_timeStep(1e-2),
	m_ticks(0),
	m_tickTime(1000.0 / 80.0),
	m_deltaUpTime(m_tickTime),
	m_physicsDistance(1),

	// Graphics
	m_display(nullptr),
	m_deltaReTime(m_frameTime),
	m_frameTime(1000.0 / 128.0),
	m_renderDistance(1)
{
	// Load config.json file
	std::ifstream cfgFile("./data/config.json", std::ifstream::binary);

	// Throw if loading config file failed
	if (cfgFile.is_open() == false)
	{
		throw std::exception(std::string("Error: Can't find config.json from given filepath. Filepath: " + cfgFilePath).c_str());
	}

	// Assign the contents of config.json to game config json object
	cfgFile >> m_config;
	cfgFile.close();

	// Get game JSON object
	json & json_game = m_config["game"];

	// Parse window configuration, create display
	json & json_window = json_game["window"];
	m_display = new Display(
		json_window["title"].get<std::string>(),
		json_window["width"].get<int32_t>(),
		json_window["height"].get<int32_t>(),
		json_window["scale"].get<int32_t>()
	);

	// Set to fullscreen if config says so
	if (json_window["fullscreen"].get<bool>() == true)
		m_display->setState(SDL_WINDOW_FULLSCREEN);

	// Configure physics
	json & json_phys = json_game["physics"];
	m_timeStep = json_phys["timeStep"].get<double>();
	m_tickTime = 1000.0 / json_phys["tickRate"].get<double>();
	m_deltaUpTime = m_tickTime;
	m_physicsDistance = json_phys["physicsDistance"].get<int32_t>();

	// Configure graphics
	json & json_graph = json_game["graphics"];
	m_frameTime = 1000.0 / json_graph["frameRate"].get<double>();
	m_deltaReTime = m_frameTime;
	m_renderDistance = json_graph["renderDistance"].get<int32_t>();

	// Load fonts
	json & json_fonts = json_game["fonts"];
	size_t n_fonts = json_fonts.size();
	for (size_t i = 0; i < n_fonts; i++)
	{
		json & json_font = json_fonts[i];
		m_gameFont.push_back(m_resMan->loadFont("./data/fonts/" + json_font["fileName"].get<std::string>(), json_font["fontSize"].get<uint32_t>()));
	}

	// Load musicz
	json & json_music = json_game["music"];
	size_t n_music = json_music.size();
	for (size_t i = 0; i < n_music; i++)
	{
		m_gameMusic.push_back(m_resMan->loadMusic("./data/music/" + json_music[i].get<std::string>()));
	}

	// Load levels
	json & json_levels = json_game["levels"];
	size_t n_levels = json_levels.size();
	for (size_t i = 0; i < n_levels; i++)
	{
		m_gameLevels.push_back(m_resMan->loadLevel("./data/levels/" + json_levels[i].get<std::string>() + ".tmx", json_levels[i].get<std::string>()));
	}

	// Pust the initial gamestate into stack
	m_gameStates.push(new PlayState(this, m_gameLevels[0]));

	// Setup some settings
	setMusicSong(m_gameMusic[1]);
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

std::vector<Level *> Game::getGameLevels() const
{
	return m_gameLevels;
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

int32_t Game::getPhysicsDistance() const
{
	return m_physicsDistance;
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

int32_t Game::getRenderDistance() const
{
	return m_renderDistance;
}