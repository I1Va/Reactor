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

#include <algorithm>
#include "reactorcore.h"

static const int PISTON_SLIDER_MINVAL = 10;
static const int PISTON_SLIDER_MAXVAL = 80;
static const int PISTON_SLIDER_STRETCH_FACTOR = 1;
static const int REACTOR_CANVAS_STRETCH_FACTOR = 4;
static const int MOLECULE_BUTTONS_STRETCH_FACTOR = 1;
static const double CORE_CORD_SYSTEM_SCALE = 10;


class ReactorCanvas : public QWidget {
    Q_OBJECT
    
    friend class Reactor; 

    QPixmap reactorCoreTexture;
    QPixmap reactorPistonTexture;

    QRect pistonRectangle;
    QRect coreRectangle;

    const ReactorCore *reactorCore;

public:
    explicit ReactorCanvas
    (
        const QString &pistonTexturePath, 
        const QString &coreTexturePath,
        const QRect &pistonRectangle,
        const QRect &coreRectangle,
        const ReactorCore *reactorCore,
        QWidget *parent = nullptr
    ) : 
        reactorCoreTexture(coreTexturePath), 
        reactorPistonTexture(pistonTexturePath),
        reactorCore(reactorCore),
        QWidget(parent)
    {
        setInternalRectangles(pistonRectangle, coreRectangle);
    }

private:
    void setInternalRectangles(const QRect &pistonRectangle, const QRect &coreRectangle) {
        this->pistonRectangle = pistonRectangle;
        this->coreRectangle = coreRectangle;

        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.drawPixmap(pistonRectangle, reactorPistonTexture);
        painter.drawPixmap(coreRectangle, reactorCoreTexture);


        for (auto iterator = reactorCore->getMoleculeList().begin(); iterator != reactorCore->getMoleculeList().end(); ++iterator) {
            Molecule *molecule = iterator->get();

            ShapeType shapeType = molecule->getShapeType();
            double moleculeSize = molecule->getSize() * CORE_CORD_SYSTEM_SCALE;
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

    QSlider     *pistonSlider;
    QPushButton *addCirclitButton;
    QPushButton *addQuadritButton;

    int pistonPercentage;
    QRect pistonRectangle;
    QRect coreRectangle;

    ReactorCanvas *reactorCanvas;
    ReactorCore   *reactorCore;
    
    
public:
    void addPistonSlider(QVBoxLayout *reactorLayout, const int sliderStretchFactor) {
        assert(reactorLayout);

        pistonSlider = new QSlider(Qt::Horizontal);

        pistonSlider->setRange(PISTON_SLIDER_MINVAL, PISTON_SLIDER_MAXVAL);      
        pistonSlider->setValue(getPistonPercentage());

        connect(this, &Reactor::pistonPercentageChanged,
                pistonSlider, &QSlider::setValue);
    
        connect(pistonSlider, &QSlider::valueChanged,
                this, &Reactor::setPistonPercentage);
        
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

        pistonPercentage = PISTON_SLIDER_MINVAL;
        updateInternalRectanglesInfo();

        reactorCore = new ReactorCore(coreRectangle, CORE_CORD_SYSTEM_SCALE);
        reactorCanvas = new ReactorCanvas(pistonTexturePath, coreTexturePath, pistonRectangle, coreRectangle, reactorCore, this);
    
        reactorLayout->addWidget(reactorCanvas, REACTOR_CANVAS_STRETCH_FACTOR);
        addPistonSlider(reactorLayout, PISTON_SLIDER_STRETCH_FACTOR);
        addReactorCoreButtons(reactorLayout, reactorCore, MOLECULE_BUTTONS_STRETCH_FACTOR);
    }

    
signals:
    void pistonPercentageChanged(int value);

private:
    void setPistonPercentage(const int value);
    double getPistonPercentage() const { return pistonPercentage; }



    void updateInternalRectanglesInfo() {
        double pistonWidth = geometry().width() * pistonPercentage / 100.0;
        double coreWidth = geometry().width() - pistonWidth;

        pistonRectangle = QRect(QPoint(0,0), QSize(pistonWidth, geometry().height()));
        coreRectangle = QRect(QPoint(pistonWidth, 0), QSize(coreWidth, geometry().height()));

        update();
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