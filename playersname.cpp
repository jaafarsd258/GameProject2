#include "playersname.h"
#include "loadpage.h"
#include "opening.h"
#include "gridselection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QFontDatabase>

playersname::playersname(opening* openingPage , QWidget *parent)
    : QMainWindow(parent), m_openingPage(openingPage)
{
    setFixedSize(800,600);
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    backgroundLabel = new QLabel(centralWidget);
    QPixmap bgImage(":/new/backmonsters4.jpg");
    if(bgImage.isNull()) {
        qDebug() << "Failed to load playersname background image!";
        backgroundLabel->setStyleSheet("background-color: #333;");
    } else {
        bgImage = bgImage.scaled(this->size(),
                                 Qt::KeepAspectRatioByExpanding);
                                 backgroundLabel->setPixmap(bgImage);
    }
    backgroundLabel->setGeometry(0, 0, width(), height());

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
        if (m_openingPage) {
            m_openingPage->show();
        }
    });


    int fontId = QFontDatabase::addApplicationFont(":/fonts/comic.ttf");
    QString fontFamily = fontId != -1 ? QFontDatabase::applicationFontFamilies(fontId).at(0) : "Comic Sans MS";

    QLabel *player1Label = new QLabel("Player 1", centralWidget);
    QLabel *player2Label = new QLabel("Player 2", centralWidget);

    QFont Font(fontFamily, 18, QFont::Bold);
    player1Label->setFont(Font);
    player2Label->setFont(Font);
    player1Label->setStyleSheet("color: #e78905;");
    player2Label->setStyleSheet("color: #e78905;");

    nameInput1 = new QLineEdit(centralWidget);
    nameInput2 = new QLineEdit(centralWidget);
    nameInput1->setPlaceholderText("Enter name");
    nameInput2->setPlaceholderText("Enter name");
    nameInput1->setFixedSize(250, 40);
    nameInput2->setFixedSize(250, 40);
    nameInput1->setStyleSheet("font-size: 16px; padding: 5px; background-color: rgba(219,194,150,255);");
    nameInput2->setStyleSheet("font-size: 16px; padding: 5px; background-color: rgba(219,194,150,255);");

    submitButton = new QPushButton("Start Game", centralWidget);
    submitButton->setFixedSize(200, 50);
    submitButton->setFont(Font);
    submitButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #9d4052;"
        "   color: white;"
        "   border: 2px solid #FFD700;"
        "   border-radius: 10px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #433755;"
        "   border: 2px solid white;"
        "}"
        );


    QHBoxLayout *player1Layout = new QHBoxLayout();
    player1Layout->addStretch();
    player1Layout->addWidget(player1Label);
    player1Layout->addWidget(nameInput1);
    player1Layout->addStretch();

    QHBoxLayout *player2Layout = new QHBoxLayout();
    player2Layout->addStretch();
    player2Layout->addWidget(player2Label);
    player2Layout->addWidget(nameInput2);
    player2Layout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(player1Layout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(player2Layout);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(submitButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    backgroundLabel->lower();
    connect(submitButton, &QPushButton::clicked, this, &playersname::onStartGameClicked);

}
void playersname::onStartGameClicked()
{
    QString player1Name = nameInput1->text().isEmpty() ? "Player 1" : nameInput1->text();
    QString player2Name = nameInput2->text().isEmpty() ? "Player 2" : nameInput2->text();

    gridselection *gridSelection = new gridselection(player1Name, player2Name, this);
    gridSelection->show();
    this->hide();
}
