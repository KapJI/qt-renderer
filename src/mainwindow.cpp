#include "mainwindow.h"

#define WIDTH 1000
#define HEIGHT 700

MainWindow::MainWindow() {
    this->setFixedSize(WIDTH, HEIGHT);
    this->setWindowTitle("Qt Renderer");
    mainWidget = new MainWidget(this);
    this->setCentralWidget(mainWidget);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
	mainWidget->keyPress(event);
}