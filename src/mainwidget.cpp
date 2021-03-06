#include <QCoreApplication>
#include <QString>
#include <QChar>
#include <QPoint>
#include <QGridLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QPainter>
#include <QImage>
#include <QDebug>

#include "mainwidget.h"

MainWidget::MainWidget(MainWindow* parent): parent(parent) {
    QSignalMapper* mapper = new QSignalMapper(this);
    
    QPushButton* arrows[4];
    QChar arrows_ch[4] = {QChar(0x2190), QChar(0x2191), QChar(0x2192), QChar(0x2193)};
    QPoint* arrows_dir[4] = {new QPoint(-1, 0), new QPoint(0, -1), new QPoint(1, 0), new QPoint(0, 1)};
    QGridLayout* arrows_layout = new QGridLayout;
    
    for (int i = 0; i < 4; ++i) {
        arrows[i] = new QPushButton(arrows_ch[i], this);
        arrows[i]->setFixedSize(arrows[i]->height(), arrows[i]->height());
        arrows_layout->addWidget(arrows[i], 1 + arrows_dir[i]->y(), 1 + arrows_dir[i]->x(), 1, 1);

        mapper->setMapping(arrows[i], (QObject*)arrows_dir[i]);
        connect(arrows[i], SIGNAL(clicked()), mapper, SLOT(map()));
    }
    QLabel* light_label = new QLabel("L", this);
    light_label->setStyleSheet("QLabel { color: yellow; font: 22pt; }");
    arrows_layout->addWidget(light_label, 1, 1, 1, 1);

    layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->addLayout(arrows_layout);
    setLayout(layout);

    QVector<QString> models;
    QStringList args = QCoreApplication::arguments();
    if (args.size() == 1) {
        models.push_back("models/african_head/african_head.obj");
        models.push_back("models/african_head/african_head_eye_inner.obj");
    } else {
        for (int i = 1; i < args.size(); ++i) {
            models.push_back(args.at(i));
        }
    }
    renderer = new Renderer(models, parent->width(), parent->height(), this);
    connect(mapper, SIGNAL(mapped(QObject*)), renderer, SLOT(moveLight(QObject*)));
}

void MainWidget::paintEvent(QPaintEvent *event) {
    QImage image = renderer->genFrame();
    if (!image.isNull()) {
        QPainter painter(this);
        painter.drawImage(QPoint(0, 0), image.mirrored());
    }
}

void MainWidget::keyPress(QKeyEvent *event) const {
    int dx[] = {0, -1, 0, 1};
    int dy[] = {1, 0, -1, 0};
    char dirs[] = "wasd";
    int sides[] = {Qt::Key_Up, Qt::Key_Left, Qt::Key_Down, Qt::Key_Right};
    for (size_t i = 0; i < 4; ++i) {
        if (event->key() == sides[i]) {
            renderer->moveEye(QPoint(dx[i], dy[i]));
        }
        if (!event->text().isEmpty() && event->text()[0] == dirs[i]) {
            renderer->moveCenter(QPoint(dx[i], dy[i]));
        }
    }
}