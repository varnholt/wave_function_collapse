QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    src/config.cpp \
    src/main.cpp \
    src/tile.cpp \
    src/vector2d.cpp \
    src/wavefunctioncollapse.cpp \
    src/ui/mainwindow.cpp \
    src/ui/tiledtexturewidget.cpp \
    src/ui/tilegridwidget.cpp

HEADERS += \
    src/config.h \
    src/json.hpp \
    src/tile.h \
    src/vector2d.h \
    src/wavefunctioncollapse.h \
    src/ui/mainwindow.h \
    src/ui/tiledtexturewidget.h \
    src/ui/tilegridwidget.h

FORMS += \
    src/ui/mainwindow.ui

INCLUDEPATH += src src/ui
