#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QPoint>
#include <QString>
#include <QDebug>
#include <QVector>
#include <QMap>

class Agent : public QObject {
    Q_OBJECT
public:
    enum Type {
        WaterWalking,  // حرکت روی آب و زمین
        Grounded,      // فقط روی زمین
        Flying,        // پرواز روی همه جا ولی ایستادن فقط روی زمین
        Floating       // حرکت و ایستادن روی همه جا
    };

    // Constructor
    explicit Agent(const QString& name,
                   Type type,
                   int hp,
                   int mobility,
                   int damage,
                   int attackRange,
                   const QPoint& position = QPoint(0, 0),
                   QObject* parent = nullptr);

    // Virtual destructor
    virtual ~Agent() = default;

    // Pure virtual functions
    virtual bool canMoveTo(const QString& cellType) const = 0;
    virtual bool canStandOn(const QString& cellType) const = 0;

    // Static methods
    static QVector<Type> getAvailableAgentTypes();
    static QMap<Type, QVector<QString>> getAgentsByType();
    static Agent* createAgent(const QString& name, const QPoint& position = QPoint(0, 0), QObject* parent = nullptr);

    // Getters
    const QString& name() const { return m_name; }
    Type type() const { return m_type; }
    int hp() const { return m_hp; }
    int mobility() const { return m_mobility; }
    int damage() const { return m_damage; }
    int attackRange() const { return m_attackRange; }
    const QPoint& position() const { return m_position; }

    // Setters
    void setPosition(const QPoint& pos) { m_position = pos; }
    void takeDamage(int amount) {
        m_hp = qMax(0, m_hp - amount);
        if (m_hp <= 0) emit agentDestroyed(this);
    }

    // Signals
signals:
    void agentDestroyed(Agent* agent);
    void positionChanged(const QPoint& newPosition);

protected:
    QString m_name;
    Type m_type;
    int m_hp;
    int m_mobility;
    int m_damage;
    int m_attackRange;
    QPoint m_position;
};

// WaterWalking Agent
class WaterWalkingAgent : public Agent {
public:
    explicit WaterWalkingAgent(const QString& name,
                               int hp,
                               int mobility,
                               int damage,
                               int attackRange,
                               const QPoint& position = QPoint(0, 0),
                               QObject* parent = nullptr);

    bool canMoveTo(const QString& cellType) const override;
    bool canStandOn(const QString& cellType) const override;
};

// Grounded Agent
class GroundedAgent : public Agent {
public:
    explicit GroundedAgent(const QString& name,
                           int hp,
                           int mobility,
                           int damage,
                           int attackRange,
                           const QPoint& position = QPoint(0, 0),
                           QObject* parent = nullptr);

    bool canMoveTo(const QString& cellType) const override;
    bool canStandOn(const QString& cellType) const override;
};

// Flying Agent
class FlyingAgent : public Agent {
public:
    explicit FlyingAgent(const QString& name,
                         int hp,
                         int mobility,
                         int damage,
                         int attackRange,
                         const QPoint& position = QPoint(0, 0),
                         QObject* parent = nullptr);

    bool canMoveTo(const QString& cellType) const override;
    bool canStandOn(const QString& cellType) const override;
};

// Floating Agent
class FloatingAgent : public Agent {
public:
    explicit FloatingAgent(const QString& name,
                           int hp,
                           int mobility,
                           int damage,
                           int attackRange,
                           const QPoint& position = QPoint(0, 0),
                           QObject* parent = nullptr);

    bool canMoveTo(const QString& cellType) const override;
    bool canStandOn(const QString& cellType) const override;
};

#endif // AGENT_H
