#include "agent.h"
#include <QDebug>

WaterWalkingAgent::WaterWalkingAgent(const QString& name,
                                     int hp,
                                     int mobility,
                                     int damage,
                                     int attackRange,
                                     const QPoint& position,
                                     QObject* parent)
    : Agent(name, WaterWalking, hp, mobility, damage, attackRange, position, parent)
{
    qDebug() << "WaterWalkingAgent created:" << name;
}

bool WaterWalkingAgent::canMoveTo(const QString& cellType) const
{
    // می‌تواند روی آب (~) یا زمین خالی ("") حرکت کند
    bool canMove = (cellType == "~" || cellType == "");
    qDebug() << "WaterWalkingAgent" << m_name << "canMoveTo" << cellType << ":" << canMove;
    return canMove;
}

bool WaterWalkingAgent::canStandOn(const QString& cellType) const
{
    // می‌تواند روی آب (~) یا زمین خالی ("") بایستد
    bool canStand = (cellType == "~" || cellType == "");
    qDebug() << "WaterWalkingAgent" << m_name << "canStandOn" << cellType << ":" << canStand;
    return canStand;
}
