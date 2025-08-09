#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QBoxLayout>
#include <QDrag>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QMimeData>
#include <QPainter>
#include <QVector>
#include <QRandomGenerator>
#include "agent.h"

class QGridLayout;
class QScrollArea;
class QPushButton;

struct DraggableItem
{
    QString name;
    QString imagePath;
    int power;
    int range;
    int health;
    QString type; // WaterWalking, Grounded, Flying, Floating
    int mobility;
    int defense;
};
Q_DECLARE_METATYPE(DraggableItem)
struct GameCell
{
    int col;
    int row;
    QString type; // "1", "2", "#", "~", ""
    QString agentName; 

};
class gameboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit gameboard(const QString &gridFile,
                       const QString &gridStyle,
                       const QString &player1Name,
                       const QString &player2Name,
                       QWidget *parent = nullptr);
    virtual ~gameboard() = default;
    static const QStringList& getGridFiles();
    void setPlayer1Agents(const QStringList& agents);
    void setPlayer2Agents(const QStringList& agents);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void initializeBoard();
    void loadGridData();
    void displayHexagonalGrid();
    void drawBoard(QPainter &painter);
    void drawHexagon(QPainter &painter, const QPoint &center, int size, const QBrush &brush);
    QPoint hexagonCenter(int col, int row) const;
    QPixmap createHexagonPixmap(const QString &content, const QColor &color);
    void setupPlayerPanels();
    void createDraggableItems();
    QPoint convertToGridCoordinates(const QPoint &screenPos);
    void addItemToGrid(const QString &itemName, const QPoint &gridPos);
    void highlightMovesForAgent(const QPoint &startPos, const QString &agentType, int mobility);
    void highlightAvailableMoves(const QPoint &gridPos, int mobility);
    void clearHighlights();
    void placeAgentsOnBoard(int player, const QStringList& agents);
    void removeNumbersAfterPlacement();
    QWidget* createAgentCard(const QString& agentName, bool isPlayer1);
    QWidget *m_gridWidget; // Add this

    QWidget* createDraggableItemWidget(const DraggableItem &item, bool isPlayer1);
    void createDraggableItemsFromAgents(const QStringList& agents, bool isPlayer1);
    void updatePlayerPanels();
    QVector<DraggableItem> m_player1DraggableItems;
    QVector<DraggableItem> m_player2DraggableItems;
    QMap<QString, QPixmap> m_player1ItemPixmaps;
    QMap<QString, QPixmap> m_player2ItemPixmaps;
    QWidget* leftSidePanel;
    QWidget* rightSidePanel;
    QVector<DraggableItem> m_draggableItems;
    QMap<QString, QPixmap> m_itemPixmaps;

    // Hexagon geometry constants
    static constexpr int HEX_SIZE = 30;
    static constexpr double HEX_WIDTH = HEX_SIZE * 2;
    static constexpr double HEX_HEIGHT = HEX_SIZE * 1.73205; // sqrt(3)
    QString m_player1Name;
    QString m_player2Name;
    QString m_gridFile;
    QString m_gridStyle;
    QList<Agent*> m_agents;
    QStringList m_player1Agents;
    QStringList m_player2Agents;

    // Grid data structures
    QVector<QVector<GameCell>> m_board;

    // UI elements
    QLabel *backgroundLabel;
    QVector<QLabel*> m_agentLabels;
    // Player info displays
    QLabel *p1Label;
    QLabel *p2Label;
    QLabel *currentPlayerLabel;
    QLabel *turnLabel;

    // Game state
    int m_currentTurn;
    bool m_isPlayer1Turn;
    QVector<QPoint> m_highlightedCells;
    static const QStringList GRID_FILES;

    QSet<QString> m_placedPlayer1Agents;
    QSet<QString> m_placedPlayer2Agents;
    QPushButton *m_goGameButton;
    void switchTurn();

    bool m_gameStarted = false;
     QWidget *m_hexGridWidget = nullptr;
     QPoint m_selectedStartPos;
     QString m_selectedAgentName;
     bool m_agentSelected = false;
     void moveAgentOnBoard(const QPoint &start, const QPoint &end);
     QMap<QString,int> m_agentHP; // current HP per agent name

     // helpers
     const DraggableItem* findDraggableItem(const QString &name) const;
     int hexDistance(const QPoint &a, const QPoint &b) const;
     QVector<QPoint> getHexNeighbors(int c, int r) const;
     bool canStandOnCellForAgentType(const QString &agentType, const QString &cellType) const;
     void removeAgentByName(const QString &name);
     QVector<QPoint> getRetreatCandidates(const QPoint &defenderPos, const QString &agentType, int mobility) const;

     // main attack function
     void performAttack(const QPoint &attackerPos, const QPoint &defenderPos);
    QPoint m_attackFlashCell ;
     bool m_flashActive = false;
    void checkForWinner();

private slots:
    void onAgentMoved(const QPoint& newPosition);
    void onAgentDestroyed(Agent* agent);
};

#endif // GAMEBOARD_H
