#ifndef GAMEBOARD_H
#define GAMEBOARD_H


#include "gridselection.h"
#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QBoxLayout>
#include <QPainter>

class QGridLayout;
class QScrollArea;
class QPushButton;

struct DraggableItem {
    QString name;
    QString imagePath;
    int power;
    int range;
    int health;
    QString type; // WaterWalking, Grounded, Flying, Floating
};

struct GameCell {
    int col;
    int row;
    QString type; // "1", "2", "#", "~", ""
};

class gameboard : public QMainWindow
{
    Q_OBJECT
public:
    explicit gameboard(const QString& gridFile,
                       const QString& gridStyle,
                       const QString& player1Name,
                       const QString& player2Name,
                       QWidget *parent = nullptr);
    virtual ~gameboard() = default;

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
    QPixmap createHexagonPixmap(const QString& content, const QColor& color);
    void setupSidePanel();
    void createDraggableItems();
    QPoint convertToGridCoordinates(const QPoint& screenPos);
    void addItemToGrid(const QString& itemName, const QPoint& gridPos);
    void highlightAvailableMoves(const QPoint& gridPos, int mobility);
    void clearHighlights();

    // Hexagon geometry constants
    static constexpr int HEX_SIZE = 30;
    static constexpr double HEX_WIDTH = HEX_SIZE * 2;
    static constexpr double HEX_HEIGHT = HEX_SIZE * 1.73205; // sqrt(3)

    QString m_player1Name;
    QString m_player2Name;
    QString m_gridFile;
    QString m_gridStyle;

    // Grid data structures
    QVector<QVector<QString>> m_gridData; // Original text-based grid data
    QVector<QVector<GameCell>> m_board;   // Hexagonal grid structure

    // UI elements
    QLabel *backgroundLabel;
    QVBoxLayout *m_itemsLayout;
    QWidget *sidePanel;

    // Draggable items
    QVector<DraggableItem> m_draggableItems;
    QMap<QString, QPixmap> m_itemPixmaps;
    gridselection *m_gridselectionPage;

    // Player info displays
    QLabel *p1Label;
    QLabel *p2Label;
    QLabel *currentPlayerLabel;
    QLabel *turnLabel;

    // Game state
    int m_currentTurn;
    bool m_isPlayer1Turn;
    QVector<QPoint> m_highlightedCells;
};

#endif // GAMEBOARD_H
