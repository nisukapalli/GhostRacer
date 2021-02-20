#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, int dir, double size, int depth, StudentWorld* world, bool avoidanceWorthy);
    virtual ~Actor() {}
    virtual void doSomething() = 0;
    StudentWorld* getWorld();
    int getVertSpeed();
    int getHorizSpeed();
    void setVertSpeed(int speed);
    void setHorizSpeed(int speed);
    int getHealth();
    void setHealth(int health);
    bool isAlive();
    bool isAvoidanceWorthy();
private:
    StudentWorld* m_world;
    int m_vertSpeed;
    int m_horizSpeed;
    int m_health;
    bool m_alive;
    bool m_avoidanceWorthy;
};


class GhostRacer : public Actor
{
public:
    GhostRacer(StudentWorld* world);
    ~GhostRacer() {}
    void doSomething();
    void movement();
    int getSprays();
private:
    int m_holyWater;
};


class BorderLine : public Actor
{
public:
    BorderLine(int imageID, int startX, int startY, StudentWorld* world);
    ~BorderLine() {}
    void doSomething();
private:

};

#endif // ACTOR_H_
