#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <QApplication>
#include <QMainWindow>
#include <QFrame>
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QSlider>


class ReactorCanvas : public QWidget {
    Q_OBJECT
    QPixmap reactorCoreTexture;
    QPixmap reactorPistonTexture;

    double pistonPercentage = 0;
public:
    explicit ReactorCanvas(const QString &pistonTexturePath, const QString &coreTexturePath, QWidget *parent = nullptr)
        : QWidget(parent), reactorCoreTexture(coreTexturePath), reactorPistonTexture(pistonTexturePath) {}

public: 
    double getPistonPercentage() const { return pistonPercentage; }
    void setPistonPercentage(const double value);

signals:
    void pistonPercentageChanged(double value);

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        double pistonWidth = geometry().width() * pistonPercentage / 100.0;
        double coreWidth = geometry().width() - pistonWidth;
        QRect pistonPos(QPoint(0,0), QSize(pistonWidth, geometry().height()));
        QRect corePos(QPoint(pistonWidth, 0), QSize(coreWidth, geometry().height()));

        p.drawPixmap(pistonPos, reactorPistonTexture);
        p.drawPixmap(corePos, reactorCoreTexture);
    }
};


class Reactor : public QFrame {
    Q_OBJECT

    static const int pistonSliderMinVal = 10;
    static const int pistonSliderMaxVal = 80;
    static const int pistonSliderStretchFactor = 1;
    static const int reactorCanvasStretchFactor = 4;

    QPixmap shellTexture;
    int borderSize;

    QSlider *pistonSlider; 

    ReactorCanvas *canvas;

    
    
public:
    void addPistonSliderWidget(QVBoxLayout *reactorLayout, const int stretchFactor) {
        assert(reactorLayout);

        pistonSlider = new QSlider(Qt::Horizontal);

        canvas->setPistonPercentage(pistonSliderMinVal);
        pistonSlider->setRange(pistonSliderMinVal, pistonSliderMaxVal);      
        pistonSlider->setValue(canvas->getPistonPercentage());

        connect(canvas, &ReactorCanvas::pistonPercentageChanged,
                pistonSlider, &QSlider::setValue);
    
        connect(pistonSlider, &QSlider::valueChanged,
                canvas, &ReactorCanvas::setPistonPercentage);
        

        reactorLayout->addWidget(pistonSlider, stretchFactor);
    }


    explicit Reactor
    (
        
        const QString &shellTexturePath, int borderSize,
        const QString &pistonTexturePath,
        const QString &coreTexturePath,
        QWidget *parent = nullptr
    )
        : QFrame(parent), 
        shellTexture(shellTexturePath), borderSize(borderSize)
    {        
        auto *reactorLayout = new QVBoxLayout(this);
        reactorLayout->setContentsMargins(borderSize, borderSize, borderSize, borderSize);

        canvas = new ReactorCanvas(pistonTexturePath, coreTexturePath, this);

        reactorLayout->addWidget(canvas, reactorCanvasStretchFactor);
        addPistonSliderWidget(reactorLayout, pistonSliderStretchFactor);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter reactorPainter(this);
        if (!shellTexture.isNull()) {
            reactorPainter.drawPixmap(rect(), shellTexture);
        } else {
            qWarning("ReactorShell Texture image not loaded!");
            reactorPainter.fillRect(rect(), palette().window());
        }

        QFrame::paintEvent(event);
    }
};


#endif // CANVAS_HPP