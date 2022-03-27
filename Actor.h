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
    void moveActor();
    bool objectOverlap(Actor* other);
    StudentWorld* getWorld();
    double getSize();
    double getVertSpeed();
    double getHorizSpeed();
    void setVertSpeed(double speed);
    void setHorizSpeed(double speed);
    int getHealth();
    void setHealth(int health);
    bool isAlive();
    bool isAvoidanceWorthy();
    virtual bool isPed();
    virtual bool canAddNewBorders();
    virtual bool isDestroyable();
private:
    StudentWorld* m_world;
    double m_size;
    double m_vertSpeed;
    double m_horizSpeed;
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
    void setSprays(int sprays);
private:
    int m_holyWater;
};


class BorderLine : public Actor
{
public:
    BorderLine(int imageID, int startX, int startY, StudentWorld* world);
    ~BorderLine() {}
    void doSomething();
    virtual bool canAddNewBorders();
private:

};


class Pedestrian : public Actor
{
public:
    Pedestrian(int imageID, int startX, int startY, double size, StudentWorld* world);
    ~Pedestrian() {}
    int getMovementPlanDistance();
    void setMovementPlanDistance(int distance);
    void setNewMovementPlan();
    virtual bool isPed();
private:
    int m_movementPlanDistance;
};


class ZombiePed : public Pedestrian
{
public:
    ZombiePed(int startX, int startY, StudentWorld* world);
    ~ZombiePed() {}
    void doSomething();
    virtual bool isDestroyable();
private:
    int m_ticks;
};


class HumanPed : public Pedestrian
{
public:
    HumanPed(int startX, int startY, StudentWorld* world);
    ~HumanPed() {}
    void doSomething();
private:
    
};


class ZombieCab : public Actor
{
public:
    ZombieCab(int startX, int startY, double vert_speed, StudentWorld* world);
    ~ZombieCab() {}
    void doSomething();
    virtual bool isDestroyable();
private:
    int m_planLength;
    bool m_damagedPlayer;
};

class OilSlick : public Actor
{
public:
    OilSlick(int startX, int startY, double size, StudentWorld* world);
    ~OilSlick() {}
    void doSomething();
private:
    
};


class HolyWaterProjectile : public Actor
{
public:
    HolyWaterProjectile(int startX, int startY, int dir, StudentWorld* world);
    ~HolyWaterProjectile() {}
    void doSomething();
private:
    int m_startX;
    int m_startY;
    int m_maxDistance;
};


class LostSoul : public Actor
{
public:
    LostSoul(int startX, int startY, StudentWorld* world);
    ~LostSoul() {}
    void doSomething();
private:
    
};


class Goodies : public Actor
{
public:
    Goodies(int imageID, int startX, int startY, int dir, double size, int depth, StudentWorld* world);
    ~Goodies() {}
    void doSomething();
    virtual bool isDestroyable();
private:
    
};


class Healing : public Goodies
{
public:
    Healing(int startX, int startY, StudentWorld* world);
    ~Healing() {}
    void doSomething();
private:
    
};


class HolyWaterRefill : public Goodies
{
public:
    HolyWaterRefill(int startX, int startY, StudentWorld* world);
    ~HolyWaterRefill() {}
    void doSomething();
private:
    
};

#endif // ACTOR_H_
