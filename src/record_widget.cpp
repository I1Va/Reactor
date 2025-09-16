// #include <QVBoxLayout>
// #include <QtMath>

// #include "record_widget.h"


// RecorderWidget::RecorderWidget(QWidget *parent)
//     : QWidget(parent), t(0), maxPoints(200)
// {
    

//     QVBoxLayout *layout = new QVBoxLayout(this);

//     plot = new QCustomPlot(this);
//     plot->addGraph();
//     plot->xAxis->setLabel("Time (s)");
//     plot->yAxis->setLabel("Value");
//     plot->xAxis->setRange(0, 10);
//     plot->yAxis->setRange(-1, 1);
//     plot->setInteraction(QCP::iRangeDrag);
//     plot->setInteraction(QCP::iRangeZoom);
//     // plot->setMinimumSize(300, 200);

//     layout->addWidget(plot);

//     timer = new QTimer(this);
//     connect(timer, &QTimer::timeout, this, &RecorderWidget::updatePlot);
//     timer->start(50); // 20 Hz
// }

// void RecorderWidget::updatePlot()
// {
//     t += 0.05; // simulation step

//     double val = qSin(t); // example value (replace with your app data)

//     xData.append(t);
//     yData.append(val);

//     if (xData.size() > maxPoints) {
//         xData.remove(0, 1);
//         yData.remove(0, 1);
//     }

//     plot->graph(0)->setData(xData, yData);
//     plot->xAxis->setRange(t - 10, t); // last 10 seconds
//     plot->replot();
// }
