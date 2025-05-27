#ifndef GALLERY_H
#define GALLERY_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

class opening;  // Forward declaration

class gallery : public QMainWindow
{
    Q_OBJECT
public:
    explicit gallery(opening* openingPage = nullptr, QWidget *parent = nullptr);

private:
    QLabel *backgroundLabel;
    opening* m_openingPage;
};

#endif // GALLERY_H
