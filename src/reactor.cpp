#include "reactor.h"
// #include <QPainter>
// #include <QTimer>


void ReactorCanvas::setPistonPercentage(const double value) {
    if (value == pistonPercentage)
        return;
    pistonPercentage = value;
    emit pistonPercentageChanged(value);
    update();
}