#include <QApplication>
#include <QDebug>

#include <iostream>

#include "mainwindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    // if (argc < 2) {
    //     std::cerr << "Usage: " << argv[0] << " filename\n";
    //     return -1;
    // }
    MainWindow window;
    window.show();
    return app.exec();
}