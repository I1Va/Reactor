#include "reactor.h"
// #include <QPainter>
// #include <QTimer>


void ReactorCanvas::setPistonPercentage(const double value) {
    assert(reactorCore);
    
    if (value == pistonPercentage)
        return;
    pistonPercentage = value;


    double pistonWidth = geometry().width() * pistonPercentage / 100.0;
    double coreWidth = geometry().width() - pistonWidth;

    pistonRectangle = QRect(QPoint(0,0), QSize(pistonWidth, geometry().height()));
    coreRectangle = QRect(QPoint(pistonWidth, 0), QSize(coreWidth, geometry().height()));

    reactorCore->updateCoreRectangle(coreRectangle);


    emit pistonPercentageChanged(value);
    update();
}