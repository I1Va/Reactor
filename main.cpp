#include "canvas.h"
#include "record_widget.h"

#include <QPushButton>

int main(int argc, char** argv){
    QApplication app(argc, argv);


    QMainWindow window;
    QWidget *central = new QWidget;
    window.setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);

    TexturedFrame *frame = new TexturedFrame(QStringLiteral("images/reactor_borders.jpeg"), /*border=*/50 );

    Canvas *canvas = new Canvas(QStringLiteral("images/reactor_inside.jpeg"), QStringLiteral("images/piston.jpeg"), frame);
    frame->layout()->addWidget(canvas);

    RecorderWidget *molecule_recorder = new RecorderWidget;
    mainLayout->addWidget(molecule_recorder, /*stretch=*/1);  // narrower

    RecorderWidget *system_energy_recorder = new RecorderWidget;
    mainLayout->addWidget(system_energy_recorder, /*stretch=*/1);  // narrower

    mainLayout->addWidget(frame, 1);

    QPushButton *btn1 = new QPushButton("Add circle molecule");
    QPushButton *btn2 = new QPushButton("Add square molecule");

    mainLayout->addWidget(btn1);
    mainLayout->addWidget(btn2);

    
    window.resize(800, 800);
    window.show();
    return app.exec();
}

