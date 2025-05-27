#include "gallery.h"
#include "opening.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>

gallery::gallery(opening* openingPage, QWidget *parent)
    : QMainWindow(parent), m_openingPage(openingPage)
{
    setFixedSize(800, 600);
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
        backgroundLabel->setStyleSheet("background-color: #222;");
    } else {
        bgImage = bgImage.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
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



    backgroundLabel->lower();
}
