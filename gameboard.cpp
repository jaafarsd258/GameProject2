#include "gameboard.h"
#include <QDebug>
#include <QDrag>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <cmath>
#include <queue>
#include <QRandomGenerator>
#include <QMessageBox>


constexpr int gameboard::HEX_SIZE;
constexpr double gameboard::HEX_WIDTH;
constexpr double gameboard::HEX_HEIGHT;

// gameboard.cpp constructor
gameboard::gameboard(const QString &gridFile,
                     const QString &gridStyle,
                     const QString &player1Name,
                     const QString &player2Name,
                     QWidget *parent)
    : QMainWindow(parent),
    m_gridFile(gridFile),
    m_gridStyle(gridStyle),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_currentTurn(1),
    m_isPlayer1Turn(true)
{
    leftSidePanel = new QWidget(this);
    rightSidePanel = new QWidget(this);
    createDraggableItems();
    initializeBoard();
    setupUI();
    loadGridData();
    displayHexagonalGrid();
    setupPlayerPanels();
    qRegisterMetaType<DraggableItem>("DraggableItem");
}

void gameboard::setupUI()
{
    setFixedSize(1000,800);
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Background setup
    backgroundLabel = new QLabel(centralWidget);
    backgroundLabel->setGeometry(0, 0, this->width(), this->height());
    QPixmap bgImage(":/new/backmonsters5.jpg");
    if (bgImage.isNull()) {
        backgroundLabel->setStyleSheet("background-color: #222;");
    } else {
        bgImage = bgImage.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        backgroundLabel->setPixmap(bgImage);
    }
    backgroundLabel->lower();

    // Main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // Right panel (Player 1)
    leftSidePanel = new QWidget(centralWidget);
    leftSidePanel->setMinimumWidth(240);
    leftSidePanel->setStyleSheet("background-color: rgba(50, 50, 50, 200); border-radius: 10px;");

    // Left panel (Player 2)
    rightSidePanel = new QWidget(centralWidget);
    rightSidePanel->setMinimumWidth(240);
    rightSidePanel->setStyleSheet("background-color: rgba(50, 50, 50, 200); border-radius: 10px;");

    // Center grid container
    QWidget *gridContainer = new QWidget(centralWidget);
    QVBoxLayout *gridVLayout = new QVBoxLayout(gridContainer);
    gridVLayout->setContentsMargins(0, 0, 0, 0);
    gridVLayout->setSpacing(5);

    // Player info labels
    p1Label = new QLabel("Player 1: " + m_player1Name, gridContainer);
    p2Label = new QLabel("Player 2: " + m_player2Name, gridContainer);
    currentPlayerLabel = new QLabel("Current: " + m_player1Name, gridContainer);
    turnLabel = new QLabel("Turn: 1", gridContainer);

    p1Label->setStyleSheet("font-size: 18px; color: #FF5555; font-weight: bold;");
    p2Label->setStyleSheet("font-size: 18px; color: #5555FF; font-weight: bold;");
    currentPlayerLabel->setStyleSheet("font-size: 16px; color: white; font-weight: bold;");
    turnLabel->setStyleSheet("font-size: 16px; color: white; font-weight: bold;");

    // Grid widget
    QWidget *gridWidget = new QWidget(gridContainer);
    gridWidget->setStyleSheet("background: transparent;");
    gridWidget->setMinimumSize(800, 600);

    m_gridWidget = new QWidget(gridContainer); // Or wherever you create it

    gridVLayout->addWidget(p1Label, 0, Qt::AlignCenter);
    gridVLayout->addWidget(p2Label, 0, Qt::AlignCenter);
    gridVLayout->addWidget(currentPlayerLabel, 0, Qt::AlignCenter);
    gridVLayout->addWidget(turnLabel, 0, Qt::AlignCenter);
    gridVLayout->addWidget(gridWidget, 1);

    // Add panels to main layout
    mainLayout->addWidget(leftSidePanel, 2);
    mainLayout->addWidget(gridContainer, 6);
    mainLayout->addWidget(rightSidePanel, 2);
    QPushButton *backButton = new QPushButton("Back", centralWidget);
    backButton->move((width() - backButton->width()) / 2, height() - backButton->height() - 20);
    backButton->setFixedSize(100, 40);
    backButton->setStyleSheet(
        "QPushButton {"
        "   border: 2px solid #555;"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(100, 100, 100, 200), stop:1 rgba(150, 150, 150, 200));"
        "   color: white;"
        "   font: bold 10pt 'Arial Rounded MT';"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(200, 200, 200, 200);"
        "   border: 2px solid #FFD700;"
        "   color: #333;"
        "}");


    m_goGameButton = new QPushButton("Go Game", centralWidget);
    m_goGameButton->setFixedSize(100, 40);
    m_goGameButton->setStyleSheet(
        "QPushButton {"
        "   border: 2px solid #555;"
        "   background-color: gray;"
        "   color: white;"
        "   font: bold 10pt 'Arial Rounded MT';"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        "QPushButton:enabled {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(100, 200, 100, 200), stop:1 rgba(150, 250, 150, 200));"
        "}"
        "QPushButton:hover:enabled {"
        "   background-color: rgba(200, 255, 200, 200);"
        "   border: 2px solid #FFD700;"
        "   color: #333;"
        "}"
        );

    m_goGameButton->setEnabled(false);
    m_goGameButton->setVisible(true);


    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_goGameButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(centralWidget);
    mainVerticalLayout->addLayout(mainLayout);
    mainVerticalLayout->addLayout(buttonLayout);

    connect(m_goGameButton, &QPushButton::clicked, this, &gameboard::removeNumbersAfterPlacement);
}
void gameboard::setupPlayerPanels() {
    // Clear existing panels - safer version
    if (leftSidePanel->layout()) {
        QLayoutItem* item;
        while ((item = leftSidePanel->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

    if (rightSidePanel->layout()) {
        QLayoutItem* item;
        while ((item = rightSidePanel->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }

    // Setup left panel (Player 1)
    QVBoxLayout* leftLayout = new QVBoxLayout(leftSidePanel);
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->setContentsMargins(10, 10, 10, 10);

    QLabel* p1Title = new QLabel(m_player1Name, leftSidePanel);
    p1Title->setStyleSheet("font-size: 20px; color: #FF5555; font-weight: bold;");
    leftLayout->addWidget(p1Title, 0, Qt::AlignCenter);

    // Setup right panel (Player 2)
    QVBoxLayout* rightLayout = new QVBoxLayout(rightSidePanel);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setContentsMargins(10, 10, 10, 10);

    QLabel* p2Title = new QLabel(m_player2Name, rightSidePanel);
    p2Title->setStyleSheet("font-size: 20px; color: #5555FF; font-weight: bold;");
    rightLayout->addWidget(p2Title, 0, Qt::AlignCenter);

    leftLayout->setSpacing(10);
    rightLayout->setSpacing(10);

    updatePlayerPanels();
}
void gameboard::loadGridData()
{
    QFile file(m_gridFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open grid file:" << m_gridFile;
        return;
    }
    // Clear the board
    for (auto &column : m_board) {
        for (auto &cell : column) {
            cell.type = "";
        }
    }

    QTextStream in(&file);
    int lineNum = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        bool isOddLine = (lineNum % 2 == 1); // Line numbers start at 0
        int cellCount = 0;
        int pos = 0;

        while (pos < line.length()) {
            if (line[pos] == '/') {
                pos++;
                QString cellContent;

                // Extract cell content
                while (pos < line.length() && line[pos] != '\\') {
                    if (line[pos] != ' ' && line[pos] != '_') {
                        cellContent += line[pos];
                    }
                    pos++;
                }
                // Only process if we found valid content
                if (!cellContent.isEmpty()) {
                    int boardCol, boardRow;

                    if (isOddLine) {
                        boardCol = cellCount * 2; // Even columns for odd lines
                        boardRow = lineNum / 2;   // Integer division
                    } else {
                        boardCol = cellCount * 2 + 1; // Odd columns for even lines
                        boardRow = (lineNum - 1) / 2; // Adjusted for 0-based index
                    }

                    // Validate position and set cell content
                    if (boardCol >= 0 && boardCol < m_board.size() && boardRow >= 0
                        && boardRow < m_board[boardCol].size()
                        && (cellContent == "1" || cellContent == "2" || cellContent == "#"
                            || cellContent == "~")) {
                        m_board[boardCol][boardRow].type = cellContent;
                    }
                }

                cellCount++;
                if (pos < line.length() && line[pos] == '\\') {
                    pos++;
                }
            } else {
                pos++;
            }
        }
        lineNum++;
    }
    file.close();
}
void gameboard::updatePlayerPanels() {
    QVBoxLayout* leftLayout = leftSidePanel->layout() ?
                                  qobject_cast<QVBoxLayout*>(leftSidePanel->layout()) :
                                  new QVBoxLayout(leftSidePanel);

    QVBoxLayout* rightLayout = rightSidePanel->layout() ?
                                   qobject_cast<QVBoxLayout*>(rightSidePanel->layout()) :
                                   new QVBoxLayout(rightSidePanel);
    // Clear existing content (keep titles)
    while (leftLayout->count() > 1) {
        QLayoutItem* item = leftLayout->takeAt(1);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    while (rightLayout->count() > 1) {
        QLayoutItem* item = rightLayout->takeAt(1);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // Add Player1 agents to left panel as draggable items
    for (const auto &item : m_player1DraggableItems) {
        QWidget *itemWidget = createDraggableItemWidget(item, true);
        leftLayout->addWidget(itemWidget);
    }

    // Add Player 2 agents to right panel as draggable items
    for (const auto &item : m_player2DraggableItems) {
        QWidget *itemWidget = createDraggableItemWidget(item, false);
        rightLayout->addWidget(itemWidget);
    }
}

QWidget* gameboard::createDraggableItemWidget(const DraggableItem &item, bool isPlayer1) {
    QWidget *itemWidget = new QWidget(isPlayer1 ? leftSidePanel : rightSidePanel);
    itemWidget->setFixedHeight(120);
    itemWidget->setStyleSheet(
        "QWidget {"
        "   background-color: rgba(70, 70, 70, 180);"
        "   border-radius: 8px;"
        "   border: 2px solid #555;"
        "}"
        "QWidget:hover {"
        "   background-color: rgba(90, 90, 90, 200);"
        "   border: 2px solid #777;"
        "}");

    QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
    itemLayout->setContentsMargins(8, 8, 8, 8);
    itemLayout->setSpacing(10);

    QLabel *imageLabel = new QLabel(itemWidget);
    // Use the correct pixmap map based on player
    QPixmap pixmap;

    if ((isPlayer1 && m_player1ItemPixmaps.contains(item.name))) {
        pixmap = m_player1ItemPixmaps[item.name];
    } else if (!isPlayer1 && m_player2ItemPixmaps.contains(item.name)) {
        pixmap = m_player2ItemPixmaps[item.name];
    } else if (m_itemPixmaps.contains(item.name)) {
        pixmap = m_itemPixmaps[item.name];
    } else {
        pixmap.load(item.imagePath);
        if (!pixmap.isNull()) {
            m_itemPixmaps[item.name] = pixmap;
        }
    }

    if (!pixmap.isNull()) {
        const QSize thumbSize(80, 80);
        QPixmap thumb = pixmap.scaled(thumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        imageLabel->setPixmap(thumb);
        imageLabel->setFixedSize(thumb.size());
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setScaledContents(false);
    }

    imageLabel->setFixedSize(80, 80);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setProperty("itemData", QVariant::fromValue(item));
    imageLabel->installEventFilter(this);

    QWidget *infoWidget = new QWidget(itemWidget);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setSpacing(3);
    infoLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel(item.name, infoWidget);
    QLabel *powerLabel = new QLabel(QString("Power: %1").arg(item.power), infoWidget);
    QLabel *rangeLabel = new QLabel(QString("Range: %1").arg(item.range), infoWidget);
    QLabel *healthLabel = new QLabel(QString("Health: %1").arg(item.health), infoWidget);

    nameLabel->setStyleSheet("font-weight: bold; color: white; font-size: 12px;");
    powerLabel->setStyleSheet("color: #FF5555; font-size: 11px;");
    rangeLabel->setStyleSheet("color: #55AAFF; font-size: 11px;");
    healthLabel->setStyleSheet("color: #55FF55; font-size: 11px;");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(powerLabel);
    infoLayout->addWidget(rangeLabel);
    infoLayout->addWidget(healthLabel);

    itemLayout->addWidget(imageLabel);
    itemLayout->addWidget(infoWidget);

    return itemWidget;
}
void gameboard::removeNumbersAfterPlacement() {
    for (auto &column : m_board) {
        for (auto &cell : column) {
            if ((cell.type == "1" || cell.type == "2") && cell.agentName.isEmpty()) {
                cell.type = "";
            }
        }
    }
    m_gameStarted = true;
    m_goGameButton->setVisible(false); // إخفاء الزر بعد الضغط عليه
     switchTurn();
    // تحديث عرض الشبكة
     if (m_hexGridWidget) m_hexGridWidget->update();


}
void gameboard::displayHexagonalGrid() {
    // Get the grid widget from the layout
    QWidget *gridWidget = qobject_cast<QVBoxLayout*>(
                              centralWidget()->layout()->itemAt(1)->widget()->layout()
                              )->itemAt(4)->widget();
    // Clear existing content
    QLayout* layout = gridWidget->layout();
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    } else {
        layout = new QVBoxLayout(gridWidget);
        layout->setContentsMargins(0, 0, 0, 0);
    }

    m_hexGridWidget = new QWidget(gridWidget);
    m_hexGridWidget->setObjectName("hexGridWidget");
    m_hexGridWidget->setMinimumSize(800, 600);
    layout->addWidget(m_hexGridWidget);
    m_hexGridWidget->installEventFilter(this);
    m_hexGridWidget->update();
}
void gameboard::drawBoard(QPainter &painter) {
    // Set up text drawing properties
    QFont font = painter.font();
    font.setPointSize(12);  // Adjust size as needed
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(Qt::white);  // Default text color

    for (int col = 0; col < m_board.size(); ++col) {
        for (int row = 0; row < m_board[col].size(); ++row) {
            const GameCell &cell = m_board[col][row];
            QPoint center = hexagonCenter(col, row);
            painter.setRenderHint(QPainter::Antialiasing);
            QBrush brush;
            QString cellText = "";  // Text to display on empty cells
            if (m_highlightedCells.contains(QPoint(col, row))) {
                brush = QColor(144, 238, 144, 150); // light transparent green
            }
            else {
                if (cell.type == "1") {
                    if (!m_gameStarted) {
                        brush = QColor(255, 85, 85, 100);
                        cellText = "1";
                    }
                    else {
                        brush = QColor(50, 50, 50);
                    }
                }
                else if (cell.type == "2") {
                    if (!m_gameStarted) {
                        brush = QColor(85, 85, 255, 100);
                        cellText = "2";
                    }
                    else {
                        brush = QColor(50, 50, 50);
                    }
                }
                else if (cell.type == "#") {
                    brush = Qt::darkGray;
                    cellText = "#";
                }
                else if (cell.type == "~") {
                    brush = QColor(100, 200, 255);
                    cellText = "~";
                    painter.setPen(Qt::black);  // Darker text for light blue background
                }
                else {
                    brush = QColor(50, 50, 50);
                }
            }
            drawHexagon(painter, center, HEX_SIZE, brush);
            if (m_flashActive && QPoint(col, row) == m_attackFlashCell) {
                drawHexagon(painter, center, HEX_SIZE, QBrush(QColor(255, 0, 0, 150)));
            }
            if (!cell.agentName.isEmpty()) {
                // Reset text color to white for agent cells
                painter.setPen(Qt::white);
                QPixmap agentPixmap;
                if (cell.type == "1") {
                    if (m_player1ItemPixmaps.contains(cell.agentName)) {
                        agentPixmap = m_player1ItemPixmaps.value(cell.agentName);
                    } else if (m_itemPixmaps.contains(cell.agentName)) {
                        agentPixmap = m_itemPixmaps.value(cell.agentName);
                    }
                } else {
                    if (m_player2ItemPixmaps.contains(cell.agentName)) {
                        agentPixmap = m_player2ItemPixmaps.value(cell.agentName);
                    } else if (m_itemPixmaps.contains(cell.agentName)) {
                        agentPixmap = m_itemPixmaps.value(cell.agentName);
                    }
                }
                if (!agentPixmap.isNull()) {
                    painter.setRenderHint(QPainter::Antialiasing, false);
                    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
                    int imageSize = HEX_SIZE * 0.9;
                    QPixmap scaledPixmap = agentPixmap.scaled(
                        imageSize, imageSize,
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation
                        );
                    painter.drawPixmap(
                        center.x() - scaledPixmap.width() / 2,
                        center.y() - scaledPixmap.height() / 2,
                        scaledPixmap
                        );
                }
            } else if (!cellText.isEmpty()) {
                // Draw the cell text (1, 2, #, ~) only if there's no agent
                painter.drawText(
                    QRect(center.x() - HEX_SIZE, center.y() - HEX_SIZE,
                          HEX_SIZE * 2, HEX_SIZE * 2),
                    Qt::AlignCenter,
                    cellText
                    );
            }
            painter.setPen(Qt::white);

        }
    }
}
void gameboard::drawHexagon(QPainter &painter, const QPoint &center, int size, const QBrush &brush) {
    QPolygon hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = 2 * M_PI / 6 * i;
        int x = center.x() + size * cos(angle);
        int y = center.y() + size * sin(angle);
        hexagon << QPoint(x, y);
    }
    painter.setBrush(brush);
    painter.setPen(Qt::black);
    painter.drawPolygon(hexagon);
}

QPoint gameboard::hexagonCenter(int col, int row) const {
    double x = HEX_WIDTH * 0.75 * col;
    double y = HEX_HEIGHT * (row + 0.5 * (col % 2));
    return QPoint(x + HEX_WIDTH, y + HEX_HEIGHT); // Adjust offsets as needed
}

QPixmap gameboard::createHexagonPixmap(const QString &content, const QColor &color)
{
    QPixmap pixmap(HEX_WIDTH, HEX_HEIGHT);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPolygonF hexagon;
    hexagon << QPointF(HEX_WIDTH / 2, 0) << QPointF(HEX_WIDTH, HEX_HEIGHT * 0.25)
            << QPointF(HEX_WIDTH, HEX_HEIGHT * 0.75) << QPointF(HEX_WIDTH / 2, HEX_HEIGHT)
            << QPointF(0, HEX_HEIGHT * 0.75) << QPointF(0, HEX_HEIGHT * 0.25);
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

void gameboard::createDraggableItems()
{
    m_draggableItems = {
        // WaterWalking
        {"Billy", ":/image/billy.png", 90, 1, 320, "WaterWalking", 3},
        {"Reketon", ":/image/reketon.png", 80, 2, 320, "WaterWalking", 2},
        {"Angus", ":/image/angus.png", 100, 1, 400, "WaterWalking", 2},
        {"Duraham", ":/image/duraham.png", 100, 2, 320, "WaterWalking", 2},
        {"Colonel Baba", ":/image/colonel_baba.png", 100, 1, 400, "WaterWalking", 2},
        {"Medusa", ":/image/medusa.png", 90, 2, 320, "WaterWalking", 2},
        {"Bunka", ":/image/bunka.png", 100, 1, 320, "WaterWalking", 3},
        {"Sanka", ":/image/sanka.png", 100, 1, 320, "WaterWalking", 3},

        // Grounded
        {"Sir Lamorak", ":/image/sir_lamorak.png", 110, 1, 320, "Grounded", 3},
        {"Kabu", ":/image/kabu.png", 120, 1, 400, "Grounded", 2},
        {"Rajakal", ":/image/rajakal.png", 130, 1, 320, "Grounded", 2},
        {"Salih", ":/image/salih.png", 80, 1, 400, "Grounded", 2},
        {"Khan", ":/image/khan.png", 90, 1, 320, "Grounded", 2},
        {"Boi", ":/image/boi.png", 100, 1, 400, "Grounded", 2},
        {"Eloi", ":/image/eloi.png", 100, 2, 240, "Grounded", 2},
        {"Kanar", ":/image/kanar.png", 100, 2, 160, "Grounded", 2},
        {"Elsa", ":/image/elsa.png", 140, 2, 320, "Grounded", 2},
        {"Karissa", ":/image/karissa.png", 80, 2, 280, "Grounded", 2},
        {"Sir Philip", ":/image/sir_philip.png", 100, 1, 400, "Grounded", 2},
        {"Frost", ":/image/frost.png", 80, 2, 260, "Grounded", 2},
        {"Tusk", ":/image/tusk.png", 100, 1, 400, "Grounded", 2},

        // Flying
        {"Rambu", ":/image/rambu.png", 120, 1, 320, "Flying", 3},

        // Floating
        {"Sabrina", ":/image/sabrina.png", 100, 1, 320, "Floating", 3},
        {"Death", ":/image/death.png", 120, 2, 240, "Floating", 3}
    };
}
bool gameboard::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_hexGridWidget && event->type() == QEvent::Paint) {
        Q_UNUSED(event);
        QPainter painter(static_cast<QWidget *>(obj));
        drawBoard(painter);
        return true; // منع المعالجة الافتراضية لأننا رسمنا هنا
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton)
            return QMainWindow::eventFilter(obj, event);

        // 1) نقر على لوحة الـ hex
        if (obj == m_hexGridWidget) {
            if (!m_hexGridWidget) return true;
            // تحويل الإحداثيات بشكل واضح: نأخذ موضع النقر داخل الـ hex widget
            QPoint mainPos = m_hexGridWidget->mapTo(this, mouseEvent->pos());
            QPoint gridPos = convertToGridCoordinates(mainPos);
            // تحقق من حدود اللوحة
            if (gridPos.x() < 0 || gridPos.x() >= m_board.size()) return true;
            if (gridPos.y() < 0 || gridPos.y() >= m_board[gridPos.x()].size()) return true;
            // إذا اللعبة بعد ما بلشت، نستهلك الحدث كما كان في كودك الأصلي
            if (!m_gameStarted) {
                return true;
            }
            GameCell &cell = m_board[gridPos.x()][gridPos.y()];
            // إذا الخانة فيها وكيل
            if (!cell.agentName.isEmpty()) {
                bool isAgent1 = m_player1Agents.contains(cell.agentName);
                bool isOwn = (m_isPlayer1Turn && isAgent1) || (!m_isPlayer1Turn && !isAgent1);

                if (isOwn) {
                    // نحدد وكيلنا ونظهر الـ highlights
                    const DraggableItem* itemPtr = findDraggableItem(cell.agentName);
                    if (itemPtr) {
                        m_selectedAgentName = cell.agentName;
                        m_selectedStartPos = gridPos;
                        m_agentSelected = true;
                        highlightMovesForAgent(m_selectedStartPos, itemPtr->type, itemPtr->mobility);
                    }
                    return true;
                } else {
                    // خانة فيها وكيل عدو: محاولة الهجوم **فقط** إذا في مهاجم محدد
                    if (m_agentSelected && !m_selectedAgentName.isEmpty()) {
                        const DraggableItem* attackerItem = findDraggableItem(m_selectedAgentName);
                        if (attackerItem) {
                            int d = hexDistance(m_selectedStartPos, gridPos);
                            if (d <= attackerItem->range) {
                                // فعّل الهجوم
                                performAttack(m_selectedStartPos, gridPos);
                                // performAttack يقوم بتحديث الـ board ويدير الـ turn، فبننهي حالة الاختيار هنا
                                clearHighlights();
                                m_agentSelected = false;
                                m_selectedAgentName.clear();
                                return true;
                            } else {
                                // خارج نطاق الهجوم — لا نمسح الاختيار فوراً لتسمح للمستخدم بتعديل الهدف
                                qDebug() << "Target out of attack range (clicked):" << d << ">" << attackerItem->range;
                                // (لا نلغي الـ highlights أو التحرير هنا)
                                return true;
                            }
                        }
                        // لو ما حصلنا على data للمهاجم نلغي التحديد حفاظاً على التناسق
                        clearHighlights();
                        m_agentSelected = false;
                        m_selectedAgentName.clear();
                        return true;
                    }
                    // إذا ما كان هناك مهاجم محدد، نستهلك الحدث (أو يمكن إظهار معلومات العدو)
                    return true;
                }
            }
            else {
                // الخانة فاضية: نتحقق إذا الخانة في الـ highlights وننفّذ الحركة
                if (m_agentSelected && m_highlightedCells.contains(gridPos)) {
                    moveAgentOnBoard(m_selectedStartPos, gridPos);
                }
                clearHighlights();
                m_agentSelected = false;
                m_selectedAgentName.clear();
                return true;
            }
        } // end hexGridWidget handling

        // 2) نقر على صورة وحش في اللوحة الجانبية (أيقونات / كارد)
        QLabel *label = qobject_cast<QLabel *>(obj);
        if (label && label->property("itemData").isValid()) {
            DraggableItem item = label->property("itemData").value<DraggableItem>();
            // إذا الوحش موجود على اللوحة → نحدده ونظهر حركاته
            for (int col = 0; col < m_board.size(); ++col) {
                for (int row = 0; row < m_board[col].size(); ++row) {
                    if (m_board[col][row].agentName == item.name) {
                        QPoint startPos(col, row);
                        m_selectedAgentName = item.name;
                        m_selectedStartPos = startPos;
                        m_agentSelected = true;
                        if (m_gameStarted) {
                            highlightMovesForAgent(startPos, item.type, item.mobility);
                        }
                        return true;
                    }
                }
            }
            // إذا الوحش غير موضوع بعد → نعمل سحب لكن نتأكد أي لوحة جانبية هو تابع لها
            QWidget *ancestor = label->parentWidget();
            while (ancestor && ancestor != leftSidePanel && ancestor != rightSidePanel) {
                ancestor = ancestor->parentWidget();
            }
            bool inLeft = (ancestor == leftSidePanel);
            bool inRight = (ancestor == rightSidePanel);
            if ((m_isPlayer1Turn && inLeft && !m_placedPlayer1Agents.contains(item.name)) ||
                (!m_isPlayer1Turn && inRight && !m_placedPlayer2Agents.contains(item.name))) {
                QDrag *drag = new QDrag(label);
                QMimeData *mimeData = new QMimeData;
                mimeData->setText(item.name);
                drag->setMimeData(mimeData);

                QPixmap sourcePixmap;
                QPixmap lblPixmap = label->pixmap();
                if (!lblPixmap.isNull()) {
                    sourcePixmap = lblPixmap;
                } else if (m_itemPixmaps.contains(item.name) && !m_itemPixmaps[item.name].isNull()) {
                    sourcePixmap = m_itemPixmaps[item.name];
                }
                if (!sourcePixmap.isNull()) {
                    const double scaleFactor = 0.9;
                    const int dragSide = static_cast<int>(HEX_SIZE * scaleFactor);
                    QSize dragSize(dragSide, dragSide);
                    QPixmap dragPixmap = sourcePixmap.scaled(dragSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    drag->setPixmap(dragPixmap);
                    drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));
                }

                drag->exec(Qt::CopyAction);
                return true;
            }
        }
    } // end MouseButtonPress

    // افتراضياً ننقل الحدث للـ base class
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

QPoint gameboard::convertToGridCoordinates(const QPoint &screenPos) {
    if (!m_hexGridWidget) return QPoint(-1, -1); // أو أي سلوك مناسب لو مش موجود
    QPoint relativePos = m_hexGridWidget->mapFrom(this, screenPos);
    double hexWidth = HEX_WIDTH * 0.75;
    double hexHeight = HEX_HEIGHT;
    double col = (relativePos.x() - HEX_WIDTH) / hexWidth;
    int roundedCol = qRound(col);
    double row;
    if (roundedCol % 2 == 0) {
        row = (relativePos.y() - HEX_HEIGHT/2) / hexHeight;
    } else {
        row = (relativePos.y() - HEX_HEIGHT) / hexHeight;
    }
    int roundedRow = qRound(row);

    roundedCol = qBound(0, roundedCol, m_board.size()-1);
    if (roundedCol >= 0 && roundedCol < m_board.size()) {
        roundedRow = qBound(0, roundedRow, m_board[roundedCol].size()-1);
    } else {
        roundedRow = 0;
    }

    return QPoint(roundedCol, roundedRow);
}

void gameboard::addItemToGrid(const QString &itemName, const QPoint &gridPos) {
    if (gridPos.x() < 0 || gridPos.x() >= m_board.size() ||
        gridPos.y() < 0 || gridPos.y() >= m_board[gridPos.x()].size()) {
        return; // خارج الحدود
    }
    GameCell &cell = m_board[gridPos.x()][gridPos.y()];
    // الشرط الجديد: الخلية يجب أن تكون من نوع اللاعب الحالي (1 أو 2) وخالية
    bool isCellValid = (m_isPlayer1Turn && cell.type == "1") || (!m_isPlayer1Turn && cell.type == "2");
    if (!isCellValid || cell.type == "#" || cell.type == "~" || !cell.agentName.isEmpty()) {
        qDebug() << "لا يمكن وضع الوحش هنا!";
        return;
    }
    // تحديث الخلية (بدون تعديل نوعها)
    cell.agentName = itemName;
    // تحديث مجموعة الوحوش الموضوعة
    if (m_isPlayer1Turn) {
        m_placedPlayer1Agents.insert(itemName);
    } else {
        m_placedPlayer2Agents.insert(itemName);
    }
    // إنشاء الوحش وإضافته إلى اللعبة
    Agent* agent = Agent::createAgent(itemName, gridPos, this);
    if (agent) {
        m_agents.append(agent);
        connect(agent, &Agent::positionChanged, this, &gameboard::onAgentMoved);
        connect(agent, &Agent::agentDestroyed, this, &gameboard::onAgentDestroyed);
    }
    const DraggableItem* di = findDraggableItem(itemName);
    if (di) m_agentHP[itemName] = di->health;
    if (m_hexGridWidget) m_hexGridWidget->update();
    // إذا اللاعب حط كل وحوشه، نقلب الدور:
    bool player1Done = m_placedPlayer1Agents.size() == 4;
    bool player2Done = m_placedPlayer2Agents.size() == 4;
    // تحقق إذا يجب إظهار زر Go Game
    if (player1Done && player2Done) {
        m_goGameButton->setEnabled(true);
        m_goGameButton->setVisible(true);
    } else if ((m_isPlayer1Turn && player1Done) || (!m_isPlayer1Turn && player2Done)) {
        switchTurn();
    }

}
void gameboard::highlightAvailableMoves(const QPoint &gridPos, int mobility)
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
        if (dist > mobility)
            continue;
        m_highlightedCells.append(pos);
        QVector<QPoint> neighbors = {QPoint(pos.x(), pos.y() - 1),
                                     QPoint(pos.x() + 1, pos.y()),
                                     QPoint(pos.x(), pos.y() + 1),
                                     QPoint(pos.x() - 1, pos.y()),
                                     QPoint(pos.x() + (pos.x() % 2 ? 1 : -1), pos.y() - 1),
                                     QPoint(pos.x() + (pos.x() % 2 ? 1 : -1), pos.y() + 1)};
        for (const QPoint &neighbor : neighbors) {
            if (neighbor.x() >= 0 && neighbor.x() < m_board.size() && neighbor.y() >= 0
                && neighbor.y() < m_board[neighbor.x()].size()
                && !visited[neighbor.x()][neighbor.y()]
                && m_board[neighbor.x()][neighbor.y()].type != "#") {
                visited[neighbor.x()][neighbor.y()] = true;
                q.push({neighbor, dist + 1});
            }
        }
    }
    if (m_hexGridWidget) m_hexGridWidget->update();

}

void gameboard::clearHighlights()
{
    m_highlightedCells.clear();
    if (m_hexGridWidget) m_hexGridWidget->update();
}

void gameboard::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    QWidget *gridWidget = nullptr;
    if (centralWidget() && centralWidget()->layout()) {
        QLayoutItem *mainItem = centralWidget()->layout()->itemAt(0);
        if (mainItem && mainItem->widget() && mainItem->widget()->layout()) {
            QLayoutItem *gridItem = mainItem->widget()->layout()->itemAt(4);
            if (gridItem) {
                gridWidget = gridItem->widget();
            }
        }
    }
    if (gridWidget) {
        QPainter painter(gridWidget);
        drawBoard(painter);
    }
}
void gameboard::createDraggableItemsFromAgents(const QStringList& agents, bool isPlayer1) {
    if (isPlayer1) {
        m_player1DraggableItems.clear();
        m_player1ItemPixmaps.clear();
    } else {
        m_player2DraggableItems.clear();
        m_player2ItemPixmaps.clear();
    }
    for (const QString& agentName : agents) {
        auto it = std::find_if(m_draggableItems.begin(), m_draggableItems.end(),
                               [&agentName](const DraggableItem& item) {
                                   return item.name == agentName;
                               });
        if (it != m_draggableItems.end()) {
            if (isPlayer1) {
                m_player1DraggableItems.append(*it);
                if (m_itemPixmaps.contains(agentName)) {
                    m_player1ItemPixmaps[agentName] = m_itemPixmaps[agentName];
                }
            } else {
                m_player2DraggableItems.append(*it);
                if (m_itemPixmaps.contains(agentName)) {
                    m_player2ItemPixmaps[agentName] = m_itemPixmaps[agentName];
                }
            }
        }
    }
    updatePlayerPanels();
}
QWidget* gameboard::createAgentCard(const QString& agentName, bool isPlayer1) {
    const DraggableItem* item = nullptr;
    const QMap<QString, QPixmap>& pixmaps = isPlayer1 ? m_player1ItemPixmaps : m_player2ItemPixmaps;
    const QVector<DraggableItem>& items = isPlayer1 ? m_player1DraggableItems : m_player2DraggableItems;
    for (const DraggableItem& i : items) {
        if (i.name == agentName) {
            item = &i;
            break;
        }
    }
    if (!item) return nullptr;
    QWidget* card = new QWidget();
    card->setFixedHeight(120);
    card->setStyleSheet(
        "QWidget {"
        "   background-color: rgba(70, 70, 70, 180);"
        "   border-radius: 8px;"
        "   border: 2px solid #555;"
        "}"
        "QWidget:hover {"
        "   background-color: rgba(90, 90, 90, 200);"
        "   border: 2px solid #777;"
        "}");

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    // Image
    QLabel* imageLabel = new QLabel();
    if (pixmaps.contains(agentName)) {
        imageLabel->setPixmap(pixmaps[agentName]);
    }
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setFixedSize(80, 80);
    imageLabel->setProperty("itemData", QVariant::fromValue(*item));
    imageLabel->installEventFilter(this);

    // Info
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setSpacing(3);
    infoLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* nameLabel = new QLabel(item->name);
    QLabel* powerLabel = new QLabel(QString("Power: %1").arg(item->power));
    QLabel* rangeLabel = new QLabel(QString("Range: %1").arg(item->range));
    QLabel* healthLabel = new QLabel(QString("Health: %1").arg(item->health));

    nameLabel->setStyleSheet("font-weight: bold; color: white; font-size: 12px;");
    powerLabel->setStyleSheet("color: #FF5555; font-size: 11px;");
    rangeLabel->setStyleSheet("color: #55AAFF; font-size: 11px;");
    healthLabel->setStyleSheet("color: #55FF55; font-size: 11px;");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(powerLabel);
    infoLayout->addWidget(rangeLabel);
    infoLayout->addWidget(healthLabel);
    layout->addWidget(imageLabel);
    layout->addWidget(infoWidget);

    return card;
}

void gameboard::setPlayer1Agents(const QStringList& agents) {
    m_player1Agents = agents;
    createDraggableItemsFromAgents(agents, true);
    updatePlayerPanels();
}

void gameboard::setPlayer2Agents(const QStringList& agents) {
    m_player2Agents = agents;
    createDraggableItemsFromAgents(agents, false);
    updatePlayerPanels();
}

void gameboard::placeAgentsOnBoard(int player, const QStringList& agents) {
    // 1. التحقق الصامت من صحة المدخلات
    if (player != 1 && player != 2) return;
    if (agents.empty()) return;
    QVector<QPoint> startPositions;
    // 2. جمع المواقع الصالحة
    for (int col = 0; col < m_board.size(); ++col) {
        const QVector<GameCell>& column = m_board[col];
        for (int row = 0; row < column.size(); ++row) {
            const GameCell& cell = column[row];
            if (cell.type == QString::number(player) && cell.agentName.isEmpty()) {
                startPositions.append(QPoint(col, row));
            }
        }
    }
    // 3. ترتيب المواقع
    std::sort(startPositions.begin(), startPositions.end(),
              [](const QPoint& a, const QPoint& b) {
                  return (a.x() < b.x()) || (a.x() == b.x() && a.y() < b.y());
              });
    // 4. وضع الوحوش
    for (int i = 0; i < qMin(agents.size(), startPositions.size()); ++i) {
        const QPoint& pos = startPositions[i];
        GameCell& cell = m_board[pos.x()][pos.y()];
        if (!cell.agentName.isEmpty()) continue;
        if (Agent* agent = Agent::createAgent(agents[i], pos, this)) {
            cell.agentName = agents[i];
            m_agents.append(agent);
            const DraggableItem* di = findDraggableItem(agents[i]);
            if (di) m_agentHP[agents[i]] = di->health;
            connect(agent, &Agent::positionChanged, this, &gameboard::onAgentMoved);
            connect(agent, &Agent::agentDestroyed, this, &gameboard::onAgentDestroyed);
            if (player == 1) {
                m_placedPlayer1Agents.insert(agents[i]);
            } else {
                m_placedPlayer2Agents.insert(agents[i]);
            }
        }
    }
    // 5. تحديث الواجهة
    if (QWidget* gridWidget = findChild<QWidget*>("gridWidget")) {
        gridWidget->update();
    }
}
const QStringList gameboard::GRID_FILES = {
    ":/grids/grid1.txt",
    ":/grids/grid2.txt",
    ":/grids/grid3.txt",
    ":/grids/grid4.txt",
    ":/grids/grid5.txt",
    ":/grids/grid6.txt",
    ":/grids/grid7.txt",
    ":/grids/grid8.txt"
};
const QStringList& gameboard::getGridFiles() {
    return GRID_FILES;
}
void gameboard::onAgentMoved(const QPoint& newPosition) {
    Agent* agent = qobject_cast<Agent*>(sender());
    if (agent) {
        qDebug() << "Agent" << agent->name() << "moved to" << newPosition;
        switchTurn();
    }
}

void gameboard::onAgentDestroyed(Agent* agent) {
    if (agent) {
        // Find and clear the cell containing this agent
        for (auto &column : m_board) {
            for (auto &cell : column) {
                if (cell.agentName == agent->name()) {
                    // remove from placed sets by owner (cell.type may not be reliable at runtime)
                    if (m_player1Agents.contains(agent->name())) {
                        m_placedPlayer1Agents.remove(agent->name());
                    } else if (m_player2Agents.contains(agent->name())) {
                        m_placedPlayer2Agents.remove(agent->name());
                    }
                    cell.agentName.clear();
                    break;
                }
            }
        }
        m_agents.removeAll(agent);
        agent->deleteLater();
        if (m_hexGridWidget) m_hexGridWidget->update();
    }
}
void gameboard::initializeBoard()
{
    m_placedPlayer1Agents.clear();
    m_placedPlayer2Agents.clear();
    m_board.clear();
    m_gameStarted = false;
    const int numColumns = 9;
    m_board.resize(numColumns);
    for (int col = 0; col < numColumns; ++col) {
        int rows = (col % 2 == 0) ? 5 : 4; // Even columns have 5 rows, odd have 4
        m_board[col].resize(rows);

        for (int row = 0; row < rows; ++row) {
            m_board[col][row].col = col;
            m_board[col][row].row = row;
            m_board[col][row].type = ""; // Initialize as empty
        }
    }
}

void gameboard::switchTurn()
{
    m_isPlayer1Turn = !m_isPlayer1Turn;
    if (m_isPlayer1Turn) {
        m_currentTurn++;
    }
    currentPlayerLabel->setText("Current: " + (m_isPlayer1Turn ? m_player1Name : m_player2Name));
    turnLabel->setText("Turn: " + QString::number(m_currentTurn));

    update();
}
void gameboard::highlightMovesForAgent(const QPoint &startPos, const QString &agentType, int mobility)
{
    if (!m_gameStarted) {
        clearHighlights();
        return;
    }
    m_highlightedCells.clear();
    auto isEnemyAgent = [this](const QString &agentName) {
        bool isAgent1 = m_player1Agents.contains(agentName);
        return (m_isPlayer1Turn && !isAgent1) || (!m_isPlayer1Turn && isAgent1);
    };
    auto neighborsOf = [&](int c, int r) {
        QVector<QPoint> n;
        if (c % 2 == 0) {
            n = {
                QPoint(c,     r - 1),  // up
                QPoint(c + 1, r - 1),  // up-right
                QPoint(c + 1, r),      // down-right
                QPoint(c,     r + 1),  // down
                QPoint(c - 1, r),      // down-left
                QPoint(c - 1, r - 1)   // up-left
            };
        } else {
            n = {
                QPoint(c,     r - 1),  // up
                QPoint(c + 1, r),      // up-right
                QPoint(c + 1, r + 1),  // down-right
                QPoint(c,     r + 1),  // down
                QPoint(c - 1, r + 1),  // down-left
                QPoint(c - 1, r)       // up-left
            };
        }
        return n;
    };

    // BFS queue: position + accumulated cost
    std::queue<std::pair<QPoint,int>> q;
    q.push({startPos, 0});

    QVector<QVector<bool>> visited(m_board.size());
    for (int i = 0; i < m_board.size(); ++i) {
        visited[i].resize(m_board[i].size(), false);
    }
    visited[startPos.x()][startPos.y()] = true;
    while (!q.empty()) {
        auto cur = q.front(); q.pop();
        QPoint pos = cur.first;
        int dist = cur.second;
        if (dist > mobility) continue;
        if (dist > 0) m_highlightedCells.append(pos);
        QVector<QPoint> neigh = neighborsOf(pos.x(), pos.y());
        for (const QPoint &n : neigh) {
            // حدود اللوحة
            if (n.x() < 0 || n.x() >= m_board.size()) continue;
            if (n.y() < 0 || n.y() >= m_board[n.x()].size()) continue;
            if (visited[n.x()][n.y()]) continue;

            const QString &cellType  = m_board[n.x()][n.y()].type;
            const QString &cellAgent = m_board[n.x()][n.y()].agentName;

            bool canMove = false;
            int moveCost = 1;

            if (agentType == "WaterWalking") {
                canMove = (cellType != "#");
                if (cellType == "~") moveCost = 2;
            }
            else if (agentType == "Grounded") {
                canMove = (cellType != "#" && cellType != "~");
            }
            else if (agentType == "Flying") {
                canMove = true;
            }
            else if (agentType == "Floating") {
                canMove = (cellType != "#");
            } else {
                canMove = (cellType != "#");
            }
            if (!cellAgent.isEmpty()) {
                if (isEnemyAgent(cellAgent)) {
                    if (canMove && (dist + moveCost) <= mobility) {
                        visited[n.x()][n.y()] = true;
                        m_highlightedCells.append(n);
                    }
                } else {
                    continue;
                }
            } else {
                if (canMove && (dist + moveCost) <= mobility) {
                    visited[n.x()][n.y()] = true;
                    q.push({n, dist + moveCost});
                }
            }
        } 
    } 
    if (m_hexGridWidget) m_hexGridWidget->update();
}
void gameboard::moveAgentOnBoard(const QPoint &start, const QPoint &end)
{
    if (!m_gameStarted) return;
    if (start == end) return;
    if (start.x() < 0 || end.x() < 0) return;
    GameCell &startCell = m_board[start.x()][start.y()];
    GameCell &endCell = m_board[end.x()][end.y()];
    QString agentName = startCell.agentName;
    if (agentName.isEmpty()) return;
    if (!endCell.agentName.isEmpty()) {
        QString enemyName = endCell.agentName;
        for (int i = 0; i < m_agents.size(); ++i) {
            Agent* a = m_agents[i];
            if (a && a->name() == enemyName) {
                m_agents.removeAt(i);
                a->deleteLater();
                break;
            }
        }
        if (m_player1Agents.contains(enemyName)) {
            m_placedPlayer1Agents.remove(enemyName);
        } else if (m_player2Agents.contains(enemyName)) {
            m_placedPlayer2Agents.remove(enemyName);
        }
        m_agentHP.remove(enemyName);
    }
    endCell.agentName = agentName;
    startCell.agentName.clear();
    if (m_hexGridWidget) m_hexGridWidget->update();
    switchTurn();
}
// return pointer to DraggableItem by name (or nullptr)
const DraggableItem* gameboard::findDraggableItem(const QString &name) const {
    for (const DraggableItem &it : m_draggableItems) {
        if (it.name == name) return &it;
    }
    return nullptr;
}
// convert odd-q offset (col,row) to cube coords and return hex distance
int gameboard::hexDistance(const QPoint &a, const QPoint &b) const {
    auto toCube = [](const QPoint &p) {
        int col = p.x();
        int row = p.y();
        // odd-q vertical layout conversion (columns offset)
        int x = col;
        int z = row - ( (col - (col & 1)) / 2 );
        int y = -x - z;
        return std::tuple<int,int,int>(x,y,z);
    };
    int ax, ay, az, bx, by, bz;
    std::tie(ax,ay,az) = toCube(a);
    std::tie(bx,by,bz) = toCube(b);
    return (std::abs(ax - bx) + std::abs(ay - by) + std::abs(az - bz)) / 2;
}
// neighbors for odd-q vertical layout (consistent with highlightMovesForAgent)
QVector<QPoint> gameboard::getHexNeighbors(int c, int r) const {
    QVector<QPoint> n;
    if (c % 2 == 0) {
        n = {
            QPoint(c,     r - 1),  // up
            QPoint(c + 1, r - 1),  // up-right
            QPoint(c + 1, r),      // down-right
            QPoint(c,     r + 1),  // down
            QPoint(c - 1, r),      // down-left
            QPoint(c - 1, r - 1)   // up-left
        };
    } else {
        n = {
            QPoint(c,     r - 1),  // up
            QPoint(c + 1, r),      // up-right
            QPoint(c + 1, r + 1),  // down-right
            QPoint(c,     r + 1),  // down
            QPoint(c - 1, r + 1),  // down-left
            QPoint(c - 1, r)       // up-left
        };
    }
    return n;
}
// checks if agent of `agentType` can stand on a cell with cellType 
bool gameboard::canStandOnCellForAgentType(const QString &agentType, const QString &cellType) const {
    if (cellType == "#") return false; // rock - no one stands on rock in our rules
    if (agentType == "Grounded") {
        if (cellType == "~") return false; // grounded can't stand on water
        return true;
    } else if (agentType == "WaterWalking") {
        // waterWalking can be on water and ground but not rocks
        return (cellType != "#");
    } else if (agentType == "Floating") {
        // floating can't stand on rocks but can be on water/ground
        return (cellType != "#");
    } else if (agentType == "Flying") {
        // flying can be anywhere (we still disallow rock if you prefer, but per original logic flying = true)
        return true;
    }
    return (cellType != "#");
}
// remove agent completely by name (board cell, m_agents list, placed sets, HP map)
void gameboard::removeAgentByName(const QString &name) {
    for (auto &column : m_board) {
        for (auto &cell : column) {
            if (cell.agentName == name) {
                if (m_player1Agents.contains(name)) {
                    m_placedPlayer1Agents.remove(name);
                } else if (m_player2Agents.contains(name)) {
                    m_placedPlayer2Agents.remove(name);
                }
                cell.agentName.clear();
                break;
            }
        }
    }
    for (int i = 0; i < m_agents.size(); ++i) {
        Agent* a = m_agents[i];
        if (a && a->name() == name) {
            m_agents.removeAt(i);
            a->deleteLater();
            break;
        }
    }
    m_agentHP.remove(name);
    if (m_hexGridWidget) m_hexGridWidget->update();
}
// return candidate cells around defender (distance <= mobility, empty, and allowed by type)
QVector<QPoint> gameboard::getRetreatCandidates(const QPoint &defenderPos, const QString &agentType, int mobility) const {
    QVector<QPoint> candidates;
    // scan whole board but filter by distance <= mobility and emptiness + terrain rules
    for (int c = 0; c < m_board.size(); ++c) {
        for (int r = 0; r < m_board[c].size(); ++r) {
            QPoint p(c,r);
            if (p == defenderPos) continue;
            if (!m_board[c][r].agentName.isEmpty()) continue; // must be empty
            int dist = hexDistance(defenderPos, p);
            if (dist <= mobility) {
                const QString &cellType = m_board[c][r].type;
                if (canStandOnCellForAgentType(agentType, cellType)) {
                    candidates.append(p);
                }
            }
        }
    }
    return candidates;
}

void gameboard::performAttack(const QPoint &attackerPos, const QPoint &defenderPos)
{
    if (!m_gameStarted) return;
    if (attackerPos.x() < 0 || defenderPos.x() < 0) return;

    GameCell &attCell = m_board[attackerPos.x()][attackerPos.y()];
    GameCell &defCell = m_board[defenderPos.x()][defenderPos.y()];
    QString attackerName = attCell.agentName;
    QString defenderName = defCell.agentName;
    if (attackerName.isEmpty() || defenderName.isEmpty()) return;

    const DraggableItem* attackerItem = findDraggableItem(attackerName);
    const DraggableItem* defenderItem = findDraggableItem(defenderName);
    if (!attackerItem || !defenderItem) return;

    // ensure target within attack range
    int dist = hexDistance(attackerPos, defenderPos);
    if (dist > attackerItem->range) {
        qDebug() << "Target out of attack range:" << dist << " > " << attackerItem->range;
        return;
    }

    // initialize HP if not present
    if (!m_agentHP.contains(attackerName)) m_agentHP[attackerName] = attackerItem->health;
    if (!m_agentHP.contains(defenderName)) m_agentHP[defenderName] = defenderItem->health;

    int &defHP = m_agentHP[defenderName];
    int &attHP = m_agentHP[attackerName];

    // Track original HP for message
    int defHP_before = defHP;
    int attHP_before = attHP;

    // 1) Attack
    int attDamage = attackerItem->power;
    defHP -= attDamage;
    qDebug() << attackerName << "attacks" << defenderName << "for" << attDamage << "-> defenderHP =" << defHP;

    bool defenderDied = false;
    if (defHP <= 0) {
        qDebug() << defenderName << "died.";
        removeAgentByName(defenderName);
        QString ownerName = m_player1Agents.contains(defenderName) ? m_player1Name : m_player2Name;
        QMessageBox deathBox;
        deathBox.setWindowTitle("Agent Died");
        deathBox.setText(QString("Player %1: Your monster %2 has died!")
                             .arg(ownerName)
                             .arg(defenderName));
        deathBox.setIcon(QMessageBox::Warning);
        deathBox.exec();

        defenderDied = true;
    }

    // 2) Counterattack
    int counter = 0;
    if (!defenderDied) {
        int defDamage = defenderItem->power;
        counter = defDamage / 2;
        attHP -= counter;
        qDebug() << defenderName << "counterattacks" << attackerName << "for" << counter << "-> attackerHP =" << attHP;

        if (attHP <= 0) {
            qDebug() << attackerName << "died by counterattack.";
            removeAgentByName(attackerName);

            QString ownerName = m_player1Agents.contains(attackerName) ? m_player1Name : m_player2Name;
            QMessageBox deathBox;
            deathBox.setWindowTitle("Agent Died");
            deathBox.setText(QString("Player %1: Your monster %2 has died!")
                                 .arg(ownerName)
                                 .arg(attackerName));
            deathBox.setIcon(QMessageBox::Warning);
            deathBox.exec();
            if (m_hexGridWidget) m_hexGridWidget->update();
            switchTurn();
        }
    }

    if (!defenderDied && attHP > 0) {
        QVector<QPoint> neighbors = getHexNeighbors(defenderPos.x(), defenderPos.y());
        QVector<QPoint> validRetreat;

        for (const QPoint &p : neighbors) {
            // bounds check
            if (p.x() < 0 || p.x() >= m_board.size()) continue;
            if (p.y() < 0 || p.y() >= m_board[p.x()].size()) continue;

            // can't retreat into defender's cell (it's occupied)
            if (p == defenderPos) continue;

            const GameCell &cand = m_board[p.x()][p.y()];

            // must be empty
            if (!cand.agentName.isEmpty()) continue;

            // can't stand on rock
            if (cand.type == "#") continue;

            if (!canStandOnCellForAgentType(attackerItem->type, cand.type)) continue;

            // ADDITIONAL RULE:
            // disallow if any neighbor of candidate cell has a rock OR an agent,
            // EXCEPT the defender's cell and the attacker's original cell
            bool blockedByNearby = false;
            QVector<QPoint> around = getHexNeighbors(p.x(), p.y());
            for (const QPoint &adj : around) {
                if (adj.x() < 0 || adj.x() >= m_board.size()) continue;
                if (adj.y() < 0 || adj.y() >= m_board[adj.x()].size()) continue;

                // ignore defender and attacker's original position
                if (adj == defenderPos || adj == attackerPos) continue;

                const GameCell &adjCell = m_board[adj.x()][adj.y()];
                if (adjCell.type == "#") { blockedByNearby = true; break; }
                if (!adjCell.agentName.isEmpty()) { blockedByNearby = true; break; }
            }
            if (blockedByNearby) continue;

            // if reached, candidate is valid
            validRetreat.append(p);
        }

        if (!validRetreat.isEmpty()) {
            int idx = QRandomGenerator::global()->bounded(validRetreat.size());
            QPoint newPos = validRetreat[idx];

            // move attacker
            m_board[newPos.x()][newPos.y()].agentName = attackerName;
            attCell.agentName.clear();
            qDebug() << attackerName << "retreated to" << newPos;
        } else {
            qDebug() << "No valid immediate retreat cells for" << attackerName << "— stays in place.";
        }
    }


    // Flash effect 
    m_attackFlashCell = defenderPos;
    m_flashActive = true;
    QTimer::singleShot(500, this, [this]() {
        m_flashActive = false;
        update();
    });

    //  Battle message 
    QString msg;
    msg += QString("⚔️ Battle between %1 and %2\n\n")
               .arg(attackerName)
               .arg(defenderName);

    msg += QString("📉 %1 lost %2 Health (Health: %3)\n")
               .arg(attackerName)
               .arg(attHP_before - attHP)
               .arg(std::max(0, attHP));

    msg += QString("📉 %1 lost %2 Health (Health: %3)\n")
               .arg(defenderName)
               .arg(defHP_before - defHP)
               .arg(std::max(0, defHP));

    msg += QString("\n💪 %1 Power: %2 | 🛡 Defense: %3 | 🎯 Range: %4")
               .arg(attackerName)
               .arg(attackerItem->power)
               .arg(attackerItem->defense)
               .arg(attackerItem->range);

    msg += QString("\n💪 %1 Power: %2 | 🛡 Defense: %3 | 🎯 Range: %4")
               .arg(defenderName)
               .arg(defenderItem->power)
               .arg(defenderItem->defense)
               .arg(defenderItem->range);

    QMessageBox battleBox;
    battleBox.setWindowTitle("Battle Result");
    battleBox.setText(msg);
    battleBox.setIcon(QMessageBox::Information);
    battleBox.exec();

    checkForWinner();
    // update UI & end turn
    if (m_hexGridWidget) m_hexGridWidget->update();
    switchTurn();
}

void gameboard::checkForWinner() {
    // إذا اللاعب الأول ما عنده أي وحوش
    if (m_placedPlayer1Agents.isEmpty()) {
        QString winner = m_player2Name;
        QString loser  = m_player1Name;
        QMessageBox::information(this, "Game Over",
                                 QString("🎉 Congratulations %1! You win!\n😢 Sorry %2, you lost.")
                                     .arg(winner).arg(loser));
        return;
    }

    // إذا اللاعب الثاني ما عنده أي وحوش
    if (m_placedPlayer2Agents.isEmpty()) {
        QString winner = m_player1Name;
        QString loser  = m_player2Name;
        QMessageBox::information(this, "Game Over",
                                 QString("🎉 Congratulations %1! You win!\n😢 Sorry %2, you lost.")
                                     .arg(winner).arg(loser));
        return;
    }
}
