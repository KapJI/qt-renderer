QT += widgets gui core

CONFIG += console c++11
CONFIG -= app_bundle
DEFINES -= QT_NO_DEBUG_OUTPUT

DEPENDPATH += .
INCLUDEPATH += .

SOURCES += \
	src/main.cpp \
	src/mainwindow.cpp \
	src/mainwidget.cpp \
	src/model.cpp \
	src/image.cpp \
	src/simplegl.cpp \
	src/renderer.cpp 
HEADERS += \
	src/mainwindow.h \
	src/mainwidget.h \
	src/geometry.h \
	src/model.h \
	src/image.h \
	src/simplegl.h \
	src/renderer.h 

DESTDIR = .
PROJECT_DIR = $$_PRO_FILE_PWD_
BUILD_DIR = $${PROJECT_DIR}/build
PRECOMPILED_DIR = $${BUILD_DIR}
OBJECTS_DIR = $${BUILD_DIR}
MOC_DIR = $${BUILD_DIR}