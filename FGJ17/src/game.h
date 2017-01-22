#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <stack>
#include "3rdparty/json.hpp"

using json = nlohmann::json;

class Display;
class ResManager;
class GameState;
typedef struct _TTF_Font TTF_Font;
typedef struct _Mix_Music Mix_Music;

enum GameRunState
{
	GRS_ERROR = -1,
	GRS_STOPPED = 0,
	GRS_RUNNING = 1,
	GRS_RUNNING_DBG = 2,
	GRS_INITIALIZED = 3
};

class Game
{
public:
	Game(const std::string & cfgFilePath = "./data/config.json");
	~Game();
	GameRunState run();
	void update();
	void render();
	void setMusicSong(Mix_Music * music, int32_t loops = -1);
	void setMusicVolume(int32_t volume);

	// Game state
	json & getConfig();
	GameRunState getRunState() const;
	const uint8_t * const getInputKeys() const;

	// Resources
	ResManager * const getResMan() const;
	std::vector<TTF_Font *> getGameFont() const;
	std::vector<Mix_Music *> getGameMusic() const;

	// Physics
	double getCurrentTimeInMs() const;
	double getDeltaUpTime() const;
	double getTicksInMs() const;

	// Graphics
	Display * const getDisplay() const;
	double getDeltaReTime() const;
private:
	// Game state
	json m_config;
	GameRunState m_runState;
	std::stack<GameState *> m_gameStates;
	const uint8_t * const m_inputKeys;

	// Resources
	ResManager * m_resMan;
	std::vector<TTF_Font *> m_gameFont;
	std::vector<Mix_Music *> m_gameMusic;

	// Physics
	double m_startTime;
	double m_timeStep;
	double m_ticks;
	double m_tickTime;
	double m_deltaUpTime;

	// Graphics
	Display * m_display;
	double m_frameTime;
	double m_deltaReTime;
};

#endif // GAME_H
