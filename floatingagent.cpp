#include "agent.h"
#include <QDebug>

FloatingAgent::FloatingAgent(const QString& name,
                             int hp,
                             int mobility,
                             int damage,
                             int attackRange,
                             const QPoint& position,
                             QObject* parent)
    : Agent(name, Floating, hp, mobility, damage, attackRange, position, parent)
{
    qDebug() << "FloatingAgent created:" << name;
}

bool FloatingAgent::canMoveTo(const QString& cellType) const
{
    // می‌تواند روی همه نوع سلول‌ها حرکت کند
    Q_UNUSED(cellType);
    qDebug() << "FloatingAgent" << m_name << "can move to any cell";
    return true;
}

bool FloatingAgent::canStandOn(const QString& cellType) const
{
    // می‌تواند روی همه نوع سلول‌ها بایستد
    Q_UNUSED(cellType);
    qDebug() << "FloatingAgent" << m_name << "can stand on any cell";
    return true;
}
