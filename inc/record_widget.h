#ifndef RECORD_WIDGET_H
#define RECORD_WIDGET_H

#include <QWidget>
#include <QTimer>

#include "qcustomplot.h"


class RecorderWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecorderWidget(QWidget *parent = nullptr);

private slots:
    void updatePlot();

private:
    QCustomPlot *plot;
    QTimer *timer;
    QVector<double> xData, yData;
    double t;          // simulation time
    int maxPoints;
};

#endif // RECORD_WIDGET_H
