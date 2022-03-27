#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#define _USE_MATH_DEFINES

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, int startX, int startY, int dir, double size, int depth, StudentWorld* world, bool avoidanceWorthy)
: GraphObject(imageID, startX, startY, dir, size, depth)
{
    m_world = world;
    m_size = size;
    m_alive = true;
    m_avoidanceWorthy = avoidanceWorthy;
}


void Actor::moveActor()
{
    double vert_speed = getVertSpeed() - getWorld()->getPlayer()->getVertSpeed();
    double horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    
    //move to new x and y coordinates
    moveTo(new_x, new_y);
    
    //set to not alive if moved off road
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT)
    {
        setHealth(0);
        return;
    }
}


bool Actor::objectOverlap(Actor* other)
{
    //conditions of overlap
    double delta_x = abs(this->getX() - other->getX());
    double delta_y = abs(this->getY() - other->getY());
    double radius_sum = (this->getSize() * 0.5) + (other->getSize() * 0.5);
    if (delta_x < radius_sum * 2.5 && delta_y < radius_sum * 6.0)
    {
        return true;
    }
    return false;
}


StudentWorld* Actor::getWorld()
{
    return m_world;
}


double Actor::getSize()
{
    return m_size;
}


double Actor::getVertSpeed()
{
    return m_vertSpeed;
}


double Actor::getHorizSpeed()
{
    return m_horizSpeed;
}


void Actor::setVertSpeed(double speed)
{
    m_vertSpeed = speed;
}


void Actor::setHorizSpeed(double speed)
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


bool Actor::isPed()
{
    return false;
}


bool Actor::canAddNewBorders()
{
    return false;
}


bool Actor::isDestroyable()
{
    return false;
}




//==============================================================================




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
    if (! isAlive())
    {
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
                if (m_holyWater > 0)
                {
                    int delta_x = getX() + SPRITE_HEIGHT * cos(getDirection());
                    int delta_y = getY() + SPRITE_HEIGHT * sin(getDirection());
                    getWorld()->pushActor(new HolyWaterProjectile(delta_x, delta_y, getDirection(), getWorld()));
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    m_holyWater--;
                }
                break;
        }
    }
    movement();
}


void GhostRacer::movement()
{
    double max_shift_per_tick = 4.0;
    double direction = getDirection() * M_PI/180;
    double delta_x = cos(direction) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}


int GhostRacer::getSprays()
{
    return m_holyWater;
}


void GhostRacer::setSprays(int sprays)
{
    m_holyWater += sprays;
}




//==============================================================================




BorderLine::BorderLine(int imageID, int startX, int startY, StudentWorld* world)
: Actor(imageID, startX, startY, 0, 2.0, 2, world, false)
{
    setVertSpeed(-4);
    setHorizSpeed(0);
    setHealth(100);
}


void BorderLine::doSomething()
{
    moveActor();
}

bool BorderLine::canAddNewBorders()
{
    return true;
}




//==============================================================================




Pedestrian::Pedestrian(int imageID, int startX, int startY, double size, StudentWorld* world)
: Actor(imageID, startX, startY, 0, size, 0, world, true)
{
    m_movementPlanDistance = 0;
    setVertSpeed(-4);
    setHorizSpeed(0);
    setHealth(2);
}


int Pedestrian::getMovementPlanDistance()
{
    return m_movementPlanDistance;
}


void Pedestrian::setMovementPlanDistance(int distance)
{
    m_movementPlanDistance = distance;
}


void Pedestrian::setNewMovementPlan()
{
    if (getMovementPlanDistance() > 0)
    {
        return;
    }
    
    //change horizontal speed
    int horiz_speed = randInt(1, 3);
    int randMultiplier = randInt(0, 1);
    if (randMultiplier == 0)
    {
        horiz_speed *= -1;
    }
    setHorizSpeed(horiz_speed);
    
    //change movement plan distance
    int dist = randInt(4, 32);
    setMovementPlanDistance(dist);
    
    //change direction
    if (horiz_speed < 0)
    {
        setDirection(180);
    }
    else
    {
        setDirection(0);
    }
}


bool Pedestrian::isPed()
{
    return true;
}




//==============================================================================




ZombiePed::ZombiePed(int startX, int startY, StudentWorld* world)
: Pedestrian(IID_ZOMBIE_PED, startX, startY, 3.0, world)
{
    m_ticks = 0;
}


void ZombiePed::doSomething()
{
    if (! isAlive())
    {
        return;
    }
    
    //if zombie overlaps with ghost racer
    GhostRacer* player = getWorld()->getPlayer();
    if (objectOverlap(player))
    {
        player->setHealth(player->getHealth()-5);
        setHealth(getHealth()-2);
        getWorld()->playSound(SOUND_PED_HURT);
    }
    
    //if zombie is dead
    if (! isAlive())
    {
        setHealth(0);
        getWorld()->playSound(SOUND_PED_DIE);
    }
    
    //if zombie is ahead of ghost racer
    double yDist = getY() - getWorld()->getPlayer()->getY();
    if (yDist > 0)
    {
        //if x distance is within 30 pixels
        double xDist = getX() - getWorld()->getPlayer()->getX();
        if (abs(xDist) <= 30)
        {
            setDirection(270);
            
            //if zombie is left of ghost racer
            if (xDist < 0)
            {
                setHorizSpeed(1);
            }
            //if zombie is right of ghost racer
            if (xDist > 0)
            {
                setHorizSpeed(-1);
            }
            //if zombie and ghost racer have same x coordinate
            else
            {
                setHorizSpeed(0);
            }
            
            //update number of ticks until zombie grunts next
            m_ticks--;
            if (m_ticks <= 0)
            {
                getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
                m_ticks = 20;
            }
        }
    }
        
    //move zombie
    moveActor();
        
    //update movement plan
    int dist = getMovementPlanDistance();
    if (dist > 0)
    {
        setMovementPlanDistance(dist-1);
        return;
    }
    setNewMovementPlan();
}

bool ZombiePed::isDestroyable()
{
    return true;
}




//==============================================================================




HumanPed::HumanPed(int startX, int startY, StudentWorld* world)
: Pedestrian(IID_HUMAN_PED, startX, startY, 2.0, world)
{
    setMovementPlanDistance(0);
}


void HumanPed::doSomething()
{
    if (! isAlive())
    {
        return;
    }
    
    //if human pedestrian overlaps with ghost racer
    if (objectOverlap(getWorld()->getPlayer()))
    {
        getWorld()->getPlayer()->setHealth(0);
        return;
    }
    
    //move human
    moveActor();
    
    //decrement movement plan distance
    int dist = getMovementPlanDistance();
    setMovementPlanDistance(dist-1);
    
    //update movement plan
    setNewMovementPlan();
}




//==============================================================================




ZombieCab::ZombieCab(int startX, int startY, double vert_speed, StudentWorld* world)
: Actor(IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0, world, true)
{
    setVertSpeed(vert_speed);
    setHorizSpeed(0);
    setHealth(3);
    m_planLength = 0;
    m_damagedPlayer = false;
}

void ZombieCab::doSomething()
{
    if (! isAlive())
    {
        return;
    }
    
    //if cab overlaps with ghost racer
    if (objectOverlap(getWorld()->getPlayer()))
    {
        //if cab hasn't damaged ghost racer
        if (m_damagedPlayer == false)
        {
            //damage ghost racer
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            GhostRacer* player = getWorld()->getPlayer();
            player->setHealth(player->getHealth()-20);
            if (getX() <= player->getX())
            {
                setHorizSpeed(-5);
                setDirection(120+randInt(0, 19));
            }
            else
            {
                setHorizSpeed(5);
                setDirection(60-randInt(0, 19));
            }
            m_damagedPlayer = true;
        }
    }
    
    //move cab
    moveActor();
    
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    int xLeft = LEFT_EDGE + ROAD_WIDTH/3;
    int xRight = RIGHT_EDGE - ROAD_WIDTH/3;
    
    //if cab is in the left lane
    if (getX() >= LEFT_EDGE && getX() < xLeft)
    {
        getWorld()->checkForActorAhead(this, LEFT_EDGE, xLeft);
        getWorld()->checkForActorBehind(this, LEFT_EDGE, xLeft);
    }
    //if cab is in the middle lane
    if (getX() >= xLeft && getX() < xRight)
    {
        getWorld()->checkForActorAhead(this, xLeft, xRight);
        getWorld()->checkForActorBehind(this, xLeft, xRight);
    }
    //if cab is in the right lane
    if (getX() >= xRight && getX() < RIGHT_EDGE)
    {
        getWorld()->checkForActorAhead(this, xRight, RIGHT_EDGE);
        getWorld()->checkForActorBehind(this, xRight, RIGHT_EDGE);
    }
    
    //update movement plan
    m_planLength--;
    if (m_planLength > 0)
    {
        return;
    }
    m_planLength = randInt(4, 32);
    setVertSpeed(getVertSpeed() + randInt(-2, 2));
}


bool ZombieCab::isDestroyable()
{
    return true;
}




//==============================================================================




OilSlick::OilSlick(int startX, int startY, double size, StudentWorld* world)
: Actor(IID_OIL_SLICK, startX, startY, 0, size, 2, world, false)
{
    setVertSpeed(-4);
    setHorizSpeed(0);
    setHealth(100);
}


void OilSlick::doSomething()
{
    
    //move oil slick
    moveActor();
    
    //if oil slick overlaps with ghost racer
    if (objectOverlap(getWorld()->getPlayer()))
    {
        getWorld()->playSound(SOUND_OIL_SLICK);
        int randAngle = randInt(5, 20);
        int randDirection = randInt(0, 1);
        GhostRacer* player = getWorld()->getPlayer();
        //set ghost racer to random direction between 60 and 120
        if (randDirection == 0)
        {
            if (player->getDirection()-randAngle >= 60)
            {
                player->setDirection(player->getDirection()-randAngle);
            }
            else
            {
                player->setDirection(player->getDirection()+randAngle);
            }
        }
        else {
            if (player->getDirection()+randAngle <= 120)
            {
                player->setDirection(player->getDirection()+randAngle);
            }
            else
            {
                player->setDirection(player->getDirection()-randAngle);
            }
        }
    }
}




//==============================================================================




HolyWaterProjectile::HolyWaterProjectile(int startX, int startY, int dir, StudentWorld* world)
: Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1, world, false)
{
    setHealth(100);
    m_startX = startX;
    m_startY = startY;
    m_maxDistance = 160;
}


void HolyWaterProjectile::doSomething()
{
    if (! isAlive())
    {
        return;
    }
    
    //damage object if they overlap
    getWorld()->damageObject(this);
    
    //move forward
    moveForward(SPRITE_HEIGHT);
    
    //if projectile goes off road
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT)
    {
        setHealth(0);
        return;
    }
    
    //if projectile traveled 160 pixels
    double delta_x = getX() - m_startX;
    double delta_y = getY() - m_startY;
    if (delta_x * delta_x + delta_y * delta_y >= m_maxDistance * m_maxDistance)
    {
        setHealth(0);
    }
}




//==============================================================================




LostSoul::LostSoul(int startX, int startY, StudentWorld* world)
: Actor(IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2, world, false)
{
    setVertSpeed(-4);
    setHorizSpeed(0);
    setHealth(100);
}


void LostSoul::doSomething()
{
    //move soul
    moveActor();
    
    //if soul overlaps with ghost racer;
    if (objectOverlap(getWorld()->getPlayer()))
    {
        int souls = getWorld()->getSouls();
        getWorld()->setSouls(souls + 1);
        setHealth(0);
        getWorld()->playSound(SOUND_GOT_SOUL);
        getWorld()->increaseScore(100);
    }
    //rotate soul
    setDirection(-10);
}




//==============================================================================



Goodies::Goodies(int imageID, int startX, int startY, int dir, double size, int depth, StudentWorld* world)
: Actor(imageID, startX, startY, dir, size, depth, world, false)
{
    setVertSpeed(-4);
    setHorizSpeed(0);
    setHealth(1);
}


void Goodies::doSomething()
{
    moveActor();
}


bool Goodies::isDestroyable()
{
    return true;
}




//==============================================================================




Healing::Healing(int startX, int startY, StudentWorld* world)
: Goodies(IID_HEAL_GOODIE, startX, startY, 0, 1.0, 2, world)
{
    
}


void Healing::doSomething()
{
    //move Healing goodie
    Goodies::doSomething();
    
    //if goodie overlaps with ghost racer
    if (objectOverlap(getWorld()->getPlayer()))
    {
        int ghostRacerHealth = getWorld()->getPlayer()->getHealth();
        getWorld()->getPlayer()->setHealth(ghostRacerHealth + 10);
        setHealth(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(250);
    }
}




//==============================================================================




HolyWaterRefill::HolyWaterRefill(int startX, int startY, StudentWorld* world)
: Goodies(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0, 2, world)
{
    
}


void HolyWaterRefill::doSomething()
{
    //move Refill goodie
    Goodies::doSomething();
    
    //if goodie overlaps with ghost racer
    if (objectOverlap(getWorld()->getPlayer()))
    {
        GhostRacer* player = getWorld()->getPlayer();
        player->setSprays(10);
        setHealth(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
    }
}
