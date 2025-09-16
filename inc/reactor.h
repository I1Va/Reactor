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
#include <QTimer>
#include <QPushButton>

#include "reactorcore.h"

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
    static const int moleculeButtonStretchFactor = 1;

    QPixmap shellTexture;
    int borderSize;

    QSlider *pistonSlider;

    QPushButton *addCirclitButton;
    QPushButton *addQuadritButton;

    ReactorCanvas *reactorCanvas;
    ReactorCore   *reactorCore;


    
    
public:
    void addPistonSlider(QVBoxLayout *reactorLayout, const int sliderStretchFactor) {
        assert(reactorLayout);

        pistonSlider = new QSlider(Qt::Horizontal);

        reactorCanvas->setPistonPercentage(pistonSliderMinVal);
        pistonSlider->setRange(pistonSliderMinVal, pistonSliderMaxVal);      
        pistonSlider->setValue(reactorCanvas->getPistonPercentage());

        connect(reactorCanvas, &ReactorCanvas::pistonPercentageChanged,
                pistonSlider, &QSlider::setValue);
    
        connect(pistonSlider, &QSlider::valueChanged,
                reactorCanvas, &ReactorCanvas::setPistonPercentage);
        
        reactorLayout->addWidget(pistonSlider, sliderStretchFactor);
    }

    void addReactorCoreButtons(QVBoxLayout *reactorLayout, ReactorCore *reactorCore, const int buttonStretchFactor) {
        assert(reactorLayout);
        assert(reactorCore);

        addCirclitButton = new QPushButton("add Circlit", this);
        addQuadritButton = new QPushButton("add Quadrit", this);

        reactorLayout->addWidget(addCirclitButton, buttonStretchFactor);
        reactorLayout->addWidget(addQuadritButton, buttonStretchFactor);

        connect(addCirclitButton, &QPushButton::clicked, reactorCore, &ReactorCore::addCirclit);
        connect(addQuadritButton,  &QPushButton::clicked, reactorCore, &ReactorCore::addQuadrit);
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

        reactorCanvas = new ReactorCanvas(pistonTexturePath, coreTexturePath, this);
        reactorCore = new ReactorCore(reactorCanvas);

        reactorLayout->addWidget(reactorCanvas, reactorCanvasStretchFactor);
        addPistonSlider(reactorLayout, pistonSliderStretchFactor);
        addReactorCoreButtons(reactorLayout, reactorCore, moleculeButtonStretchFactor);
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