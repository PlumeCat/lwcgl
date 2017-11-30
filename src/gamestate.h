#ifndef _CUBE_GAMESTATE_H
#define _CUBE_GAMESTATE_H

#include <string>

class Game;

struct GameState
{
private:
    friend class Game;
protected:
    Game* game;
public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void close() = 0;
};

#endif