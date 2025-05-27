#include "gameboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <algorithm>
#include <cmath>
#include <QPixmap>
#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <queue>

constexpr int gameboard::HEX_SIZE;
constexpr double gameboard::HEX_WIDTH;
constexpr double gameboard::HEX_HEIGHT;

gameboard::gameboard(const QString& gridFile, const QString& gridStyle,
                     const QString& player1Name, const QString& player2Name, QWidget *parent)
    : QMainWindow(parent),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_gridFile(gridFile),
    m_gridStyle(gridStyle),
    m_currentTurn(1),
    m_isPlayer1Turn(true)
{
    setFixedSize(800,600);
    setupUI();
    initializeBoard();
    loadGridData();
    displayHexagonalGrid();
    setupSidePanel();
    createDraggableItems();
}

void gameboard::initializeBoard()
{
    m_board.clear();
    const int numColumns = 9;
    m_board.resize(numColumns);

    for (int col = 0; col < numColumns; ++col) {
        int rows = (col % 2) ? 4 : 5;
        m_board[col].resize(rows);

        for (int row = 0; row < rows; ++row) {
            m_board[col][row].col = col;
            m_board[col][row].row = row;
            m_board[col][row].type = "";
        }
    }
}

void gameboard::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    backgroundLabel = new QLabel(centralWidget);
    QPixmap bgImage(":/new/backmonsters4.jpg");
    if(bgImage.isNull()) {
        backgroundLabel->setStyleSheet("background-color: #222;");
    } else {
        bgImage = bgImage.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        backgroundLabel->setPixmap(bgImage);
    }
    backgroundLabel->setGeometry(0, 0, width(), height());

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    QWidget *gridContainer = new QWidget(centralWidget);
    QVBoxLayout *gridVLayout = new QVBoxLayout(gridContainer);
    gridVLayout->setContentsMargins(0, 0, 0, 0);
    gridVLayout->setSpacing(5);

    p1Label = new QLabel("Player 1: " + m_player1Name, gridContainer);
    p2Label = new QLabel("Player 2: " + m_player2Name, gridContainer);
    currentPlayerLabel = new QLabel("Current: " + m_player1Name, gridContainer);
    turnLabel = new QLabel("Turn: 1", gridContainer);

    p1Label->setStyleSheet("font-size: 18px; color: #FF5555; font-weight: bold;");
    p2Label->setStyleSheet("font-size: 18px; color: #5555FF; font-weight: bold;");
    currentPlayerLabel->setStyleSheet("font-size: 16px; color: white; font-weight: bold;");
    turnLabel->setStyleSheet("font-size: 16px; color: white; font-weight: bold;");

    QWidget *gridWidget = new QWidget(gridContainer);
    gridWidget->setStyleSheet("background: transparent;");

    gridVLayout->addWidget(p1Label, 0, Qt::AlignCenter);
    gridVLayout->addWidget(p2Label, 0, Qt::AlignCenter);
    gridVLayout->addWidget(currentPlayerLabel, 0, Qt::AlignCenter);
    gridVLayout->addWidget(turnLabel, 0, Qt::AlignCenter);
    gridVLayout->addWidget(gridWidget, 1);

    mainLayout->addWidget(gridContainer, 7);

    sidePanel = new QWidget(centralWidget);
    sidePanel->setFixedWidth(240);
    sidePanel->setStyleSheet("background-color: rgba(50, 50, 50, 200); border-radius: 10px;");
    mainLayout->addWidget(sidePanel, 3);

    backgroundLabel->lower();
}

void gameboard::loadGridData()
{
    QFile file(m_gridFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open grid file:" << m_gridFile;
        return;
    }

    QTextStream in(&file);
    int lineNum = 0;
    bool isOddLine = true;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        int pos = 0;
        int cellCount = 0;
        int expectedCells = isOddLine ? 5 : 4;

        while (pos < line.length() && cellCount < expectedCells) {
            if ((isOddLine && line[pos] == '/') || (!isOddLine && line[pos] == '\\')) {
                pos++;
                QString cell;
                while (pos < line.length() &&
                       ((isOddLine && line[pos] != '\\') ||
                        (!isOddLine && line[pos] != '/'))) {
                    cell += line[pos];
                    pos++;
                }
                pos++;
                int col = lineNum / 2;
                int row = cellCount;
                if (col < m_board.size() && row < m_board[col].size()) {
                    m_board[col][row].type = cell.trimmed();
                    if (m_board[col][row].type.isEmpty()) {
                        m_board[col][row].type = " ";
                    }
                }
                cellCount++;
            } else {
                pos++;
            }
        }

        isOddLine = !isOddLine;
        if (!isOddLine) lineNum++;
    }
    file.close();
}

void gameboard::displayHexagonalGrid()
{
    QWidget *gridWidget = qobject_cast<QVBoxLayout*>(centralWidget()->layout()->itemAt(0)->widget()->layout())->itemAt(4)->widget();

    QLayout* oldLayout = gridWidget->layout();
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    QWidget *hexGridWidget = new QWidget(gridWidget);
    hexGridWidget->setFixedSize(900, 700);

    QVBoxLayout *containerLayout = new QVBoxLayout(gridWidget);
    containerLayout->setContentsMargins(-50, -30, 0, 0);
    containerLayout->addWidget(hexGridWidget, 0, Qt::AlignLeft | Qt::AlignTop);  // Changed alignment

    hexGridWidget->installEventFilter(this);
}

void gameboard::drawBoard(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(12);
    painter.setFont(font);

    for (int col = 0; col < m_board.size(); ++col) {
        for (int row = 0; row < m_board[col].size(); ++row) {
            const GameCell &cell = m_board[col][row];
            QPoint center = hexagonCenter(col, row);

            QBrush brush;
            if (cell.type == "1") {
                brush = QColor(255, 85, 85);  // Player 1 (red)
            } else if (cell.type == "2") {
                brush = QColor(85, 85, 255);  // Player 2 (blue)
            } else if (cell.type == "#") {
                brush = Qt::darkGray;  // Rock (dark gray)
            } else if (cell.type == "~") {
                brush = QColor(100, 200, 255);  // Water (light blue)
            } else {
                brush = QColor(50, 50, 50);  // Empty (dark)
            }
            drawHexagon(painter, center, HEX_SIZE, brush);
            painter.setPen(Qt::white);
            painter.drawText(QRect(center.x() - 15, center.y() - 15, 30, 30),
                             Qt::AlignCenter, cell.type);
        }
    }
}

void gameboard::drawHexagon(QPainter &painter, const QPoint &center, int size, const QBrush &brush)
{
    QPolygon hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = 2 * M_PI / 6 * i;
        int x = static_cast<int>(center.x() + size * cos(angle));
        int y = static_cast<int>(center.y() + size * sin(angle));
        hexagon << QPoint(x, y);
    }
    painter.setBrush(brush);
    painter.setPen(Qt::black);
    painter.drawPolygon(hexagon);
}

QPoint gameboard::hexagonCenter(int col, int row) const
{
    int x = 450 + (col - 4) * HEX_WIDTH * 0.75;
    int y = 350;
    if (col % 2 == 0) {
        y += (row - 2) * HEX_HEIGHT;
    } else {
        y += (row - 1.5) * HEX_HEIGHT;
    }
    return QPoint(x, y);
}

QPixmap gameboard::createHexagonPixmap(const QString& content, const QColor& color)
{
    QPixmap pixmap(HEX_WIDTH, HEX_HEIGHT);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPolygonF hexagon;
    hexagon << QPointF(HEX_WIDTH/2, 0)
            << QPointF(HEX_WIDTH, HEX_HEIGHT*0.25)
            << QPointF(HEX_WIDTH, HEX_HEIGHT*0.75)
            << QPointF(HEX_WIDTH/2, HEX_HEIGHT)
            << QPointF(0, HEX_HEIGHT*0.75)
            << QPointF(0, HEX_HEIGHT*0.25);

    painter.setBrush(color);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawPolygon(hexagon);

    if (!content.isEmpty()) {
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(10);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, content);
    }

    return pixmap;
}

void gameboard::setupSidePanel()
{
    QVBoxLayout *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setAlignment(Qt::AlignTop);
    sideLayout->setContentsMargins(10, 10, 10, 10);
    sideLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("Available Units", sidePanel);
    titleLabel->setStyleSheet("font-size: 20px; color: white; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(titleLabel);

    QScrollArea *scrollArea = new QScrollArea(sidePanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { width: 12px; background: rgba(50, 50, 50, 150); }"
        "QScrollBar::handle:vertical { background: rgba(100, 100, 100, 200); min-height: 20px; }"
        );

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");

    m_itemsLayout = new QVBoxLayout(scrollContent);
    m_itemsLayout->setAlignment(Qt::AlignTop);
    m_itemsLayout->setSpacing(15);
    m_itemsLayout->setContentsMargins(0, 0, 5, 0);

    scrollArea->setWidget(scrollContent);
    sideLayout->addWidget(scrollArea);
}

void gameboard::createDraggableItems()
{
    m_draggableItems = {
                        {"Billy", ":/units/billy.png", 90, 1, 320, "WaterWalking"},
                        {"Reketon", ":/units/reketon.png", 80, 2, 320, "WaterWalking"},
                        {"Angus", ":/units/angus.png", 100, 1, 400, "WaterWalking"},
                        {"Duraham", ":/units/duraham.png", 100, 2, 320, "WaterWalking"},
                        {"Colonel Baba", ":/units/colonel_baba.png", 100, 1, 400, "WaterWalking"},
                        {"Medusa", ":/units/medusa.png", 90, 2, 320, "WaterWalking"},
                        {"Bunka", ":/units/bunka.png", 100, 1, 320, "WaterWalking"},
                        {"Sanka", ":/units/sanka.png", 100, 1, 320, "WaterWalking"},
                        {"Sir Lamorak", ":/units/sir_lamorak.png", 110, 1, 320, "Grounded"},
                        {"Kabu", ":/units/kabu.png", 120, 1, 400, "Grounded"},
                        {"Rajakal", ":/units/rajakal.png", 130, 1, 320, "Grounded"},
                        };

    for (const auto &item : m_draggableItems) {
        QPixmap pixmap(item.imagePath);
        if (!pixmap.isNull()) {
            m_itemPixmaps[item.name] = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }

    for (const auto &item : m_draggableItems) {
        QWidget *itemWidget = new QWidget();
        itemWidget->setFixedHeight(140);
        itemWidget->setStyleSheet(
            "QWidget {"
            "   background-color: rgba(70, 70, 70, 180);"
            "   border-radius: 8px;"
            "   border: 2px solid #555;"
            "}"
            "QWidget:hover {"
            "   background-color: rgba(90, 90, 90, 200);"
            "   border: 2px solid #777;"
            "}"
            );

        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(8, 8, 8, 8);
        itemLayout->setSpacing(10);

        QLabel *imageLabel = new QLabel(itemWidget);
        if (m_itemPixmaps.contains(item.name)) {
            imageLabel->setPixmap(m_itemPixmaps[item.name]);
        }
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setFixedSize(100, 100);

        imageLabel->setProperty("itemData", QVariant::fromValue(item));
        imageLabel->installEventFilter(this);

        QWidget *infoWidget = new QWidget(itemWidget);
        QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
        infoLayout->setSpacing(5);

        QLabel *nameLabel = new QLabel(item.name, infoWidget);
        QLabel *powerLabel = new QLabel(QString("⚔ Power: %1").arg(item.power), infoWidget);
        QLabel *rangeLabel = new QLabel(QString("🎯 Range: %1").arg(item.range), infoWidget);
        QLabel *healthLabel = new QLabel(QString("❤ Health: %1").arg(item.health), infoWidget);

        nameLabel->setStyleSheet("font-weight: bold; color: white; font-size: 14px;");
        powerLabel->setStyleSheet("color: #FF5555; font-size: 12px;");
        rangeLabel->setStyleSheet("color: #55AAFF; font-size: 12px;");
        healthLabel->setStyleSheet("color: #55FF55; font-size: 12px;");

        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(powerLabel);
        infoLayout->addWidget(rangeLabel);
        infoLayout->addWidget(healthLabel);

        itemLayout->addWidget(imageLabel);
        itemLayout->addWidget(infoWidget);
        m_itemsLayout->addWidget(itemWidget);
    }
}

bool gameboard::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Paint) {
        QWidget *hexGridWidget = qobject_cast<QWidget*>(obj);
        if (hexGridWidget) {
            QPaintEvent *paintEvent = static_cast<QPaintEvent*>(event);
            QPainter painter(hexGridWidget);
            drawBoard(painter);
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QLabel *label = qobject_cast<QLabel*>(obj);
            if (label && label->property("itemData").isValid()) {
                DraggableItem item = label->property("itemData").value<DraggableItem>();

                QDrag *drag = new QDrag(label);
                QMimeData *mimeData = new QMimeData;
                mimeData->setText(item.name);
                drag->setMimeData(mimeData);

                if (m_itemPixmaps.contains(item.name)) {
                    drag->setPixmap(m_itemPixmaps[item.name]);
                    drag->setHotSpot(QPoint(m_itemPixmaps[item.name].width()/2,
                                            m_itemPixmaps[item.name].height()/2));
                }

                drag->exec(Qt::CopyAction);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void gameboard::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void gameboard::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void gameboard::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        QString itemName = event->mimeData()->text();
        QPoint dropPos = event->pos();
        QPoint gridPos = convertToGridCoordinates(dropPos);
        addItemToGrid(itemName, gridPos);
        event->acceptProposedAction();
    }
}

QPoint gameboard::convertToGridCoordinates(const QPoint& screenPos)
{
    QWidget *gridWidget = qobject_cast<QVBoxLayout*>(centralWidget()->layout()->itemAt(0)->widget()->layout())->itemAt(4)->widget();
    QPoint relativePos = gridWidget->mapFromParent(screenPos);

    double col = (relativePos.x() - 450) / (HEX_WIDTH * 0.75) + 4;
    int roundedCol = qRound(col);

    double row;
    if (roundedCol % 2 == 0) {
        row = (relativePos.y() - 350) / HEX_HEIGHT + 2;
    } else {
        row = (relativePos.y() - 350) / HEX_HEIGHT + 1.5;
    }
    int roundedRow = qRound(row);

    roundedCol = qBound(0, roundedCol, m_board.size() - 1);
    if (roundedCol >= 0 && roundedCol < m_board.size()) {
        roundedRow = qBound(0, roundedRow, m_board[roundedCol].size() - 1);
    } else {
        roundedRow = 0;
    }

    return QPoint(roundedCol, roundedRow);
}

void gameboard::addItemToGrid(const QString& itemName, const QPoint& gridPos)
{
    auto it = std::find_if(m_draggableItems.begin(), m_draggableItems.end(),
                           [&itemName](const DraggableItem& item) { return item.name == itemName; });

    if (it != m_draggableItems.end() &&
        gridPos.x() >= 0 && gridPos.x() < m_board.size() &&
        gridPos.y() >= 0 && gridPos.y() < m_board[gridPos.x()].size()) {

        m_board[gridPos.x()][gridPos.y()].type = m_isPlayer1Turn ? "1" : "2";

        QWidget *gridWidget = qobject_cast<QVBoxLayout*>(centralWidget()->layout()->itemAt(0)->widget()->layout())->itemAt(4)->widget();
        gridWidget->update();
    }
}

void gameboard::highlightAvailableMoves(const QPoint& gridPos, int mobility)
{
    clearHighlights();
    std::queue<std::pair<QPoint, int>> q;
    q.push({gridPos, 0});

    QVector<QVector<bool>> visited(m_board.size(), QVector<bool>());
    for (int i = 0; i < m_board.size(); ++i) {
        visited[i].resize(m_board[i].size(), false);
    }
    visited[gridPos.x()][gridPos.y()] = true;

    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        QPoint pos = current.first;
        int dist = current.second;
        if (dist > mobility) continue;
        m_highlightedCells.append(pos);
        QVector<QPoint> neighbors = {
            QPoint(pos.x(), pos.y()-1),
            QPoint(pos.x()+1, pos.y()),
            QPoint(pos.x(), pos.y()+1),
            QPoint(pos.x()-1, pos.y()),
            QPoint(pos.x() + (pos.x()%2 ? 1 : -1), pos.y()-1),
            QPoint(pos.x() + (pos.x()%2 ? 1 : -1), pos.y()+1)
        };
        for (const QPoint& neighbor : neighbors) {
            if (neighbor.x() >= 0 && neighbor.x() < m_board.size() &&
                neighbor.y() >= 0 && neighbor.y() < m_board[neighbor.x()].size() &&
                !visited[neighbor.x()][neighbor.y()] &&
                m_board[neighbor.x()][neighbor.y()].type != "#") {

                visited[neighbor.x()][neighbor.y()] = true;
                q.push({neighbor, dist+1});
            }
        }
    }
    QWidget *gridWidget = qobject_cast<QVBoxLayout*>(centralWidget()->layout()->itemAt(0)->widget()->layout())->itemAt(4)->widget();
    gridWidget->update();
}

void gameboard::clearHighlights()
{
    m_highlightedCells.clear();
    QWidget *gridWidget = qobject_cast<QVBoxLayout*>(centralWidget()->layout()->itemAt(0)->widget()->layout())->itemAt(4)->widget();
    gridWidget->update();
}

void gameboard::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    // Paint the hex grid if needed
    QWidget *gridWidget = qobject_cast<QVBoxLayout*>(centralWidget()->layout()->itemAt(0)->widget()->layout())->itemAt(4)->widget();
    if (gridWidget) {
        QPainter painter(gridWidget);
        drawBoard(painter);
    }
}
