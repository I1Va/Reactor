#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QTimer>

#include "canvas.hpp"

int main(int argc, char** argv){
    QApplication app(argc, argv);

    QMainWindow window;
    QWidget *central = new QWidget;
    auto *layout = new QVBoxLayout(central);

    QLabel *counterLabel = new QLabel("0");
    QPushButton *btn = new QPushButton("Increment");
    Canvas *canvas = new Canvas; // custom painted widget

    layout->addWidget(counterLabel);
    layout->addWidget(btn);
    layout->addWidget(canvas);

    QObject::connect(btn, &QPushButton::clicked, [&](){
        bool ok;
        int v = counterLabel->text().toInt(&ok);
        counterLabel->setText(QString::number(v + 1));
        canvas->setAngle((canvas->angle() + 15) % 360);
        canvas->update(); // request repaint
    });

    window.setCentralWidget(central);
    window.resize(400, 400);
    window.show();
    return app.exec();
}

