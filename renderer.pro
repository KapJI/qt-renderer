QT += widgets gui core

CONFIG += console c++11
CONFIG -= app_bundle
DEFINES -= QT_NO_DEBUG_OUTPUT

DEPENDPATH += .
INCLUDEPATH += .

SOURCES += main.cpp mainwindow.cpp model.cpp mainwidget.cpp renderer.cpp
HEADERS += mainwindow.h geometry.h model.h mainwidget.h renderer.h