#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#define _USE_MATH_DEFINES

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, int startX, int startY, int dir, double size, int depth, StudentWorld* world, bool avoidanceWorthy)
: GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_world = world;
    m_alive = true;
    m_avoidanceWorthy = avoidanceWorthy;
}

StudentWorld* Actor::getWorld()
{
    return m_world;
}

int Actor::getVertSpeed()
{
    return m_vertSpeed;
}

int Actor::getHorizSpeed()
{
    return m_horizSpeed;
}

void Actor::setVertSpeed(int speed)
{
    m_vertSpeed = speed;
}

void Actor::setHorizSpeed(int speed)
{
    m_horizSpeed = speed;
}

int Actor::getHealth()
{
    return m_health;
}

void Actor::setHealth(int health)
{
    m_health = health;
    if (m_health <= 0)
    {
        m_alive = false;
    }
}

bool Actor::isAlive()
{
    return m_health > 0;
}

bool Actor::isAvoidanceWorthy()
{
    return m_avoidanceWorthy;
}


GhostRacer::GhostRacer(StudentWorld* world)
: Actor(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, world, true)
{
    setVertSpeed(0);
    setHorizSpeed(0);
    setHealth(100);
    m_holyWater = 10;
}

void GhostRacer::doSomething()
{
    if (! isAlive()) {
        return;
    }
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    if (getX() <= LEFT_EDGE)
    {
        if (getDirection() > 90)
        {
            setHealth(getHealth() - 10);
        }
        setDirection(82);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
        movement();
    }
    if (getX() >= RIGHT_EDGE)
    {
        if (getDirection() < 90)
        {
            setHealth(getHealth() - 10);
        }
        setDirection(98);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
        movement();
    }
    
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                if (getDirection() < 114)
                {
                    setDirection(getDirection()+8);
                    movement();
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() > 66)
                {
                    setDirection(getDirection()-8);
                    movement();
                }
                break;
            case KEY_PRESS_UP:
                if (getVertSpeed() < 5)
                {
                    setVertSpeed(getVertSpeed()+1);
                }
                break;
            case KEY_PRESS_DOWN:
                if (getVertSpeed() > -1)
                {
                    setVertSpeed(getVertSpeed()-1);
                }
                break;
            case KEY_PRESS_SPACE:
                //... add spray in front of Ghost Racer...;
                break;
            // etc…
        }
    }
    //...
}

void GhostRacer::movement()
{
    double max_shift_per_tick = 4.0;
    double direction = getDirection() * M_PI/180;
    double delta_x = cos(direction) * max_shift_per_tick;
    int cur_x = getX();
    int cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

int GhostRacer::getSprays()
{
    return m_holyWater;
}


BorderLine::BorderLine(int imageID, int startX, int startY, StudentWorld* world)
: Actor(imageID, startX, startY, 0, 2.0, 2, world, false)
{
    setVertSpeed(-4);
    setHorizSpeed(0);
    setHealth(100);
}

void BorderLine::doSomething()
{
    int vert_speed = getVertSpeed() - getWorld()->getPlayer()->getVertSpeed();
    int horiz_speed = getHorizSpeed();
    int new_y = getY() + vert_speed;
    int new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT)
    {
        setHealth(0);
        return;
    }
}
