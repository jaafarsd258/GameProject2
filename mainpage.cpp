#include "mainpage.h"
#include "opening.h"
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>

mainPage::mainPage(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(800, 600);
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    QLabel *background = new QLabel(this);
    QPixmap bgImage(":/new/background.jpg");
    if(bgImage.isNull()) {
        qDebug() << "Failed to load image!";
        background->setStyleSheet("background-color: black;");
    } else {
        bgImage = bgImage.scaled(this->size(), Qt::KeepAspectRatioByExpanding);

        background->setPixmap(bgImage);

        int xOffset = -70;
        background->setGeometry(xOffset, 0,
                                bgImage.width() + abs(xOffset),
                                height());
    }

    QPushButton *startButton = new QPushButton("START", this);
    startButton->setFixedSize(100, 50);
    startButton->move(
        (width() - startButton->width()) / 2,
        height() - startButton->height() - 20
        );

    startButton->setStyleSheet(
        "QPushButton {"
        "   border: 2px solid white;"
        "   background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:1, stop:0 rgba(20, 41, 61, 255), stop:1 rgba(255, 255, 255, 255));"
        "   color: grey;"
        "   font: 700 14pt 'Arial Rounded MT';"
        "   border-radius: 20px;"
        "   padding: 2px;"
        "}"
        "QPushButton:hover {"
        "   background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:1, stop:0 rgba(134, 101, 44, 255), stop:1 rgba(255, 255, 255, 255));"
        "   border: 2px solid white;"
        "}"
        );

    connect(startButton, &QPushButton::clicked, this, [this]() {
        opening *openingWindow = new opening(this);
        openingWindow->show();
        this->hide();
    });
}

mainPage::~mainPage() {}
