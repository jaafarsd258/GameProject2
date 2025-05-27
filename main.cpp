#include "mainpage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainPage w;
    w.show();
    return a.exec();
}
