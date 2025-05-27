#include "gridselection.h"
#include "gameboard.h"
#include "playersname.h"
#include "loadpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QFile>
#include <QMessageBox>

gridselection::gridselection(const QString& player1Name, const QString& player2Name,
                             playersname* playersNamePage, QWidget *parent)
    : QMainWindow(parent),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_playersNamePage(playersNamePage)
{
    setFixedSize(800, 600);
    setupUI();
}

void gridselection::setupUI()
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

    // Title
    QLabel *titleLabel = new QLabel("Select a Grid", centralWidget);
    titleLabel->setStyleSheet("font-size: 24px; color: white;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setGeometry(0, 50, width(), 40);

    // Grid selection buttons
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(50, 120, 50, 50);

    for (int i = 1; i <= 8; i++) {
        QPushButton *gridButton = new QPushButton(QString("Grid %1").arg(i), centralWidget);
        gridButton->setFixedSize(120, 60);
        gridButton->setProperty("gridNumber", i);
        gridButton->setStyleSheet(
            "QPushButton {"
            "   border: 2px solid white;"
            "   background-color: rgba(20, 41, 61, 200);"
            "   color: white;"
            "   font: bold 12pt 'Arial Rounded MT';"
            "   border-radius: 5px;"
            "   padding: 5px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(134, 101, 44, 200);"
            "   border: 2px solid #FFD700;"
            "}"
            );

        connect(gridButton, &QPushButton::clicked, this, [this, i]() {
            onGridSelected(i);
        });

        gridLayout->addWidget(gridButton, (i-1)/4, (i-1)%4);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    backgroundLabel->lower();
}
QString gridselection::getGridStyle(int gridNumber) {
    // Define different styles for different grids
    switch(gridNumber) {
    case 1: return "Forest";
    case 2: return "Desert";
    case 3: return "Winter";
    case 4: return "Lava";
    case 5: return "Water";
    case 6: return "Mountain";
    case 7: return "Swamp";
    case 8: return "Castle";
    default: return "Default";
    }
}

void gridselection::onGridSelected(int gridNumber)
{
    QString gridFile = QString(":/grids/grid%1.txt").arg(gridNumber);
    QString gridStyle = getGridStyle(gridNumber);

    QFile file(gridFile);
    if (!file.exists()) {
        QMessageBox::warning(this, "Error", "Grid file not found: " + gridFile);
        return;
    }

    gameboard *board = new gameboard(gridFile, gridStyle, m_player1Name, m_player2Name, this);
    board->show();
    this->hide();
}
