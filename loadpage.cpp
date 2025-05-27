#include "loadpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QDebug>
#include <QScrollArea>
#include "playersname.h"

loadpage::loadpage(const QString& player1Name, const QString& player2Name,
                   const QString& gridFile, playersname* playersNamePage, QWidget *parent)
    : QMainWindow(parent),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_gridFile(gridFile),
    m_playersNamePage(playersNamePage)
{
    setFixedSize(800, 600);
    setupUI();
    loadGridData();
    displayHexagonalGrid();
}

void loadpage::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Background
    backgroundLabel = new QLabel(centralWidget);
    QPixmap bgImage(":/new/backmonsters.jpg");
    if(bgImage.isNull()) {
        backgroundLabel->setStyleSheet("background-color: #222;");
    } else {
        bgImage = bgImage.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        backgroundLabel->setPixmap(bgImage);
    }
    backgroundLabel->setGeometry(0, 0, width(), height());

    // Back button
    QPushButton *backButton = new QPushButton("Back", centralWidget);
    backButton->setFixedSize(100, 40);
    backButton->move(30, 30);
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
        "}"
        );

    connect(backButton, &QPushButton::clicked, this, [this]() {
        this->hide();
        if (m_playersNamePage) {
            m_playersNamePage->show();
        }
    });

    // Player labels
    QLabel *p1Label = new QLabel("Player 1: " + m_player1Name, centralWidget);
    QLabel *p2Label = new QLabel("Player 2: " + m_player2Name, centralWidget);
    p1Label->setStyleSheet("font-size: 18px; color: #FF5555; font-weight: bold;");  // Red for player 1
    p2Label->setStyleSheet("font-size: 18px; color: #5555FF; font-weight: bold;");  // Blue for player 2

    // Create a scroll area for the grid
    QScrollArea *scrollArea = new QScrollArea(centralWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    // Container for the grid
    QWidget *gridContainer = new QWidget();
    gridContainer->setStyleSheet("background: transparent;");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(p1Label, 0, Qt::AlignCenter);
    mainLayout->addWidget(p2Label, 0, Qt::AlignCenter);
    mainLayout->addWidget(scrollArea);
    mainLayout->addStretch();

    scrollArea->setWidget(gridContainer);
    backgroundLabel->lower();
}

void loadpage::loadGridData()
{
    QFile file(m_gridFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open grid file:" << m_gridFile;
        return;
    }
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList cells;
        int start = line.indexOf('/');
        while (start != -1) {
            int end = line.indexOf('\\', start + 1);
            if (end == -1) break;
            QString cell = line.mid(start + 1, end - start - 1);
            cells.append(cell);
            start = line.indexOf('/', end + 1);
        }
        if (!cells.isEmpty()) {
            m_gridData.append(QVector<QString>(cells.begin(), cells.end()));
        }
    }
    file.close();
}

QPixmap loadpage::createHexagonPixmap(const QString& content, const QColor& color)
{
    int size = 60;
    QPixmap pixmap(size * 2, size * 2);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPolygonF hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = 2 * M_PI / 6 * i;
        hexagon << QPointF(size + size * cos(angle), size + size * sin(angle));
    }

    painter.setBrush(color);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawPolygon(hexagon);

    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, content);

    return pixmap;
}

void loadpage::displayHexagonalGrid()
{
    QWidget *gridContainer = qobject_cast<QScrollArea*>(centralWidget()->layout()->itemAt(2)->widget())->widget();
    QGridLayout *gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(20, 20, 20, 20);

    QColor player1Color(255, 85, 85, 200);    // Red for player 1
    QColor player2Color(85, 85, 255, 200);    // Blue for player 2
    QColor obstacleColor(100, 100, 100, 200);  // Gray for obstacles
    QColor emptyColor(50, 50, 50, 200);       // Dark gray for empty

    for (int row = 0; row < m_gridData.size(); ++row) {
        for (int col = 0; col < m_gridData[row].size(); ++col) {
            QString cellContent = m_gridData[row][col].trimmed();
            QColor cellColor;

            if (cellContent == "1") {
                cellColor = player1Color;
                cellContent = "P1";
            } else if (cellContent == "2") {
                cellColor = player2Color;
                cellContent = "P2";
            } else if (cellContent == "#") {
                cellColor = obstacleColor;
                cellContent = "#";
            } else if (cellContent == "~") {
                cellColor = QColor(100, 200, 255, 200);  // Light blue for water
                cellContent = "~";
            } else {
                cellColor = emptyColor;
                cellContent = "";
            }

            QLabel *hexLabel = new QLabel(gridContainer);
            hexLabel->setPixmap(createHexagonPixmap(cellContent, cellColor));
            hexLabel->setAlignment(Qt::AlignCenter);

            // Offset every other row for hexagonal grid effect
            int layoutCol = col * 2 + (row % 2);
            gridLayout->addWidget(hexLabel, row, layoutCol, Qt::AlignCenter);
        }
    }

    gridContainer->setLayout(gridLayout);
}
