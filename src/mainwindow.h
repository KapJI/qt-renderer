#pragma once

#include <QMainWindow>

#include "mainwidget.h"

class MainWidget;

class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    void keyPressEvent(QKeyEvent *event);
private:
	MainWidget* mainWidget;
};