#include "reactor.h"
// #include <QPainter>
// #include <QTimer>

// Canvas::Canvas(QWidget *parent) : QWidget(parent), m_tex(":/texture.png") {
//     setMinimumHeight(200);
//     // optional: animate with timer
//     QTimer *t = new QTimer(this);
//     connect(t, &QTimer::timeout, this, [this](){ setAngle((m_angle+2)%360); update(); });
//     t->start(100);
// }

// void Canvas::paintEvent(QPaintEvent *) {
//     QPainter p(this);
//     p.setRenderHint(QPainter::Antialiasing);
//     // background
//     p.fillRect(rect(), Qt::white);

//     // draw circle at center
//     QPointF center(width()/2.0, height()/2.0);
//     qreal radius = qMin(width(), height())*0.25;
//     p.setPen(Qt::black);
//     p.setBrush(Qt::NoBrush);
//     p.drawEllipse(center, radius, radius);

//     // draw rotated texture inside circle
//     if (!m_tex.isNull()) {
//         p.save();
//         p.translate(center);
//         p.rotate(m_angle);
//         qreal scale = (radius*1.6) / qMax(m_tex.width(), m_tex.height());
//         p.scale(scale, scale);
//         p.drawPixmap(-m_tex.width()/2, -m_tex.height()/2, m_tex);
//         p.restore();
//     }
// }

void Reactor::reactorUpdate() {
    update();
}

void Reactor::addCirclitHandle() {
    reactorCore->addCirclit();
    update();
}

void Reactor::addQuadritHandle() {
    reactorCore->addQuadrit();
    update();
}

void Reactor::setPistonPercentage(const int value) {
    assert(reactorCore);
     
    if (value == pistonPercentage)
        return;
    pistonPercentage = value;


    updateInternalRectanglesInfo();

    reactorCanvas->setInternalRectangles(pistonRectangle, coreRectangle);
    reactorCore->setCoreRectangle(coreRectangle);

    emit pistonPercentageChanged(value);
}