#include "mainwindow.h"
#include "mainwidget.h"

#define WIDTH 1000
#define HEIGHT 700

MainWindow::MainWindow() {
    this->setFixedSize(WIDTH, HEIGHT);
    this->setWindowTitle("Qt Renderer");
    MainWidget* mainWidget = new MainWidget(this);
    this->setCentralWidget(mainWidget);
}