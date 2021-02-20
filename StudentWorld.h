#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    GhostRacer* getPlayer();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

private:
    GhostRacer* m_player;
    vector<Actor*> m_actors;
    int m_lastWhiteBorderY;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
