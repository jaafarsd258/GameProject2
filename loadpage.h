#ifndef LOADPAGE_H
#define LOADPAGE_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QVector>

class playersname;  // Forward declaration

class loadpage : public QMainWindow
{
    Q_OBJECT
public:
    explicit loadpage(const QString& player1Name, const QString& player2Name,
                      const QString& gridFile, playersname* playersNamePage = nullptr,
                      QWidget *parent = nullptr);
private:
    QString m_player1Name;
    QString m_player2Name;
    QLabel *backgroundLabel;
    QString m_gridFile;
    playersname* m_playersNamePage;
    QVector<QVector<QString>> m_gridData;  // To store grid data

    void setupUI();
    void loadGridData();
    void displayHexagonalGrid();
    QPixmap createHexagonPixmap(const QString& content, const QColor& color);
};

#endif // LOADPAGE_H
