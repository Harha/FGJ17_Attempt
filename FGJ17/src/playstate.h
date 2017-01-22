#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <string>
#include "gamestate.h"
#include "level.h"

class PlayState : public GameState
{
public:
	PlayState(Game * const game, const std::string & lvlPath);
	virtual ~PlayState() override;
	virtual void update(double t, double dt) override;
	virtual void render(Display * const display) override;
private:
	Level m_level;
};

#endif // PLAYSTATE_H
