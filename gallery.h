#ifndef GALLERY_H
#define GALLERY_H

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QVector>

struct AgentInfo
{
    QString name;
    QString imagePath;
    QString type;
    int hp;
    int mobility;
    int damage;
    int attackRange;
};

class opening; 

class gallery : public QMainWindow
{
    Q_OBJECT
public:
    explicit gallery(opening *openingPage = nullptr, QWidget *parent = nullptr);

private:
    void setupAgentsData();
    void createAgentCards();

    QLabel *backgroundLabel;
    opening *m_openingPage;
    QVector<AgentInfo> m_agents;
    QScrollArea *m_scrollArea;

};

#endif // GALLERY_H
