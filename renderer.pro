QT += widgets gui core

CONFIG += console c++11
CONFIG -= app_bundle
DEFINES -= QT_NO_DEBUG_OUTPUT

DEPENDPATH += .
INCLUDEPATH += .

SOURCES += src/main.cpp src/mainwindow.cpp src/model.cpp src/mainwidget.cpp src/renderer.cpp src/image.cpp
HEADERS += src/mainwindow.h src/geometry.h src/model.h src/mainwidget.h src/renderer.h src/image.h

DESTDIR = .
PROJECT_DIR = $$_PRO_FILE_PWD_
BUILD_DIR = $${PROJECT_DIR}/build
PRECOMPILED_DIR = $${BUILD_DIR}
OBJECTS_DIR = $${BUILD_DIR}
MOC_DIR = $${BUILD_DIR}