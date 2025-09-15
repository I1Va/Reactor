
#include "canvas.hpp"


int main(int argc, char** argv){
    QApplication app(argc, argv);


    QMainWindow window;
    QWidget *central = new QWidget;
    window.setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);

    TexturedFrame *frame = new TexturedFrame(QStringLiteral("images/reactor_borders.jpeg"), /*border=*/300   );

    Canvas *canvas = new Canvas(QStringLiteral("images/reactor_inside.jpeg"));
    frame->layout()->addWidget(canvas);

    mainLayout->addWidget(frame);

   
    
    window.resize(800, 600);
    window.show();
    return app.exec();
}

