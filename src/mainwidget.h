#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPaintEvent>

#include "mainwindow.h"
#include "renderer.h"

class MainWindow;

class MainWidget: public QWidget {
    Q_OBJECT
public:
    MainWidget(MainWindow* parent = 0);
    void keyPress(QKeyEvent *event) const;
protected:
    void paintEvent(QPaintEvent *event);
private:
    Renderer* renderer;
    MainWindow* parent;
    QVBoxLayout* layout;
};