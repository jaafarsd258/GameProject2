#ifndef OPENING_H
#define OPENING_H

#include <QMainWindow>
#include <QLabel>

class mainPage;
class playersname;
class gallery;

class opening : public QMainWindow
{
    Q_OBJECT
public:
    explicit opening(mainPage* mainPagePtr, QWidget *parent = nullptr);

private slots:
    void onGameButtonClicked();
    void onGalleryButtonClicked();

private:
    QLabel *backgroundLabel;
    mainPage* m_mainPage;  // Pointer to main page
    playersname* m_playersName; // Pointer to playersname page
    gallery* m_gallery; // Pointer to gallery page
};

#endif // OPENING_H
