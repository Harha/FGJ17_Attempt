#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <stack>

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
	Game(Display * const display, double timeStep = 1e-2, uint32_t tickRate = 80, uint32_t frameRate = 128);
	~Game();
	GameRunState run();
	void update();
	void render();
	void playMusic(Mix_Music * music, int32_t loops = -1);
	void setMusicVolume(int32_t volume);
	Display * const getDisplay() const;
	ResManager * const getResMan() const;
	const uint8_t * const getInputKeys() const;
	double getCurrentTimeInMs() const;
	double getDeltaUpTime() const;
	double getDeltaReTime() const;
	double getTicksInMs() const;
	GameRunState getRunState() const;

	TTF_Font * GAMEFONT;
	Mix_Music * GAMEMUSIC;
private:
	Display * const m_display;
	ResManager * m_resMan;
	std::stack<GameState *> m_gameStates;
	const uint8_t * const m_inputKeys;
	double m_startTime;
	double m_timeStep;
	double m_ticks;
	double m_tickTime;
	double m_frameTime;
	double m_deltaUpTime;
	double m_deltaReTime;
	GameRunState m_runState;
};

#endif // GAME_H
