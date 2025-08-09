#include "agent.h"
#include <QDebug>

FlyingAgent::FlyingAgent(const QString& name,
                         int hp,
                         int mobility,
                         int damage,
                         int attackRange,
                         const QPoint& position,
                         QObject* parent)
    : Agent(name, Flying, hp, mobility, damage, attackRange, position, parent)
{
    qDebug() << "FlyingAgent created:" << name;
}

bool FlyingAgent::canMoveTo(const QString& cellType) const
{
    // می‌تواند روی همه نوع سلول‌ها حرکت کند
    Q_UNUSED(cellType);
    qDebug() << "FlyingAgent" << m_name << "can move to any cell";
    return true;
}

bool FlyingAgent::canStandOn(const QString& cellType) const
{
    // فقط روی زمین خالی ("") می‌تواند بایستد
    bool canStand = (cellType == "");
    qDebug() << "FlyingAgent" << m_name << "canStandOn" << cellType << ":" << canStand;
    return canStand;
}
