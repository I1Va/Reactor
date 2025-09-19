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

enum MoleculePhysicalStates {
    DEATH,
    UNRESPONSIVE,
    ALIVE,
};

static const double DISTANCE_COLLISION_EPS = 0.1;
static const double DISTANCE_COLLISION_EPS2 = DISTANCE_COLLISION_EPS * DISTANCE_COLLISION_EPS;
static const double TIME_COLLISION_EPS = 0.01;

static const double SQRT_2 = 1.41421356237;
static const double MS_IN_S = 1000;
static const double REACTOR_CORE_UPDATE_SECS = 0.016;
static const size_t MAX_CLASS_NAME_LEN = 10;

static const gm_vector<unsigned char, 3> CIRCLIT_COLOR(255, 0, 0);
static const gm_vector<unsigned char, 3> QUADRIT_COLOR(0, 0, 255);

static const double MoleculeMinInitSpeed = 1;
static const double MoleculeMaxInitSpeed = 3;

static const gm_vector<double, 2> INITIAL_speedVector(1, 1);
static const double INITIAL_MASS = 1;
static const double CIRCLIT_MIN_RADIUS = 1;
// static const double INITIAL_CIRCLIT_RADIUS = 1;
// static const double INITIAL_QUADRIT_LENGTH = 1;

enum ShapeType {
    NONE_SHAPE_TYPE,

    SQUARE,
    CIRCLE,
    
};


class Molecule;

typedef void (*moleculeReaction) (
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
);

void CirclitQuadritReaction(
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
);

const moleculeReaction CirclitCirclitReaction = CirclitQuadritReaction;

void QuadritQuadritReaction(
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
);

void launchMoleculeReaction (
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT 
);


const size_t moleculeReactionsVTableMaxSz = 100;
const moleculeReaction moleculeReactionsVTable[moleculeReactionsVTableMaxSz][moleculeReactionsVTableMaxSz] = 
{
    {/*[0:CIRCLIT][0:CIRCLIT]=*/CirclitCirclitReaction, /*[0:CIRCLIT][1:QUADRIT]=*/ CirclitQuadritReaction},
    {/*[1:QUADRIT][0:CIRCLIT]=*/CirclitQuadritReaction, /*[1:QUADRIT][1:QUADRIT]=*/ QuadritQuadritReaction}
};




class Molecule {
    MoleculeTypes moleculeType;
    MoleculePhysicalStates moleculePhysicalState;

    gm_vector<double, 2> position;
    gm_vector<double, 2> speedVector;
    int mass;

    gm_vector<unsigned char, 3> color;

    friend class Circlit;
    friend class Quadrit;
    friend void launchMoleculeReaction(
        std::list<std::unique_ptr<Molecule>> &moleculeList,
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT);

private:
    Molecule
    (
        const gm_vector<double, 2> &position, 
        const gm_vector<double, 2> &speedVector, 
        const int mass,
        const gm_vector<unsigned char, 3> &color
    ):
        position(position),
        speedVector(speedVector),
        mass(mass),
        color(color)
    {
        moleculeType = NONE;
        moleculePhysicalState = ALIVE;
    };
    

public:
    virtual ~Molecule() {};

    virtual ShapeType getShapeType() const { return ShapeType::NONE_SHAPE_TYPE; };
    virtual double getSize() const { return 0; };
    virtual double getCollideCircleRadius() const { return 0; }

    gm_vector<unsigned char, 3> getColor() const { return color; }
    gm_vector<double, 2> getPosition() const { return position; }
    gm_vector<double, 2> getSpeedVector() const { return speedVector; }
    int getMass() const { return mass; }

    void setPosition(const gm_vector<double, 2> &newPosition) {
        position = newPosition;
    }

    void sePhysicalState(const MoleculePhysicalStates state) { moleculePhysicalState = state; }

    MoleculeTypes getMoleculeType() const { return moleculeType; }
    MoleculePhysicalStates getPhysicalState() const { return moleculePhysicalState; }

    void setspeedVector(const gm_vector<double, 2> &newspeedVector) {
        speedVector = newspeedVector;
    }

    

};

class Circlit : public Molecule {
    double radius;

public:    
    Circlit
    (
        const gm_vector<double, 2> &position, 
        const gm_vector<double, 2> &speedVector, 
        const int mass, 
        gm_vector<unsigned char, 3> color=CIRCLIT_COLOR
    ): 
        Molecule(position, speedVector, mass, color)
    { 
        moleculeType = MoleculeTypes::CIRCLIT;
        radius = getRadius(); 
    }

    ~Circlit() override {}


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
        const gm_vector<double, 2> &speedVector, 
        const int mass,  
        gm_vector<unsigned char, 3> color=QUADRIT_COLOR
    ): 
        Molecule(position, speedVector, mass, color)
    { 
        moleculeType = MoleculeTypes::QUADRIT;
        length = getLength(); 
    }

    ~Quadrit() override {}


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
        walls[LEFT_WALL]  = gm_line<double, 2>({0, 0}, {0, 1});
        walls[LOWER_WALL] = gm_line<double, 2>({0, cordSysHeight}, {1, 0});
        walls[RIGHT_WALL] = gm_line<double, 2>({cordSysWidth,  0}, {0, 1});
    }
    
    gm_vector<int, 2> convertMoleculeCords(const gm_vector<double, 2> &moleculeCords) const {
        return moleculeCords * coreCordSystemScale + coreCanvasPos;
    }

    double randRange(double start, double end) {
        return start + (end - start) * randomGenerator() / double(randomGenerator.max());
    }

    const std::list<std::unique_ptr<Molecule>> &getMoleculeList() const { return moleculesList; }


private:
    gm_vector<double, 2> genRandomVec(const double xMin, const double xMax, const double yMin, const double yMax) {
        return gm_vector<double, 2> (randRange(xMin, xMax), randRange(yMin, yMax));
    }

    gm_vector<double, 2> genRandomSpeedVec(const double minSpeed, const double maxSpeed) {
        double randomAngle = randRange(0, 2 * std::numbers::pi);
        gm_vector<double, 2> initSpeedVector = gm_vector<double, 2>(0, 1) * randRange(minSpeed, maxSpeed);
        return initSpeedVector.rotate(randomAngle);
    }

    void reactorCoreAddMolecule(const enum MoleculeTypes moleculeType) {
        gm_vector<double, 2> moleculePosition = genRandomVec(0, cordSysWidth, 0, cordSysHeight);
        gm_vector<double, 2> moleculetspeedVector = genRandomSpeedVec(MoleculeMinInitSpeed, MoleculeMaxInitSpeed);

        switch (moleculeType) {
            case CIRCLIT:
                moleculesList.push_back(std::make_unique<Circlit>(moleculePosition, moleculetspeedVector, INITIAL_MASS));
                break;
            case QUADRIT:
                moleculesList.push_back(std::make_unique<Quadrit>(moleculePosition, moleculetspeedVector, INITIAL_MASS));
                break;
            default:
                assert(0 && "switch(moleculeType) default");
        }   
    }

    void updateMoleculePosition(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT, const double deltaSecs) {
        Molecule *moleculePtr = (*moleculeIT).get();
        
        gm_vector<double, 2> newPosition = 
            moleculePtr->getPosition() + 
            moleculePtr->getSpeedVector() * deltaSecs;
        
        moleculePtr->setPosition(newPosition);
    }


    double getWallIntersectionDelta(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT, WallType wallType) {
        Molecule *moleculePtr = (*moleculeIT).get();

        gm_line<double, 2> moveRay(moleculePtr->getPosition(), moleculePtr->getSpeedVector());
        gm_vector<double, 2> intersection = get_ray_line_intersection(moveRay, walls[wallType]);
        
        if (intersection.is_poison()) return std::numeric_limits<double>::quiet_NaN();
        if (intersection.get_x() < 0 || intersection.get_x() > cordSysWidth) return std::numeric_limits<double>::quiet_NaN();
        if (intersection.get_y() < 0 || intersection.get_y() > cordSysHeight) return std::numeric_limits<double>::quiet_NaN();

        gm_vector<double, 2> path = intersection - moleculePtr->getPosition();

        double curDelta = path.get_len2() / moleculePtr->getSpeedVector().get_len2();

        return std::sqrt(curDelta);
    } 

    gm_vector<double, 2> processWallCollision(const gm_vector<double, 2> &speedVector, const WallType wallType) {
        double x = speedVector.get_x();
        double y = speedVector.get_y();

        switch (wallType) {
            case UPPER_WALL:
                return gm_vector<double, 2>({x, -y});
            case LEFT_WALL:
                return gm_vector<double, 2>({-x, y});
            case LOWER_WALL:
                return gm_vector<double, 2>({x, -y});
            case RIGHT_WALL:
                return gm_vector<double, 2>({-x, y});
            default:
                assert(0 && "unknown wallType");
        }
    }

    void ProcessMoleculeMovement(std::list<std::unique_ptr<Molecule>>::iterator moleculeIT, double deltaSecs) {
        Molecule *moleculePtr = (*moleculeIT).get();

        if (moleculePtr->getPhysicalState() == DEATH) return; 

        for (size_t i = 0; i < WALLS_CNT; i++) {
            WallType wallType = WallType (i);

            double intersectionDelta = getWallIntersectionDelta(moleculeIT, wallType);

            if (std::isnan(intersectionDelta) || intersectionDelta > deltaSecs) continue;

            gm_vector<double, 2> moveVector = 
                moleculePtr->getSpeedVector() * intersectionDelta + 
                processWallCollision(moleculePtr->getSpeedVector() * (deltaSecs - intersectionDelta), wallType);
    
            moleculePtr->setPosition(moleculePtr->getPosition() + moveVector);
            moleculePtr->setspeedVector(processWallCollision(moleculePtr->getSpeedVector(), wallType));
            return;
        }

        updateMoleculePosition(moleculeIT, deltaSecs);
    }

    double getMoleculeCollisionDelta(
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
    ) {
        Molecule *fstMoleculePTR = (*fstMoleculeIT).get();
        Molecule *sndMoleculePTR = (*sndMoleculeIT).get();

        double coliisionRadius = fstMoleculePTR->getCollideCircleRadius() + sndMoleculePTR->getCollideCircleRadius();
        double coliisionRadius2 = coliisionRadius * coliisionRadius;

    
        gm_vector<double, 2> V = fstMoleculePTR->getSpeedVector() - sndMoleculePTR->getSpeedVector();
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

    void processMoleculeCollision
    (
        std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
        std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT 
    ) {
     

        Molecule *fstMoleculePTR = (*fstMoleculeIT).get();
        Molecule *sndMoleculePTR = (*sndMoleculeIT).get();

        if (fstMoleculePTR->getPhysicalState() != ALIVE || sndMoleculePTR->getPhysicalState() != ALIVE) return; 

        double distance2 = (fstMoleculePTR->getPosition() - sndMoleculePTR->getPosition()).get_len2();
        double collisionDistance = (fstMoleculePTR->getCollideCircleRadius() + sndMoleculePTR->getCollideCircleRadius());

        if (distance2 - collisionDistance * collisionDistance < DISTANCE_COLLISION_EPS2)
            launchMoleculeReaction(moleculesList, fstMoleculeIT, sndMoleculeIT);
    }

signals:
    void reactorCoreUpdated();

public slots:
    void reactorCoreUpdate(const double deltaSecs) {
        for (auto fstMoleculeIT = moleculesList.begin(); fstMoleculeIT != moleculesList.end() && 
                                                         (*fstMoleculeIT).get()->getPhysicalState() != UNRESPONSIVE; fstMoleculeIT++) {
            ProcessMoleculeMovement(fstMoleculeIT, deltaSecs);
        
            for (auto sndMoleculeIT = std::next(fstMoleculeIT); sndMoleculeIT != moleculesList.end(); sndMoleculeIT++) {
                processMoleculeCollision(fstMoleculeIT, sndMoleculeIT);
            }
        }

        
        std::list<std::unique_ptr<Molecule>>::iterator curMoleculeIT = moleculesList.begin();
        while (curMoleculeIT != moleculesList.end()) {
            auto nextMoleculeIT = std::next(curMoleculeIT);
            Molecule *curMoleculePTR = (*curMoleculeIT).get(); 
            
            switch(curMoleculePTR->getPhysicalState()) {
                case DEATH: moleculesList.erase(curMoleculeIT); break;
                case UNRESPONSIVE: curMoleculePTR->sePhysicalState(ALIVE); break;
                case ALIVE: break;
                default: assert(0);
            }
            curMoleculeIT = nextMoleculeIT;
        }

        emit reactorCoreUpdated();
    }

private slots:
    void reactorCoreUpdateHandle() {
        reactorCoreUpdate(REACTOR_CORE_UPDATE_SECS);
    }
};


#endif // REACTORCORE_H