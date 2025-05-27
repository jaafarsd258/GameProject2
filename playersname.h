#ifndef PLAYERSNAME_H
#define PLAYERSNAME_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class opening;  // Forward declaration

class playersname : public QMainWindow
{
    Q_OBJECT
public:
 explicit playersname(opening* openingPage = nullptr, QWidget *parent = nullptr);
private slots:
    void onStartGameClicked();

private:
    QLabel *backgroundLabel;
    QLineEdit *nameInput1;
    QLineEdit *nameInput2;
    QPushButton *submitButton;
    opening* m_openingPage;
};

#endif // PLAYERSNAME_H
