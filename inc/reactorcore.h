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

enum MoleculeTypes {
    NONE = -1,

    CIRCLIT = 0,
    QUADRIT = 1,    
};

static const double MS_IN_S = 1000;
static const double REACTOR_CORE_UPDATE_SECS = 0.016;
static const size_t MAX_CLASS_NAME_LEN = 10;

static const gm_vector<unsigned char, 3> CIRCLIT_COLOR(255, 0, 0);
static const gm_vector<unsigned char, 3> QUADRIT_COLOR(0, 0, 255);

static const gm_vector<double, 2> INITIAL_MOVEVECTOR(1, 1);
static const double INITIAL_MASS = 1;
static const double CIRCLIT_MIN_RADIUS = 1;
// static const double INITIAL_CIRCLIT_RADIUS = 1;
// static const double INITIAL_QUADRIT_LENGTH = 1;

enum ShapeType {
    NONE_SHAPE_TYPE,

    SQUARE,
    CIRCLE,
    
};


class Molecule {
    enum MoleculeTypes moleculeType;


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

public:
    virtual ShapeType getShapeType() const { return ShapeType::NONE_SHAPE_TYPE; };
    virtual double getSize() const { return 0; };
    gm_vector<unsigned char, 3> getColor() const { return color; }
    gm_vector<double, 2> getPosition() const { return position; }
    gm_vector<double, 2> getMoveVector() const { return moveVector; }

    void setPosition(const gm_vector<double, 2> &newPosition) {
        position = newPosition;
    }
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
};

class ReactorCore : public QObject {
    Q_OBJECT

    
    std::mt19937 randomGenerator;

    gm_vector<double, 2> coreCanvasPos;
    gm_vector<double, 2> coreCanvasSize;
    double               coreCordSystemScale;

    double cordSysWidth;
    double cordSysHeight;

    int circlitCnt;
    int quadritCnt;




    double currentReactorCoreTime;
    double closestEventTimePoint;
    std::list<std::unique_ptr<Molecule>> moleculesList;

    enum WallType {
        UPPER_WALL = 0,
        LEFT_WALL = 1,
        LOWER_WALL = 2,
        RIGHT_WALL = 3,
    };

    static const size_t WALLS_CNT = 4;
    gm_line<double, 2> walls[WALLS_CNT] = {};

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
        connect(timer, &QTimer::timeout, this, &ReactorCore::reactorCoreUpdateHandle);
        timer->start(REACTOR_CORE_UPDATE_SECS);

        setCoreRectangle(coreRectangle);

        circlitCnt = 0;
        quadritCnt = 0;
        currentReactorCoreTime = 0;
        closestEventTimePoint = std::numeric_limits<double>::quiet_NaN();

        
    }
    
    void addCirclit() {
        reactorCoreAddMolecule(/*moleculeType=*/CIRCLIT);
    }

    void addQuadrit() {
        reactorCoreAddMolecule(/*moleculeType=*/QUADRIT);
    }

    void setCoreRectangle(const QRect &coreRectangle) {
        coreCanvasPos = gm_vector<double, 2>(coreRectangle.topLeft().x(), coreRectangle.topLeft().y());
        coreCanvasSize = gm_vector<double, 2>(coreRectangle.width(), coreRectangle.height());


        cordSysWidth = coreCanvasSize.get_x() / coreCordSystemScale;
        cordSysHeight = coreCanvasSize.get_y() / coreCordSystemScale;
        
        walls[UPPER_WALL] = gm_line<double, 2>({0, 0}, {1, 0});
        walls[LEFT_WALL]  = gm_line<double, 2>({0, 0}, {1, 0});
        walls[LOWER_WALL] = gm_line<double, 2>({0, 0}, {1, 0});
        walls[RIGHT_WALL] = gm_line<double, 2>({0, 0}, {1, 0});
    }
    
    gm_vector<int, 2> convertMoleculeCords(const gm_vector<double, 2> &moleculeCords) const {
        return moleculeCords * coreCordSystemScale + coreCanvasPos;
    }

    double randRange(double start, double end) {
        return start + (end - start) * randomGenerator() / double(randomGenerator.max());
    }

    const std::list<std::unique_ptr<Molecule>> &getMoleculeList() const { return moleculesList; }


private:
    void reactorCoreAddMolecule(const enum MoleculeTypes moleculeType) {
        gm_vector<double, 2> moleculePosition(randRange(0, cordSysWidth), randRange(0, cordSysHeight));

        double randomAngle = randRange(0, 2 * std::numbers::pi);
        gm_vector<double, 2> moleculetMoveVector = INITIAL_MOVEVECTOR.rotate(randomAngle);

        switch (moleculeType) {
            case CIRCLIT:
                moleculesList.push_back(std::make_unique<Circlit>(moleculePosition, moleculetMoveVector, INITIAL_MASS));
                break;
            case QUADRIT:
                moleculesList.push_back(std::make_unique<Quadrit>(moleculePosition, moleculetMoveVector, INITIAL_MASS));
                break;
            default:
                assert(0 && "switch(moleculeType) default");
        }   
    }

    void updateMoleculePosition(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT, const double deltaSecs) {
        Molecule *moleculePtr = (*moleculeIT).get();
        
        gm_vector<double, 2> newPosition = 
            moleculePtr->getPosition() + 
            moleculePtr->getMoveVector() * deltaSecs;
        
        moleculePtr->setPosition(newPosition);
    }

    void stableProcessContinue(double deltaSecs) {
        for (auto moleculeIT = moleculesList.begin(); moleculeIT != moleculesList.end(); moleculeIT++) {
            updateMoleculePosition(moleculeIT, deltaSecs);
        }
        currentReactorCoreTime += deltaSecs;
    }

    double getWallCollisionTimePoint(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT) {
        Molecule *moleculePtr = (*moleculeIT).get();

        double closestWallCollisionDelta2 = std::numeric_limits<double>::quiet_NaN();
    
        for (size_t i = 0; i < WALLS_CNT; i++) {
            gm_line<double, 2> moveRay(moleculePtr->getPosition(), moleculePtr->getMoveVector());
            gm_vector<double, 2> intersection = get_ray_line_intersection(moveRay, walls[i]);

            if (intersection.is_poison()) continue;

            gm_vector<double, 2> path = intersection - moleculePtr->getPosition();
            double curDelta2 = path.get_len2() / moleculePtr->getMoveVector().get_len2();
        
            if (std::isnan(closestWallCollisionDelta2) || curDelta2 < closestWallCollisionDelta2)
                closestWallCollisionDelta2 = curDelta2;
        }
        
        return currentReactorCoreTime + std::sqrt(closestWallCollisionDelta2);
    } 


    // void updateClosestKeyEventInfo() {
    //     for (auto fstMoleculeIT = moleculesList.begin(); fstMoleculeIT != moleculesList.end(); fstMoleculeIT++) {
    //         double wallCollisionTimePoint = getWallCollisionTimePoint(fstMoleculeIT);

    //         if ((std::isnan(closestEventTimePoint) || wallCollisionTimePoint < closestEventTimePoint))
    //             closestEventTimePoint = wallCollisionTimePoint;

    //         for (auto sndMoleculeIT = std::next(fstMoleculeIT); sndMoleculeIT != moleculesList.end(); sndMoleculeIT++) {
    //             double moleculeCollisionTimePoint = currentReactorCoreTime + checkMoleculeCollision(fstMoleculeIT, sndMoleculeIT);

    //             if (std::isnan(closestEventTimePoint) || moleculeCollisionTimePoint < closestEventTimePoint)
    //             closestEventTimePoint = moleculeCollisionTimePoint;
                
    //         }
    //     }
    // }


    








    

private slots:
    void reactorCoreUpdateHandle();
};









// void reactionCriclitCirclit(std::unique_ptr<Molecule> moleculeAPtr, std::unique_ptr<Molecule> moleculeBPtr) {
    //     // weak ptr 
    //     std::unique_ptr<Circlit> CirclitAPtr = std::unique_ptr<Circlit>(static_cast<Circlit*>(moleculeAPtr.release()));
    //     std::unique_ptr<Circlit> CirclitBPtr = std::unique_ptr<Circlit>(static_cast<Circlit*>(moleculeBPtr.release()));


    //     int newMass = CirclitAPtr.get + other.mass;
    //     gm_vector<double, 2> newMoveVector = (moveVector * mass + other.moveVector * other.mass) * (1.0 / newMass);
        
    //     reactionResult.push_back(std::make_unique<Quadrit>(collideCenter, newMoveVector, newMass));        
    // }

    // void processMoleculeReaction(std::unique_ptr<Molecule> moleculeAPtr, std::unique_ptr<Molecule> moleculeBPtr) {
    //     //
    //     if (typeid(*moleculeAPtr) == typeid(Circlit) && typeid(*moleculeBPtr) == typeid(Circlit)) {

    //     }
    // }




















#endif // REACTORCORE_H