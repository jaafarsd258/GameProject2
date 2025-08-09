#include "agent.h"
#include <QDebug>

GroundedAgent::GroundedAgent(const QString& name,
                             int hp,
                             int mobility,
                             int damage,
                             int attackRange,
                             const QPoint& position,
                             QObject* parent)
    : Agent(name, Grounded, hp, mobility, damage, attackRange, position, parent)
{
    qDebug() << "GroundedAgent created:" << name;
}

bool GroundedAgent::canMoveTo(const QString& cellType) const
{
    // فقط روی زمین خالی ("") می‌تواند حرکت کند
    bool canMove = (cellType == "");
    qDebug() << "GroundedAgent" << m_name << "canMoveTo" << cellType << ":" << canMove;
    return canMove;
}

bool GroundedAgent::canStandOn(const QString& cellType) const
{
    // فقط روی زمین خالی ("") می‌تواند بایستد
    bool canStand = (cellType == "");
    qDebug() << "GroundedAgent" << m_name << "canStandOn" << cellType << ":" << canStand;
    return canStand;
}
