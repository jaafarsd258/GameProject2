#ifndef SELECTAGENTS_H
#define SELECTAGENTS_H

#include <QMainWindow>
#include <QMap>
#include <QSet>
#include <QVector>
#include "agent.h"
#include "gameboard.h"

class QPushButton;
class QLabel;
class QVBoxLayout;
class QGridLayout;
class QFrame;

struct AgentInfo {
    QString name;
    QString imagePath;
    QString type;
    int hp;
    int mobility;
    int damage;
    int attackRange;
};

class ClickableFrame : public QFrame {
    Q_OBJECT
public:
    explicit ClickableFrame(QWidget* parent = nullptr) : QFrame(parent) {}
signals:
    void clicked();  // سیگنال کلیک
protected:
    void mousePressEvent(QMouseEvent* event) override {
        QFrame::mousePressEvent(event);
        emit clicked();  // ارسال سیگنال هنگام کلیک
    }
};


class SelectAgents : public QMainWindow {
    Q_OBJECT
public:
    SelectAgents(const QString& player1Name,
                 const QString& player2Name,
                 QWidget* parent = nullptr);
    QString getPlayer1Name() const { return m_player1Name; }
    QString getPlayer2Name() const { return m_player2Name; }
    QStringList getPlayer1Agents() const; // اضافه شود
    QStringList getPlayer2Agents() const; // اضافه شود
signals:
    void gameReady();

private slots:
    void onAgentSelected(int player, const QString& agentName, Agent::Type type);
    void onPlayer1ConfirmClicked();
    void onPlayer2ConfirmClicked();
    void onStartGameClicked();

private:
    void setupUI();
    void createAgentSelectionUI();
    void updateSelectedAgentsDisplay();
    void updateStatus();
    void setupAgentsData();
    QString m_player1Name;
    QString m_player2Name;
    bool m_player1Confirmed = false;
    bool m_player2Confirmed = false;


    QMap<int, QSet<QString>> m_selectedAgents; // 1 for player1, 2 for player2
    QMap<Agent::Type, QVector<QString>> m_agentsByType;

    QLabel* m_player1Label;
    QLabel* m_player2Label;
    QLabel* m_selected1Label;
    QLabel* m_selected2Label;
    QLabel* m_statusLabel;
    QPushButton* m_player1ConfirmButton;
    QPushButton* m_player2ConfirmButton;
    QPushButton* m_startButton;
    QLabel* backgroundLabel;
    QVector<AgentInfo> m_agents;
    gameboard* m_gameBoard = nullptr; // Add this line

public:
    ~SelectAgents() {
        if (m_gameBoard) {
            delete m_gameBoard;
        }
    }

};

#endif // SELECTAGENTS_H
