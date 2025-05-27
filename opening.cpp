#include "opening.h"
#include "mainpage.h"
#include "playersname.h"
#include "gallery.h"
#include <QPixmap>
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>
#include <QPushButton>

opening::opening(mainPage* mainPagePtr, QWidget *parent)
    : QMainWindow(parent), m_mainPage(mainPagePtr), m_playersName(nullptr),
    m_gallery(nullptr)
{
    setFixedSize(800, 600);
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    backgroundLabel = new QLabel(this);
    QPixmap bgImage(":/new/backmonsters.jpg");
    if(bgImage.isNull()) {
        qDebug() << "Failed to load backmonsters image!";
        backgroundLabel->setStyleSheet("background-color: black;");
    } else {
        bgImage = bgImage.scaled(this->size(),
                                 Qt::KeepAspectRatioByExpanding);
        backgroundLabel->setPixmap(bgImage);
    }
    backgroundLabel->setGeometry(0, 0, width(), height());

    const int leftMargin = 30;
    const int rightMargin = 30;
    const int topMargin = 30;
    const int buttonSpacing = 20;


    QPushButton *backButton = new QPushButton("Back", this);
    backButton->setFixedSize(100, 40);
    backButton->move(leftMargin, topMargin);

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
        m_mainPage->show();
    });

    const int buttonsTopMargin = 150;
    QPushButton *galleryButton = new QPushButton("Gallery", this);
    galleryButton->setFixedSize(180, 40);
    galleryButton->move(width() - galleryButton->width() - rightMargin, buttonsTopMargin);

    QPushButton *gameButton = new QPushButton("Start an Async PVP Game", this);
    gameButton->setFixedSize(250,40);
    gameButton->move(width() - gameButton->width() - rightMargin,
                      buttonsTopMargin  + galleryButton->height() + buttonSpacing);

    connect(gameButton, &QPushButton::clicked, this, &opening::onGameButtonClicked);

    connect(galleryButton, &QPushButton::clicked, this, &opening::onGalleryButtonClicked);

    QString buttonStyle =
        "QPushButton {"
        "   border: 2px solid white;"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(20, 41, 61, 255), stop:1 rgba(100, 150, 200, 255));"
        "   color: white;"
        "   font: bold 12pt 'Arial Rounded MT';"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(134, 101, 44, 255), stop:1 rgba(200, 150, 100, 255));"
        "   border: 2px solid #FFD700;"
        "   color: #FFD700;"
        "}"
        "QPushButton:pressed {"
        "   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(80, 60, 20, 255), stop:1 rgba(150, 100, 50, 255));"
        "}";

    galleryButton->setStyleSheet(buttonStyle);
    gameButton->setStyleSheet(buttonStyle);

}

void opening::onGameButtonClicked()
{
    if (!m_playersName) {
        m_playersName = new playersname(this);
    }
    m_playersName->show();
    this->hide();
}
void opening::onGalleryButtonClicked()
{
    if (!m_gallery) {
        m_gallery = new gallery(this);
    }
    m_gallery->show();
    this->hide();
}



