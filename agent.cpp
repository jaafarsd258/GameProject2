#include "agent.h"
#include <QMap>

namespace {
struct AgentStats {
    int hp;
    int mobility;
    int damage;
    int attackRange;
};

const QMap<QString, AgentStats> AGENT_STATS = {
    // WaterWalking Agents
    {"Billy",         {320, 3, 90,  1}},
    {"Reketon",       {320, 2, 80,  2}},
    {"Angus",         {400, 2, 100, 1}},
    {"Duraham",       {320, 2, 100, 2}},
    {"Colonel Baba",  {400, 2, 100, 1}},
    {"Medusa",        {320, 2, 90,  2}},
    {"Bunka",         {320, 3, 100, 1}},
    {"Sanka",         {320, 3, 100, 1}},

    // Grounded Agents
    {"Sir Lamorak",   {320, 3, 110, 1}},
    {"Kabu",          {400, 2, 120, 1}},
    {"Rajakal",       {320, 2, 130, 1}},
    {"Salih",         {400, 2, 80,  1}},
    {"Khan",          {320, 2, 90,  1}},
    {"Boi",           {400, 2, 100, 1}},
    {"Eloi",          {240, 2, 100, 2}},
    {"Kanar",         {160, 2, 100, 2}},
    {"Elsa",          {320, 2, 140, 2}},
    {"Karissa",       {280, 2, 80,  2}},
    {"Sir Philip",    {400, 2, 100, 1}},
    {"Frost",         {260, 2, 80,  2}},
    {"Tusk",          {400, 2, 100, 1}},

    // Flying Agents
    {"Rambu",         {320, 3, 120, 1}},

    // Floating Agents
    {"Sabrina",       {320, 3, 100, 1}},
    {"Death",         {240, 3, 120, 2}}
};
}

QVector<Agent::Type> Agent::getAvailableAgentTypes() {
    return {WaterWalking, Grounded, Flying, Floating};
}

QMap<Agent::Type, QVector<QString>> Agent::getAgentsByType() {
    return {
        {WaterWalking, {"Billy", "Reketon", "Angus", "Duraham",
                        "Colonel Baba", "Medusa", "Bunka", "Sanka"}},
        {Grounded,     {"Sir Lamorak", "Kabu", "Rajakal", "Salih",
                    "Khan", "Boi", "Eloi", "Kanar",
                    "Elsa", "Karissa", "Sir Philip", "Frost", "Tusk"}},
        {Flying,       {"Rambu"}},
        {Floating,     {"Sabrina", "Death"}}
    };
}

Agent* Agent::createAgent(const QString& name, const QPoint& position, QObject* parent) {
    if (!AGENT_STATS.contains(name)) {
        qWarning() << "Unknown agent name:" << name;
        return nullptr;
    }

    const AgentStats& stats = AGENT_STATS[name];

    if (name == "Rambu") {
        return new FlyingAgent(name, stats.hp, stats.mobility,
                               stats.damage, stats.attackRange, position, parent);
    }
    else if (name == "Sabrina" || name == "Death") {
        return new FloatingAgent(name, stats.hp, stats.mobility,
                                 stats.damage, stats.attackRange, position, parent);
    }
    else if (getAgentsByType()[WaterWalking].contains(name)) {
        return new WaterWalkingAgent(name, stats.hp, stats.mobility,
                                     stats.damage, stats.attackRange, position, parent);
    }
    else {
        return new GroundedAgent(name, stats.hp, stats.mobility,
                                 stats.damage, stats.attackRange, position, parent);
    }
}

Agent::Agent(const QString& name, Type type, int hp, int mobility,
             int damage, int attackRange, const QPoint& position, QObject* parent)
    : QObject(parent),
    m_name(name),
    m_type(type),
    m_hp(hp),
    m_mobility(mobility),
    m_damage(damage),
    m_attackRange(attackRange),
    m_position(position)
{
    qDebug() << "Created agent:" << name
             << "| Type:" << type
             << "| HP:" << hp
             << "| Position:" << position;
}
