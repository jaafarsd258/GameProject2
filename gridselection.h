#ifndef GRIDSELECTION_H
#define GRIDSELECTION_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>

class playersname;  // Forward declaration

class gridselection : public QMainWindow
{
    Q_OBJECT
public:
    explicit gridselection(const QString& player1Name, const QString& player2Name,
                           playersname* playersNamePage, QWidget *parent = nullptr);

private slots:
    void onGridSelected(int gridNumber);

private:
    QString m_player1Name;
    QString m_player2Name;
    QLabel *backgroundLabel;
    playersname* m_playersNamePage;
    void setupUI();
    QString getGridStyle(int gridNumber);
};

#endif // GRIDSELECTION_H
