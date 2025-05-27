#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class opening;

class mainPage : public QMainWindow
{
    Q_OBJECT

public:
    mainPage(QWidget *parent = nullptr);
    ~mainPage();

private:
    QPushButton *startButton;
    QLabel *backgroundLabel;
};

#endif // MAINPAGE_H
