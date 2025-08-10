#include "gallery.h"
#include <QDebug>
#include <QFrame>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include "opening.h"

gallery::gallery(opening *openingPage, QWidget *parent)
    : QMainWindow(parent)
    , m_openingPage(openingPage)
{
    setFixedSize(800, 600);
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);
    }

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    backgroundLabel = new QLabel(centralWidget);
    QPixmap bgImage(":/new/backmonsters4.jpg");
    if (bgImage.isNull()) {
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
        "}");

    connect(backButton, &QPushButton::clicked, this, [this]() {
        this->hide();
        if (m_openingPage) {
            m_openingPage->show();
        }
    });
    m_scrollArea = new QScrollArea(centralWidget);
    m_scrollArea->setGeometry(50, 100, 700, 450);
    m_scrollArea->setStyleSheet(
        "QScrollArea { border: none; background: transparent; }"
        "QScrollBar:vertical { width: 12px; background: rgba(50, 50, 50, 150); }"
        "QScrollBar::handle:vertical { background: rgba(100, 100, 100, 200); min-height: 20px; }");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");

    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollLayout->setSpacing(20);
    scrollLayout->setContentsMargins(10, 10, 10, 10);

    m_scrollArea->setWidget(scrollContent);
    m_scrollArea->setWidgetResizable(true);

    // Title label
    QLabel *titleLabel = new QLabel("Agents Gallery", scrollContent);
    titleLabel->setStyleSheet("font-size: 24px; color: white; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    scrollLayout->addWidget(titleLabel);

    setupAgentsData();
    createAgentCards();

    backgroundLabel->lower();
}

void gallery::setupAgentsData()
{
    // WaterWalking agents
    m_agents.append({"Billy", ":/image/billy.png", "WaterWalking", 320, 3, 90, 1});
    m_agents.append({"Reketon", ":/image/reketon.png", "WaterWalking", 320, 2, 80, 2});
    m_agents.append({"Angus", ":/image/angus.png", "WaterWalking", 400, 2, 100, 1});
    m_agents.append({"Duraham", ":/image/duraham.png", "WaterWalking", 320, 2, 100, 2});
    m_agents.append({"Colonel Baba", ":/image/colonel_baba.png", "WaterWalking", 400, 2, 100, 1});
    m_agents.append({"Medusa", ":/image/medusa.png", "WaterWalking", 320, 2, 90, 2});
    m_agents.append({"Bunka", ":/image/bunka.png", "WaterWalking", 320, 3, 100, 1});
    m_agents.append({"Sanka", ":/image/sanka.png", "WaterWalking", 320, 3, 100, 1});

    // Grounded agents
    m_agents.append({"Sir Lamorak", ":/image/sir_lamorak.png", "Grounded", 320, 3, 110, 1});
    m_agents.append({"Kabu", ":/image/kabu.png", "Grounded", 400, 2, 120, 1});
    m_agents.append({"Rajakal", ":/image/rajakal.png", "Grounded", 320, 2, 130, 1});
    m_agents.append({"Salih", ":/image/salih.png", "Grounded", 400, 2, 80, 1});
    m_agents.append({"Khan", ":/image/khan.png", "Grounded", 320, 2, 90, 1});
    m_agents.append({"Boi", ":/image/boi.png", "Grounded", 400, 2, 100, 1});
    m_agents.append({"Eloi", ":/image/eloi.png", "Grounded", 240, 2, 100, 2});
    m_agents.append({"Kanar", ":/image/kanar.png", "Grounded", 160, 2, 100, 2});
    m_agents.append({"Elsa", ":/image/elsa.png", "Grounded", 320, 2, 140, 2});
    m_agents.append({"Karissa", ":/image/karissa.png", "Grounded", 280, 2, 80, 2});
    m_agents.append({"Sir Philip", ":/image/sir_philip.png", "Grounded", 400, 2, 100, 1});
    m_agents.append({"Frost", ":/image/frost.png", "Grounded", 260, 2, 80, 2});
    m_agents.append({"Tusk", ":/image/tusk.png", "Grounded", 400, 2, 100, 1});

    // Flying agents
    m_agents.append({"Rambu", ":/image/rambu.png", "Flying", 320, 3, 120, 1});

    // Floating agents
    m_agents.append({"Sabrina", ":/image/sabrina.png", "Floating", 320, 3, 100, 1});
    m_agents.append({"Death", ":/image/death.png", "Floating", 240, 3, 120, 2});
}

void gallery::createAgentCards()
{
    QWidget *scrollContent = m_scrollArea->widget();
    QVBoxLayout *scrollLayout = qobject_cast<QVBoxLayout *>(scrollContent->layout());

    // Create sections for each agent type
    QStringList agentTypes = {"WaterWalking", "Grounded", "Flying", "Floating"};

    for (const QString &type : agentTypes) {
        QLabel *typeLabel = new QLabel(type + " Agents", scrollContent);
        typeLabel->setStyleSheet(
            "font-size: 18px; color: #FFD700; font-weight: bold; margin-top: 10px;");
        scrollLayout->addWidget(typeLabel);

        QFrame *separator = new QFrame(scrollContent);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        separator->setStyleSheet("color: rgba(255, 255, 255, 100);");
        scrollLayout->addWidget(separator);

        QWidget *agentsContainer = new QWidget(scrollContent);
        agentsContainer->setStyleSheet("background: transparent;");

        QGridLayout *gridLayout = new QGridLayout(agentsContainer);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);
        gridLayout->setContentsMargins(0, 0, 0, 0);

        int row = 0;
        int col = 0;
        const int maxColumns = 3;

        for (const AgentInfo &agent : m_agents) {
            if (agent.type != type)
                continue;

            QFrame *agentCard = new QFrame(agentsContainer);
            agentCard->setFixedSize(200, 120);
            agentCard->setStyleSheet("QFrame {"
                                     "   background-color: rgba(70, 70, 70, 180);"
                                     "   border-radius: 8px;"
                                     "   border: 2px solid #555;"
                                     "}"
                                     "QFrame:hover {"
                                     "   background-color: rgba(90, 90, 90, 200);"
                                     "   border: 2px solid #777;"
                                     "}");

            QHBoxLayout *cardLayout = new QHBoxLayout(agentCard);
            cardLayout->setContentsMargins(8, 8, 8, 8);
            cardLayout->setSpacing(10);

            // Agent image
            QLabel *imageLabel = new QLabel(agentCard);
            QPixmap pixmap(agent.imagePath);
            if (!pixmap.isNull()) {
                imageLabel->setPixmap(
                    pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            imageLabel->setAlignment(Qt::AlignCenter);

            // Agent info
            QWidget *infoWidget = new QWidget(agentCard);
            QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
            infoLayout->setSpacing(3);
            infoLayout->setContentsMargins(0, 0, 0, 0);

            QLabel *nameLabel = new QLabel(agent.name, infoWidget);
            nameLabel->setStyleSheet("font-weight: bold; color: white; font-size: 12px;");

            QLabel *typeLabel = new QLabel("Type: " + agent.type, infoWidget);
            typeLabel->setStyleSheet("color: #55AAFF; font-size: 10px;");

            QLabel *hpLabel = new QLabel(QString(" HP: %1").arg(agent.hp), infoWidget);
            hpLabel->setStyleSheet("color: #FF5555; font-size: 10px;");

            QLabel *mobilityLabel = new QLabel(QString("↗ Mobility: %1").arg(agent.mobility),
                                               infoWidget);
            mobilityLabel->setStyleSheet("color: #55FF55; font-size: 10px;");

            QLabel *damageLabel = new QLabel(QString(" Damage: %1").arg(agent.damage), infoWidget);
            damageLabel->setStyleSheet("color: #FFAA55; font-size: 10px;");

            QLabel *rangeLabel = new QLabel(QString(" Range: %1").arg(agent.attackRange),
                                            infoWidget);
            rangeLabel->setStyleSheet("color: #AA55FF; font-size: 10px;");

            infoLayout->addWidget(nameLabel);
            infoLayout->addWidget(typeLabel);
            infoLayout->addWidget(hpLabel);
            infoLayout->addWidget(mobilityLabel);
            infoLayout->addWidget(damageLabel);
            infoLayout->addWidget(rangeLabel);

            cardLayout->addWidget(imageLabel);
            cardLayout->addWidget(infoWidget);

            gridLayout->addWidget(agentCard, row, col);
            col++;
            if (col >= maxColumns) {
                col = 0;
                row++;
            }
        }

        scrollLayout->addWidget(agentsContainer);
    }
}

