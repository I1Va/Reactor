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

static const double DISTANCE_COLLISION_EPS = 0.01;
static const double DISTANCE_COLLISION_EPS2 = DISTANCE_COLLISION_EPS * DISTANCE_COLLISION_EPS;
static const double TIME_COLLISION_EPS = 0.01;

static const double SQRT_2 = 1.41421356237;
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
    virtual double getCollideCircleRadius() const { return 0; }

    gm_vector<unsigned char, 3> getColor() const { return color; }
    gm_vector<double, 2> getPosition() const { return position; }
    gm_vector<double, 2> getMoveVector() const { return moveVector; }

    void setPosition(const gm_vector<double, 2> &newPosition) {
        position = newPosition;
    }

    void setMoveVector(const gm_vector<double, 2> &newMoveVector) {
        moveVector = newMoveVector;
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
        Molecule(position, moveVector, mass, color)
    { 
        moleculeType = MoleculeTypes::CIRCLIT;
        radius = getRadius(); 
    }

    ShapeType getShapeType() const override { return ShapeType::CIRCLE; }
    double getSize() const override { return radius; }
    double getCollideCircleRadius() const override { return radius; }

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
        Molecule(position, moveVector, mass, color)
    { 
        moleculeType = MoleculeTypes::QUADRIT;
        length = getLength(); 
    }

    ShapeType getShapeType() const override { return ShapeType::SQUARE; }
    double getSize() const override { return length; }
    double getCollideCircleRadius() const override { return length / SQRT_2; }

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
        NONE_WALL = -1,

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
        walls[LOWER_WALL] = gm_line<double, 2>({0, cordSysHeight}, {1, 0});
        walls[RIGHT_WALL] = gm_line<double, 2>({cordSysWidth,  0}, {1, 0});
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

    double getMoleculeCollisionDelta(
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
    ) {
        Molecule *fstMoleculePTR = (*fstMoleculeIT).get();
        Molecule *sndMoleculePTR = (*sndMoleculeIT).get();

        double coliisionRadius = fstMoleculePTR->getCollideCircleRadius() + sndMoleculePTR->getCollideCircleRadius();
        double coliisionRadius2 = coliisionRadius * coliisionRadius;

    
        gm_vector<double, 2> V = fstMoleculePTR->getMoveVector() - sndMoleculePTR->getMoveVector();
        gm_vector<double, 2> P = fstMoleculePTR->getPosition() - sndMoleculePTR->getPosition();
        
    
        double t1 = 0, t2 = 0;
        int nRoots = 0;
        double aCoef = V.get_x() * V.get_x() + V.get_y() * V.get_y();
        double bCoef = 2 * (P.get_x() * V.get_x() + P.get_y() * V.get_y());
        double cCoef = P.get_x() * P.get_x() + P.get_y() * P.get_y() - coliisionRadius2;

        solveQuadratic(aCoef, bCoef, cCoef, &t1, &t2, &nRoots);

        if (nRoots != 2) return std::numeric_limits<double>::quiet_NaN();
        if (t1 < 0) return std::numeric_limits<double>::quiet_NaN();

        return t1;
    }

    double getMoleculeCollisionTimePoint(
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
    ) {
        return currentReactorCoreTime + getMoleculeCollisionDelta(fstMoleculeIT, sndMoleculeIT);
    }
    
    void updateClosestKeyEventInfo() {
        for (auto fstMoleculeIT = moleculesList.begin(); fstMoleculeIT != moleculesList.end(); fstMoleculeIT++) {
            double wallCollisionTimePoint = getWallCollisionTimePoint(fstMoleculeIT);

            if ((std::isnan(closestEventTimePoint) || wallCollisionTimePoint < closestEventTimePoint))
                closestEventTimePoint = wallCollisionTimePoint;

            for (auto sndMoleculeIT = std::next(fstMoleculeIT); sndMoleculeIT != moleculesList.end(); sndMoleculeIT++) {
                double moleculeCollisionTimePoint = getMoleculeCollisionTimePoint(fstMoleculeIT, sndMoleculeIT);

                if (std::isnan(closestEventTimePoint) || moleculeCollisionTimePoint < closestEventTimePoint)
                    closestEventTimePoint = moleculeCollisionTimePoint;
            }
        }
    }

    void launchMoleculeReaction(
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT 
    ) {
        moleculesList.erase(fstMoleculeIT);
        moleculesList.erase(sndMoleculeIT);

        std::cout << "launchMoleculeReaction!\n";
    }

    bool tryMoleculeCollision
    (
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT 
    ) {
        Molecule *fstMoleculePTR = (*fstMoleculeIT).get();
        Molecule *sndMoleculePTR = (*sndMoleculeIT).get();

        double distance2 = (fstMoleculePTR->getPosition() - sndMoleculePTR->getPosition()).get_len2();
        double collisionDistance = (fstMoleculePTR->getCollideCircleRadius() + sndMoleculePTR->getCollideCircleRadius());

        if (distance2 - collisionDistance * collisionDistance < DISTANCE_COLLISION_EPS2) {
            launchMoleculeReaction(fstMoleculeIT, sndMoleculeIT);
            return true;
        }

        return false;
    }


    double getWallCollisionTimePoint(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT, WallType *collidedWall=NULL) {
        Molecule *moleculePtr = (*moleculeIT).get();

        double closestWallCollisionDelta = std::numeric_limits<double>::quiet_NaN();
    
        for (size_t i = 0; i < WALLS_CNT; i++) {            
            gm_line<double, 2> moveRay(moleculePtr->getPosition(), moleculePtr->getMoveVector());
            gm_vector<double, 2> intersection = get_ray_line_intersection(moveRay, walls[i]);
            
            
            if (intersection.is_poison()) continue;


            gm_vector<double, 2> path = intersection - moleculePtr->getPosition();

            double curDelta = (std::sqrt(path.get_len2()) - moleculePtr->getCollideCircleRadius()) / moleculePtr->getMoveVector().get_len2();

            if (std::isnan(closestWallCollisionDelta) || curDelta < closestWallCollisionDelta) {
                closestWallCollisionDelta = curDelta;
                if (collidedWall) *collidedWall = (WallType) (i);
            }
        }
        return currentReactorCoreTime + closestWallCollisionDelta;
    } 

    void processWallCollision(Molecule *molecule, const WallType wallType) {
        assert(molecule);

        double x = molecule->getMoveVector().get_x();
        double y = molecule->getMoveVector().get_y();

        switch (wallType) {
            case UPPER_WALL:
                molecule->setMoveVector({x, -y}); break;
            case LEFT_WALL:
                molecule->setMoveVector({-x, y}); break;
            case LOWER_WALL:
                molecule->setMoveVector({x, -y}); break;
            case RIGHT_WALL:
                molecule->setMoveVector({-x, y}); break;
            default:
                assert(0 && "unknown wallType");
        }
    }

    bool tryWallCollision(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT) {
        Molecule *moleculePtr = (*moleculeIT).get();

        WallType wallType = NONE_WALL;

        if (std::fabs(getWallCollisionTimePoint(moleculeIT, &wallType) - currentReactorCoreTime) < TIME_COLLISION_EPS) {
            processWallCollision(moleculePtr, wallType);
            return true;
        }

        return false;
    }

    void processReactorCoreEvent() {
        for (auto fstMoleculeIT = moleculesList.begin(); fstMoleculeIT != moleculesList.end(); fstMoleculeIT++) {
            // if (tryWallCollision(fstMoleculeIT)) return;

            for (auto sndMoleculeIT = std::next(fstMoleculeIT); sndMoleculeIT != moleculesList.end(); sndMoleculeIT++) {
                // if (tryMoleculeCollision(fstMoleculeIT, sndMoleculeIT)) return;
            }
        }
    }

    
    


    




signals:
    void reactorCoreUpdated();

public slots:
    void reactorCoreUpdate(const double deltaSecs);

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