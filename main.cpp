#include "reactor.h"
#include "record_widget.h"


#include <QObject>


// MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
//     auto *central = new QWidget(this);
//     auto *layout = new QVBoxLayout(central);

//     auto *canvas = new Canvas(this);
//     auto *slider = new QSlider(Qt::Horizontal, this);

//     slider->setRange(0, 100);          // min=0, max=100
//     slider->setValue(canvas->pistonPercentage());

//     // Connect slider to canvas
//     connect(slider, &QSlider::valueChanged,
//             canvas, &Canvas::setPistonPercentage);

//     // Optional: keep slider synced if canvas changes internally
//     connect(canvas, &Canvas::pistonPercentageChanged,
//             slider, &QSlider::setValue);

//     layout->addWidget(canvas);
//     layout->addWidget(slider);
//     setCentralWidget(central);
// }

const static double pistonSliderMinVal = 10.0;
const static double pistonSliderMaxVal = 80.0;

const QString reactorShellTexturePath("images/reactor_borders.jpeg");
const QString reactorPistonTexturePath("images/piston.jpeg");
const QString reactorCoreTexturePath("images/reactor_inside.jpeg");


int main(int argc, char** argv){
    QApplication app(argc, argv);

    QMainWindow window;

    QWidget *central = new QWidget;
    auto *mainLayout = new QVBoxLayout(central);



    window.setCentralWidget(central);

    

    Reactor *reactor = new Reactor
    (
        reactorShellTexturePath, /*borderSize=*/20,
        reactorPistonTexturePath,
        reactorCoreTexturePath
    );

    mainLayout->addWidget(reactor, 1);

    
   
    

    
    // RecorderWidget *molecule_recorder = new RecorderWidget;
    // mainLayout->addWidget(molecule_recorder, /*stretch=*/1);  // narrower

    // RecorderWidget *system_ene.rgy_recorder = new RecorderWidget;
    // mainLayout->addWidget(system_energy_recorder, /*stretch=*/1);  // narrower

    
    
    window.resize(800, 800);
    window.show();
    return app.exec();
}

