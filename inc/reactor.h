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

static const int pistonSliderMinVal = 10;
static const int pistonSliderMaxVal = 80;
static const int pistonSliderStretchFactor = 1;
static const int reactorCanvasStretchFactor = 4;
static const int moleculeButtonStretchFactor = 1;
static const double coreCordSystemScale = 10;


class ReactorCanvas : public QWidget {
    Q_OBJECT
    QPixmap reactorCoreTexture;
    QPixmap reactorPistonTexture;

    double pistonPercentage;

    QRect pistonRectangle;
    QRect coreRectangle;

    ReactorCore *reactorCore;

public:
    explicit ReactorCanvas
    (
        const QString &pistonTexturePath, 
        const QString &coreTexturePath, 
        QWidget *parent = nullptr
    ) : 
        QWidget(parent), 
        reactorCoreTexture(coreTexturePath), 
        reactorPistonTexture(pistonTexturePath)
    {
        reactorCore = new ReactorCore(coreRectangle, coreCordSystemScale, this);
        setPistonPercentage(pistonSliderMinVal);
    }

    ~ReactorCanvas() {
        delete reactorCore;
    }

public: 
    double getPistonPercentage() const { return pistonPercentage; }
    ReactorCore *getReactorCore() const { return reactorCore; }

    void setPistonPercentage(const double value);

signals:
    void pistonPercentageChanged(double value);

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.drawPixmap(pistonRectangle, reactorPistonTexture);
        painter.drawPixmap(coreRectangle, reactorCoreTexture);


        for (auto iterator = reactorCore->getMoleculeList().begin(); iterator != reactorCore->getMoleculeList().end(); ++iterator) {
            Molecule *molecule = iterator->get();

            ShapeType shapeType = molecule->getShapeType();
            double moleculeSize = molecule->getSize() * coreCordSystemScale;
            QColor moleculeColor = QColor(molecule->getColor().get_x(), molecule->getColor().get_y(), molecule->getColor().get_z());
            gm_vector<int, 2> moleculeCanvasPos = reactorCore->convertMoleculeCords(molecule->getPosition());
            
            painter.setBrush(moleculeColor);
            painter.setPen(Qt::NoPen); 

            switch (shapeType) {
                case ShapeType::SQUARE:
                    std::cout << "draw SQUARE: " << moleculeCanvasPos.get_x() << " " << moleculeCanvasPos.get_y() << "\n";

                    painter.drawRect
                    (
                        moleculeCanvasPos.get_x() - moleculeSize / 2, 
                        moleculeCanvasPos.get_y() - moleculeSize / 2,
                        moleculeSize,
                        moleculeSize
                    );
        
                    break;
                case ShapeType::CIRCLE:
                    painter.drawEllipse(QPointF(moleculeCanvasPos.get_x(), moleculeCanvasPos.get_y()), moleculeSize, moleculeSize);
                    break;

                default:
                    qWarning() << QString("Can't paint molecule shape`%1`").arg(shapeType);
                    break;
            }
        }
    }
};

class Reactor : public QFrame {
    Q_OBJECT

    QPixmap shellTexture;
    int borderSize;

    QSlider *pistonSlider;

    QPushButton *addCirclitButton;
    QPushButton *addQuadritButton;

    ReactorCanvas *reactorCanvas;
    
    
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
        

        reactorLayout->addWidget(reactorCanvas, reactorCanvasStretchFactor);
        addPistonSlider(reactorLayout, pistonSliderStretchFactor);
        addReactorCoreButtons(reactorLayout, reactorCanvas->getReactorCore(), moleculeButtonStretchFactor);
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