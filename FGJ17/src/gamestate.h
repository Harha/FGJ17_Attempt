#ifndef GAMESTATE_H
#define GAMESTATE_H

class Game;
class Display;

class GameState
{
public:
    GameState(Game * const game);
    virtual ~GameState();
    virtual void update(double t, double dt) = 0;
    virtual void render(Display * const display) = 0;
protected:
    Game * const m_game;
};

#endif // GAMESTATE_H
