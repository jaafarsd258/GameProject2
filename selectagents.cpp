#include "selectagents.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QDebug>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QFrame>
#include <QGridLayout>


SelectAgents::SelectAgents(const QString& player1Name,
                           const QString& player2Name,
                           QWidget* parent)
    : QMainWindow(parent),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_agentsByType(Agent::getAgentsByType())
{
    setWindowTitle("Select Agents");
    resize(800, 600);
    // Initialize selected agents sets
    m_selectedAgents[1] = QSet<QString>();
    m_selectedAgents[2] = QSet<QString>();
    backgroundLabel = new QLabel(this);
    QPixmap bgImage(":/new/backmonsters5.jpg"); // Make sure this path is correct
    if (!bgImage.isNull()) {
        bgImage = bgImage.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
        backgroundLabel->setPixmap(bgImage);
        backgroundLabel->setScaledContents(true);
    } else {
        // Fallback if image not found
        backgroundLabel->setStyleSheet("background-color: #222;");
        qWarning() << "Background image not found!";
    }
    backgroundLabel->setGeometry(0, 0, width(), height());
    setupAgentsData();
    setupUI();
    backgroundLabel->lower();
}

void SelectAgents::setupAgentsData()
{
    // WaterWalking Agents
    m_agents.append({"Billy", ":/image/billy.png", "WaterWalking", 320, 3, 90, 1});
    m_agents.append({"Reketon", ":/image/reketon.png", "WaterWalking", 320, 2, 80, 2});
    m_agents.append({"Angus", ":/image/angus.png", "WaterWalking", 400, 2, 100, 1});
    m_agents.append({"Duraham", ":/image/duraham.png", "WaterWalking", 320, 2, 100, 2});
    m_agents.append({"Colonel Baba", ":/image/colonel_baba.png", "WaterWalking", 400, 2, 100, 1});
    m_agents.append({"Medusa", ":/image/medusa.png", "WaterWalking", 320, 2, 90, 2});
    m_agents.append({"Bunka", ":/image/bunka.png", "WaterWalking", 320, 3, 100, 1});
    m_agents.append({"Sanka", ":/image/sanka.png", "WaterWalking", 320, 3, 100, 1});

    // Grounded Agents
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

    // Flying Agents
    m_agents.append({"Rambu", ":/image/rambu.png", "Flying", 320, 3, 120, 1});

    // Floating Agents
    m_agents.append({"Sabrina", ":/image/sabrina.png", "Floating", 320, 3, 100, 1});
    m_agents.append({"Death", ":/image/death.png", "Floating", 240, 3, 120, 2});
}


void SelectAgents::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Player 1 Layout: نام + Selected + Confirm
    QHBoxLayout* player1Layout = new QHBoxLayout();
    m_player1Label = new QLabel(m_player1Name, this);
    m_player1Label->setStyleSheet("font-weight: bold; font-size: 16px;");
    m_selected1Label = new QLabel("Selected: None", this);
    m_selected1Label->setStyleSheet("color: #FF5555; margin-left: 10px;");
    m_player1ConfirmButton = new QPushButton("Confirm Selection", this);
    m_player1ConfirmButton->setStyleSheet("background-color: #FF5555; color: white; margin-left: 10px;");
    player1Layout->addWidget(m_player1Label);
    player1Layout->addWidget(m_selected1Label);
    player1Layout->addWidget(m_player1ConfirmButton);
    player1Layout->addStretch();

    QHBoxLayout* player2Layout = new QHBoxLayout();
    m_player2Label = new QLabel(m_player2Name, this);
    m_player2Label->setStyleSheet("font-weight: bold; font-size: 16px;");
    m_selected2Label = new QLabel("Selected: None", this);
    m_selected2Label->setStyleSheet("color: #5555FF; margin-left: 10px;");
    m_player2ConfirmButton = new QPushButton("Confirm Selection", this);
    m_player2ConfirmButton->setStyleSheet("background-color: #5555FF; color: white; margin-left: 10px;");
    player2Layout->addWidget(m_player2Label);
    player2Layout->addWidget(m_selected2Label);
    player2Layout->addWidget(m_player2ConfirmButton);
    player2Layout->addStretch();

    mainLayout->addLayout(player1Layout);
    mainLayout->addLayout(player2Layout);

    // فاصله برای جدا کردن از بقیه‌ی صفحه
    mainLayout->addSpacing(20);

    // Status label
    m_statusLabel = new QLabel("Each player must select 4 agents and confirm their selection", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 10px 0;");
    mainLayout->addWidget(m_statusLabel);

    // Start Game button
    m_startButton = new QPushButton("Start Game", this);
    m_startButton->setFixedSize(200, 40);
    m_startButton->setStyleSheet(R"(
        QPushButton {
            background-color: green;
            color: white;
            font-weight: bold;
            border-radius: 5px;
        }
        QPushButton:disabled {
            background-color: gray;
            color: lightgray;
        }
    )");
    m_startButton->setEnabled(false);  // غیرفعال از ابتدا
    mainLayout->addWidget(m_startButton, 0, Qt::AlignCenter);

    // فضای خالی بین دکمه استارت و انتخاب ایجنت‌ها
    mainLayout->addSpacing(20);

    // منطقه انتخاب ایجنت‌ها
    createAgentSelectionUI();

    connect(m_startButton, &QPushButton::clicked, this, &SelectAgents::onStartGameClicked);
    connect(m_player1ConfirmButton, &QPushButton::clicked, this, &SelectAgents::onPlayer1ConfirmClicked);
    connect(m_player2ConfirmButton, &QPushButton::clicked, this, &SelectAgents::onPlayer2ConfirmClicked);
}




void SelectAgents::createAgentSelectionUI()
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    QWidget* container = new QWidget();
    container->setStyleSheet(R"(
    QWidget {
        background-image: url(:/new/backmonsters5.jpg);
        background-repeat: no-repeat;
        background-position: center;
        background-attachment: fixed;
    }
)");


    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setAlignment(Qt::AlignTop);
    containerLayout->setSpacing(20);
    containerLayout->setContentsMargins(10, 10, 10, 10);

    // Create sections for each agent type
    QStringList agentTypes = {"WaterWalking", "Grounded", "Flying", "Floating"};

    for (const QString &type : agentTypes) {
        QLabel* typeLabel = new QLabel(type + " Agents", container);
        typeLabel->setStyleSheet("font-size: 18px; color: #FFD700; font-weight: bold; margin-top: 10px;");
        containerLayout->addWidget(typeLabel);

        QFrame* separator = new QFrame(container);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        separator->setStyleSheet("color: rgba(255, 255, 255, 100);");
        containerLayout->addWidget(separator);

        QWidget* agentsContainer = new QWidget(container);
        agentsContainer->setStyleSheet("background: transparent;");

        QGridLayout* gridLayout = new QGridLayout(agentsContainer);
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(15);
        gridLayout->setContentsMargins(0, 0, 0, 0);

        int row = 0;
        int col = 0;
        const int maxColumns = 3;
        for (const AgentInfo &agent : m_agents) {
            if (agent.type != type) continue;

            ClickableFrame* agentCard = new ClickableFrame(agentsContainer);
            agentCard->setFixedSize(200, 120);
            agentCard->setStyleSheet(
                "QFrame {"
                "   background-color: rgba(70, 70, 70, 180);"
                "   border-radius: 8px;"
                "   border: 2px solid #555;"
                "}"
                "QFrame:hover {"
                "   background-color: rgba(90, 90, 90, 200);"
                "   border: 2px solid #777;"
                "}");
            agentCard->setCursor(Qt::PointingHandCursor);

            QHBoxLayout* cardLayout = new QHBoxLayout(agentCard);
            cardLayout->setContentsMargins(8, 8, 8, 8);
            cardLayout->setSpacing(10);

            QLabel* imageLabel = new QLabel(agentCard);
            QPixmap pixmap(agent.imagePath);
            if (!pixmap.isNull()) {
                imageLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            imageLabel->setAlignment(Qt::AlignCenter);

            QWidget* infoWidget = new QWidget(agentCard);
            QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
            infoLayout->setSpacing(3);
            infoLayout->setContentsMargins(0, 0, 0, 0);

            // نام ایجنت
            QLabel* nameLabel = new QLabel(agent.name, infoWidget);
            nameLabel->setStyleSheet("font-weight: bold; color: white; font-size: 12px;");

            // نوع ایجنت
            QLabel* typeLabel = new QLabel("Type: " + agent.type, infoWidget);
            typeLabel->setStyleSheet("color: #55AAFF; font-size: 10px;");

            // سلامت
            QLabel* hpLabel = new QLabel(QString("HP: %1").arg(agent.hp), infoWidget);
            hpLabel->setStyleSheet("color: #FF5555; font-size: 10px;");

            // تحرک
            QLabel* mobilityLabel = new QLabel(QString("↗ Mobility: %1").arg(agent.mobility), infoWidget);
            mobilityLabel->setStyleSheet("color: #55FF55; font-size: 10px;");

            // آسیب
            QLabel* damageLabel = new QLabel(QString("Damage: %1").arg(agent.damage), infoWidget);
            damageLabel->setStyleSheet("color: #FFAA55; font-size: 10px;");

            // برد حمله
            QLabel* rangeLabel = new QLabel(QString("Range: %1").arg(agent.attackRange), infoWidget);
            rangeLabel->setStyleSheet("color: #AA55FF; font-size: 10px;");

            // اضافه کردن ویجت‌ها به لایه‌ها
            infoLayout->addWidget(nameLabel);
            infoLayout->addWidget(typeLabel);
            infoLayout->addWidget(hpLabel);
            infoLayout->addWidget(mobilityLabel);
            infoLayout->addWidget(damageLabel);
            infoLayout->addWidget(rangeLabel);

            cardLayout->addWidget(imageLabel);
            cardLayout->addWidget(infoWidget);

            // اتصال رویداد کلیک
            connect(agentCard, &ClickableFrame::clicked, [this, agent]() {
                int currentPlayer = m_player1Confirmed ? 2 : 1;
                Agent::Type type;
                if (agent.type == "WaterWalking") type = Agent::WaterWalking;
                else if (agent.type == "Grounded") type = Agent::Grounded;
                else if (agent.type == "Flying") type = Agent::Flying;
                else type = Agent::Floating;

                onAgentSelected(currentPlayer, agent.name, type);
            });

            gridLayout->addWidget(agentCard, row, col);
            col++;
            if (col >= maxColumns) {
                col = 0;
                row++;
            }
        }containerLayout->addWidget(agentsContainer);
    }
    scrollArea->setWidget(container);
    centralWidget()->layout()->addWidget(scrollArea);
}

void SelectAgents::onAgentSelected(int player, const QString& agentName, Agent::Type type) {
    // Check if player has already confirmed
    if ((player == 1 && m_player1Confirmed) || (player == 2 && m_player2Confirmed)) {
        qDebug() << "Player" << player << "has already confirmed their selection";
        return;
    }

    // Only allow 4 agents per player
    if (m_selectedAgents[player].size() >= 4) {
        qDebug() << "Player" << player << "has already selected 4 agents";
        return;
    }

    // Check for duplicate agents
    if (m_selectedAgents[player].contains(agentName)) {
        qDebug() << "Agent" << agentName << "already selected by player" << player;
        return;
    }

    m_selectedAgents[player].insert(agentName);
    updateSelectedAgentsDisplay();
    QString status = QString("Player %1 has selected %2/4 agents")
                         .arg(player)
                         .arg(m_selectedAgents[player].size());
    if (player == 1) {
        m_player1Label->setText("Player 1: " + m_player1Name + " (" +
                                QString::number(m_selectedAgents[1].size()) + "/4)");
    } else {
        m_player2Label->setText("Player 2: " + m_player2Name + " (" +
                                QString::number(m_selectedAgents[2].size()) + "/4)");
    }
}

void SelectAgents::updateSelectedAgentsDisplay() {
    QStringList p1Agents(m_selectedAgents[1].begin(), m_selectedAgents[1].end());
    QStringList p2Agents(m_selectedAgents[2].begin(), m_selectedAgents[2].end());

    m_selected1Label->setText("Selected: " + (p1Agents.isEmpty() ? "None" : p1Agents.join(", ")));
    m_selected2Label->setText("Selected: " + (p2Agents.isEmpty() ? "None" : p2Agents.join(", ")));
}

void SelectAgents::onPlayer1ConfirmClicked() {
    qDebug() << "Player 1 Confirm clicked!";
    if (m_selectedAgents[1].size() != 4) {
        QMessageBox::warning(this, "Error", "Player 1 must select exactly 4 agents!");
        return;
    }
    m_player1Confirmed = true;
    m_player1ConfirmButton->setEnabled(false);
    updateStatus();
}

void SelectAgents::onPlayer2ConfirmClicked() {
    qDebug() << "Player 2 Confirm clicked!";
    if (m_selectedAgents[2].size() != 4) {
        QMessageBox::warning(this, "Error", "Player 2 must select exactly 4 agents!");
        return;
    }
    m_player2Confirmed = true;
    m_player2ConfirmButton->setEnabled(false);
    updateStatus();
}

void SelectAgents::updateStatus() {
    if (m_player1Confirmed && m_player2Confirmed) {
        m_statusLabel->setText("Both players confirmed! Click Start Game.");
        m_startButton->setEnabled(true);
    } else {
        m_statusLabel->setText(QString("Player 1: %1/4 | Player 2: %2/4")
                                   .arg(m_selectedAgents[1].size())
                                   .arg(m_selectedAgents[2].size()));
        m_startButton->setEnabled(false);
    }
}

void SelectAgents::onStartGameClicked() {
    // Verify both players have confirmed
    if (!m_player1Confirmed || !m_player2Confirmed) {
        qDebug() << "Both players must confirm before starting the game";
        return;
    }

    // Verify both players have selected 4 agents
    if (m_selectedAgents[1].size() != 4 || m_selectedAgents[2].size() != 4) {
        qDebug() << "Both players must select 4 agents before starting the game";
        return;
    }

    // Select a random grid file
    const QStringList& gridFiles = gameboard::getGridFiles();
    int randomIndex = QRandomGenerator::global()->bounded(gridFiles.size());
    QString gridFile = gridFiles.at(randomIndex);

    // Create and store the game board
    if (m_gameBoard) {
        m_gameBoard->close();
        delete m_gameBoard;
    }

    m_gameBoard = new gameboard(gridFile, "dark", m_player1Name, m_player2Name);

    // Transfer selected agents
    QStringList p1Agents(m_selectedAgents[1].begin(), m_selectedAgents[1].end());
    QStringList p2Agents(m_selectedAgents[2].begin(), m_selectedAgents[2].end());

    m_gameBoard->setPlayer1Agents(p1Agents);
    m_gameBoard->setPlayer2Agents(p2Agents);



    // Show the game board and hide the selection window
    m_gameBoard->show();
    this->hide();
}
