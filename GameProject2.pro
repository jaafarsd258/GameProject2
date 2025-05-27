QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    agent.cpp \
    gallery.cpp \
    gameboard.cpp \
    gridselection.cpp \
    loadpage.cpp \
    main.cpp \
    mainpage.cpp \
    opening.cpp \
    playersname.cpp


HEADERS += \
    agent.h \
    gallery.h \
    gameboard.h \
    gridselection.h \
    loadpage.h \
    mainpage.h \
    opening.h \
    playersname.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc\
    grids.qrc
