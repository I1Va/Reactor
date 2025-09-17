#ifndef REACTORCORE_H
#define REACTORCORE_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QTimer>

#include "gm_primitives.hpp"
#include <list>
#include <typeinfo>
#include <cstring>
#include <numbers>
#include <random>


static const int REACTOR_CORE_UPDATE_MS = 16;
static const size_t MAX_CLASS_NAME_LEN = 10;

static const gm_vector<unsigned char, 3> CIRCLIT_COLOR(255, 0, 0);
static const gm_vector<unsigned char, 3> QUADRIT_COLOR(0, 0, 255);

static const gm_vector<double, 2> INITIAL_MOVEVECTOR(1, 1);
static const double INITIAL_MASS = 1;
static const double CIRCLIT_MIN_RADIUS = 1;
// static const double INITIAL_CIRCLIT_RADIUS = 1;
// static const double INITIAL_QUADRIT_LENGTH = 1;

enum ShapeType {
    SQUARE,
    CIRCLE,
    NONE_SHAPE_TYPE
};


class Molecule {
    gm_vector<double, 2> position;
    gm_vector<double, 2> moveVector;
    int mass;

    gm_vector<unsigned char, 3> color;

    friend class Circlit;
    friend class Quadrit;
private:
    Molecule
    (
        const gm_vector<double, 2> &position, 
        const gm_vector<double, 2> &moveVector, 
        const int mass,
        const gm_vector<unsigned char, 3> &color
    ):
        position(position),
        moveVector(moveVector),
        mass(mass),
        color(color) {};

    virtual void moleculeReactionHandler(const Molecule &other, std::list<std::unique_ptr<Molecule>> &reactionResult) const {};
    void moleculeLaunchReaction(bool thisCirclitState, const Molecule &other, std::list<std::unique_ptr<Molecule>> &reactionResult) const;

public:
    virtual ShapeType getShapeType() const { return NONE_SHAPE_TYPE; };
    virtual double getSize() const { return 0; };

    gm_vector<unsigned char, 3> getColor() const { return color; }
    gm_vector<double, 2> getPosition() const { return position; }
};

class Circlit : public Molecule {
    double radius;

public:    
    Circlit
    (
        const gm_vector<double, 2> &position, 
        const gm_vector<double, 2> &moveVector, 
        const int mass, 
        gm_vector<unsigned char, 3> color=CIRCLIT_COLOR
    ): 
        Molecule(position, moveVector, mass, color) { radius = getRadius(); }

    ShapeType getShapeType() const override { return ShapeType::CIRCLE; }
    double getSize() const override { return radius; }

private:
    double getRadius() const { return mass; }; // temp formula: radius = mass

    void moleculeReactionHandler(const Molecule &other, std::list<std::unique_ptr<Molecule>> &reactionResult) const override {
        moleculeLaunchReaction(/*thisCirclitState=*/true, other, reactionResult);
    }
};

class Quadrit : public Molecule {
    double length;
public:
    Quadrit
    (
        const gm_vector<double, 2> &position, 
        const gm_vector<double, 2> &moveVector, 
        const int mass,  
        gm_vector<unsigned char, 3> color=QUADRIT_COLOR
    ): 
        Molecule(position, moveVector, mass, color) { length = getLength(); }

    ShapeType getShapeType() const override { return ShapeType::SQUARE; }
    double getSize() const override { return length; }

private:
    double getLength() const { return mass; }; // temp formula: length = mass

    void moleculeReactionHandler(const Molecule &other, std::list<std::unique_ptr<Molecule>> &reactionResult) const override {
        moleculeLaunchReaction(/*thisCirclitState=*/false, other, reactionResult);
    }
};

class ReactorCore : public QObject {
    Q_OBJECT

    std::mt19937 randomGenerator;

    gm_vector<double, 2> coreCanvasPos;
    gm_vector<double, 2> coreCanvasSize;
    double               coreCordSystemScale;

    double cordSysWidth;
    double cordSysHeight;

    std::list<std::unique_ptr<Molecule>> moleculeList;
    int circlitCnt;
    int quadritCnt;

public:
    explicit ReactorCore
    (
        const QRect &coreRectangle, const double coreCordSystemScale,
        QObject *parent = nullptr
    ) :
        QObject(parent), randomGenerator(std::random_device{}()),
        coreCordSystemScale(coreCordSystemScale)
    {
        auto *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ReactorCore::reactorCoreUpdate);
        timer->start(REACTOR_CORE_UPDATE_MS);

        setCoreRectangle(coreRectangle);
    }

    double randRange(double start, double end) {
        return start + (end - start) * randomGenerator() / double(randomGenerator.max());
    }


    const std::list<std::unique_ptr<Molecule>> &getMoleculeList() const { return moleculeList; }

    gm_vector<int, 2> convertMoleculeCords(const gm_vector<double, 2> &moleculeCords) const {
        return moleculeCords * coreCordSystemScale + coreCanvasPos;
    }

    void setCoreRectangle(const QRect &coreRectangle) {
        coreCanvasPos = gm_vector<double, 2>(coreRectangle.topLeft().x(), coreRectangle.topLeft().y());
        coreCanvasSize = gm_vector<double, 2>(coreRectangle.width(), coreRectangle.height());


        cordSysWidth = coreCanvasSize.get_x() / coreCordSystemScale;
        cordSysHeight = coreCanvasSize.get_y() / coreCordSystemScale;
    }

private:
    void addMolecule(const bool CirclitState);

private slots:
    void reactorCoreUpdate();

public slots:
    void addCirclit();
    void addQuadrit();
};


#endif // REACTORCORE_H