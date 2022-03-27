#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_player = nullptr;
    m_souls = 0;
    m_bonus = 5000;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

GhostRacer* StudentWorld::getPlayer()
{
    return m_player;
}

void StudentWorld::pushActor(Actor* a)
{
    m_actors.push_back(a);
}

void StudentWorld::zombieCabInit()
{
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    int xLeft = LEFT_EDGE + ROAD_WIDTH/3;
    int xRight = RIGHT_EDGE - ROAD_WIDTH/3;
    int left;
    int right;
    double startX;
    double startY;
    double vert_speed;
    
    
    //choose a random lane
    vector<int> randLanes;
    int cur_lane = randInt(0, 2);
    randLanes.push_back(cur_lane);
    if (cur_lane == 0)
    {
        randLanes.push_back(1);
        randLanes.push_back(2);
    }
    if (cur_lane == 1)
    {
        randLanes.push_back(0);
        randLanes.push_back(2);
    }
    else
    {
        randLanes.push_back(0);
        randLanes.push_back(1);
    }
    
    for (int i = 0; i != randLanes.size(); i++)
    {
        //left lane
        if (randLanes[i] == 0)
        {
            left = LEFT_EDGE;
            right = xLeft;
        }
        //middle lane
        if (randLanes[i] == 1)
        {
            left = xLeft;
            right = xRight;
        }
        //right lane
        else
        {
            left = xRight;
            right = RIGHT_EDGE;
        }
    
        //find actor closest to bottom in the lane
        Actor* closestToBottom = nullptr;
        double smallestY = VIEW_HEIGHT;
        for (int i = 0; i != m_actors.size(); i++)
        {
            Actor* act = m_actors[i];
            //if actor is in same lane
            if (act->getX() >= left && act->getX() < right)
            {
                //if actor is avoidance worthy and closer to bottom
                if (act->isAvoidanceWorthy() && act->getY() < smallestY)
                {
                    //set actor as closest to bottom
                    closestToBottom = act;
                    smallestY = act->getY();
                }
            }
        }
        
        if (closestToBottom == nullptr || closestToBottom->getY() > VIEW_HEIGHT/3)
        {
            //if ghost racer isn't in the lane
            if (! (getPlayer()->getX() >= left && getPlayer()->getX() < right))
            {
                //add zombie cab
                startX = (left+right)/2;
                startY = SPRITE_HEIGHT/2;
                vert_speed = getPlayer()->getVertSpeed() + randInt(2, 4);
                m_actors.push_back(new ZombieCab(startX, startY, vert_speed, this));
                break;
            }
        }
        
        //find actor closest to top in the lane
        Actor* closestToTop = nullptr;
        double largestY = 0;
        for (int i = 0; i != m_actors.size(); i++)
        {
            Actor* act = m_actors[i];
            //if actor is in same lane
            if (act->getX() >= left && act->getX() < right)
            {
                //if actor is avoidance worthy and closer to top
                if (act->isAvoidanceWorthy() && act->getY() > largestY)
                {
                    //set actor as closest to top
                    closestToTop = act;
                    largestY = act->getY();
                }
            }
        }
        
        if (closestToTop == nullptr || closestToTop->getY() < VIEW_HEIGHT*2/3)
        {
            //if ghost racer isn't in the lane
            if (! (getPlayer()->getX() >= left && getPlayer()->getX() < right))
            {
                //add zombie cab
                startX = (left+right)/2;
                startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                vert_speed = getPlayer()->getVertSpeed() - randInt(2, 4);
                m_actors.push_back(new ZombieCab(startX, startY, vert_speed, this));
                break;
            }
        }
    }
}

void StudentWorld::checkForActorAhead(ZombieCab* cab, int left, int right)
{
    //if cab is faster than ghost racer
    if (cab->getVertSpeed() > getPlayer()->getVertSpeed())
    {
        //traverse actors vector
        for (int i = 0; i != m_actors.size(); i++)
        {
            Actor* act = m_actors[i];
            
            //if actor is avoidance worthy and not the cab
            if (act != cab && act->isAvoidanceWorthy())
            {
                //if actor is in the same lane as cab
                if (act->getX() >= left && act->getX() < right)
                {
                    //if actor is less than 96 pixels ahead of cab
                    double dist = act->getY() - cab->getY();
                    if (dist > 0 && dist < 96)
                    {
                        //decrease cab's vertical speed
                        double curSpeed = cab->getVertSpeed();
                        cab->setVertSpeed(curSpeed-0.5);
                        return;
                    }
                }
            }
        }
    }
}

void StudentWorld::checkForActorBehind(ZombieCab* cab, int left, int right)
{
    if (cab->getVertSpeed() <= getPlayer()->getVertSpeed())
    {
        //traverse actors vector
        for (int i = 0; i != m_actors.size(); i++)
        {
            Actor* act = m_actors[i];
            
            //if actor is avoidance worthy and not the cab
            if (act != cab && act->isAvoidanceWorthy())
            {
                //if actor is in the same lane as cab
                if (act->getX() >= left && act->getX() < right)
                {
                    //if actor is less than 96 pixels behind cab
                    double dist = m_actors[i]->getY() - cab->getY();
                    if (dist < 0 && dist > -96)
                    {
                        double curSpeed = cab->getVertSpeed();
                        cab->setVertSpeed(curSpeed+0.5);
                        return;
                    }
                }
            }
        }
    }
}

void StudentWorld::damageObject(HolyWaterProjectile* holyWater)
{
    for (int i = 0; i != m_actors.size(); i++)
    {
        Actor* act = m_actors[i];
        
        //if projectile hits a destroyable object
        if (holyWater->objectOverlap(act) && act->isDestroyable())
        {
            //damage object
            act->setHealth(act->getHealth()-1);
            if (act->isAvoidanceWorthy())
            {
                //if actor is a zombie
                if (act->isPed())
                {
                    if (act->isAlive())
                    {
                        playSound(SOUND_PED_HURT);
                    }
                    else
                    {
                        playSound(SOUND_PED_DIE);
                        
                        //add new healing goodie
                        int randHealing = randInt(1, 5);
                        if (randHealing == 1)
                        {
                            pushActor(new Healing(act->getX(), act->getY(), this));
                        }
                        increaseScore(150);
                    }
                }
                
                //if actor is a cab
                else
                {
                    if (act->isAlive())
                    {
                        playSound(SOUND_VEHICLE_HURT);
                    }
                    else
                    {
                        playSound(SOUND_VEHICLE_DIE);
                        
                        //add new oil slick
                        int randOilSlick = randInt(1, 5);
                        if (randOilSlick == 1)
                        {
                            int randSize = randInt(2, 5);
                            pushActor(new OilSlick(act->getX(), act->getY(), randSize, this));
                        }
                        increaseScore(200);
                    }
                }
            }
            //set projectile to not alive
            holyWater->setHealth(0);
            return;
        }
    }
}

int StudentWorld::getSouls()
{
    return m_souls;
}

void StudentWorld::setSouls(int souls)
{
    m_souls = souls;
}


int StudentWorld::init()
{
    //add ghost racer
    m_player = new GhostRacer(this);
    
    //add yellow border lines
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    for (int i = 0; i < N; i++) {
        int xLeft = LEFT_EDGE;
        int xRight = RIGHT_EDGE;
        int y = i * SPRITE_HEIGHT;
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, xLeft, y, this));
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, xRight, y, this));
    }
    
    //add white border lines
    for (int i = 0; i < M; i++) {
        int xLeft = LEFT_EDGE + ROAD_WIDTH/3;
        int xRight = RIGHT_EDGE - ROAD_WIDTH/3;
        int y = i * 4 * SPRITE_HEIGHT;
        m_lastWhiteBorderY = y;
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, xLeft, y, this));
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, xRight, y, this));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //tell ghost racer and actors to do something
    m_player->doSomething();
    for (int i = 0; i != m_actors.size(); i++)
    {
        //check if actor is alive
        if (m_actors[i]->isAlive())
        {
            m_actors[i]->doSomething();
        }
        
        //check if ghost racer is alive
        if (! m_player->isAlive())
        {
            decLives();
            playSound(SOUND_PLAYER_DIE);
            
            //reset bonus
            m_bonus = 5000;
            return GWSTATUS_PLAYER_DIED;
        }
        
        //if required number of souls are saved
        if (2 * getLevel() + 5 - getSouls() <= 0)
        {
            m_souls = 0;
            playSound(SOUND_FINISHED_LEVEL);
            
            //add bonus to score
            increaseScore(m_bonus);
            
            //reset bonus
            m_bonus = 5000;
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    //delete dead actors
    int i = 0;
    while(i != m_actors.size())
    {
        if (! m_actors[i]->isAlive())
        {
            delete m_actors[i];
            m_actors.erase(m_actors.begin()+i);
        }
        else
        {
            i++;
        }
    }
    
    //find a border line's speed
    int j = 0;
    for (; j != m_actors.size(); j++)
    {
        if (m_actors[j]->canAddNewBorders())
        {
            break;
        }
    }
    m_lastWhiteBorderY += m_actors[j]->getVertSpeed();
    
    //add new border lines
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - m_lastWhiteBorderY;
    if (delta_y >= SPRITE_HEIGHT)
    {
        int xLeft = ROAD_CENTER - ROAD_WIDTH/2;
        int xRight = ROAD_CENTER + ROAD_WIDTH/2;
        int y = new_border_y;
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, xLeft, y, this));
        m_actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, xRight, y, this));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        int xLeft = ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3;
        int xRight = ROAD_CENTER + ROAD_WIDTH/2 - ROAD_WIDTH/3;
        int y = new_border_y;
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, xLeft, y, this));
        m_actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, xRight, y, this));
        m_lastWhiteBorderY = y;
    }
    
    //add new zombie cab
    int chanceVehicle = max(100-getLevel()*10, 20);
    int randVehicle = randInt(0, chanceVehicle-1);
    
    //zombie movement plan
    if (randVehicle == 0)
    {
        //determine starting location and speed for new cab
        zombieCabInit();
    }
    
    //add new oil slick
    int chanceOilSlick = max(150-getLevel()*10, 40);
    int randOilSlick = randInt(0, chanceOilSlick-1);
    if (randOilSlick == 0)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
        int randX = randInt(LEFT_EDGE, RIGHT_EDGE);
        int randSize = randInt(2, 5);
        m_actors.push_back(new OilSlick(randX, VIEW_HEIGHT, randSize, this));
    }
    
    //add new zombie pedestrian
    int chanceZombiePed = max(100-getLevel()*10, 20);
    int randZombiePed = randInt(0, chanceZombiePed-1);
    if (randZombiePed == 0)
    {
        int randX = randInt(0, VIEW_WIDTH);
        m_actors.push_back(new ZombiePed(randX, VIEW_HEIGHT, this));
    }
    
    //add new human pedestrian
    int chanceHumanPed = max(200-getLevel()*10, 30);
    int randHumanPed = randInt(0, chanceHumanPed-1);
    if (randHumanPed == 0)
    {
        int randX = randInt(0, VIEW_WIDTH);
        m_actors.push_back(new HumanPed(randX, VIEW_HEIGHT, this));
    }
    
    //add new holy water refill goodie
    int chanceOfHolyWater = 100+getLevel()*10;
    int randHolyWater = randInt(0, chanceOfHolyWater-1);
    if (randHolyWater == 0)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
        int randX = randInt(LEFT_EDGE, RIGHT_EDGE);
        m_actors.push_back(new HolyWaterRefill(randX, VIEW_HEIGHT, this));
    }
    
    //add new lost soul goodie
    int chanceOfLostSoul = 100;
    int randLostSoul = randInt(0, chanceOfLostSoul-1);
    if (randLostSoul == 0)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
        int randX = randInt(LEFT_EDGE, RIGHT_EDGE);
        m_actors.push_back(new LostSoul(randX, VIEW_HEIGHT, this));
    }
    
    //decrement bonus for each tick
    if (m_bonus > 0)
    {
        m_bonus--;
    }
    
    //update status text
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << 2 * getLevel() + 5 - getSouls() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_player->getHealth() << "  ";
    oss << "Sprays: " << m_player->getSprays() << "  ";
    oss << "Bonus: " << m_bonus;
    setGameStatText(oss.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //iterate through vector
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    
    //delete actors
    while (it != m_actors.end())
    {
        delete *it;
        it = m_actors.erase(it);
    }
    
    //delete ghost racer
    if (m_player != nullptr)
    {
        delete m_player;
    }
    m_player = nullptr;
}
