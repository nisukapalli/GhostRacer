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


int StudentWorld::init()
{
    m_player = new GhostRacer(this);
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
    m_player->doSomething();
    for (int i = 0; i != m_actors.size(); i++)
    {
        if (m_actors[i]->isAlive())
        {
            m_actors[i]->doSomething();
        }
        if (! m_player->isAlive())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    
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
    
    
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    int xLeft = LEFT_EDGE + ROAD_WIDTH/3;
    int xRight = RIGHT_EDGE - ROAD_WIDTH/3;
    int j = m_actors.size()-1;
    while(j >= 0)
    {
        if (! m_actors[j]->isAvoidanceWorthy())
        {
            if (m_actors[j]->getX() == LEFT_EDGE || m_actors[j]->getX() == RIGHT_EDGE
                || m_actors[j]->getX() == xLeft || m_actors[j]->getX() == xRight)
            {
                break;
            }
        }
        else
        {
            j--;
        }
    }
    m_lastWhiteBorderY += m_actors[j]->getVertSpeed();
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
    
    
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << 2*getLevel()+5 << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_player->getHealth() << "  ";
    oss << "Sprays: " << m_player->getSprays() << "  ";
    oss << "Bonus: " << m_bonus--;
    setGameStatText(oss.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;
}
