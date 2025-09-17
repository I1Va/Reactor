#include "reactorcore.h"

void Molecule::moleculeLaunchReaction(bool thisCirclitState, const Molecule &other, std::list<std::unique_ptr<Molecule>> &reactionResult) const {
    bool otherCirclitState = typeid(other) == typeid(Circlit);

    gm_vector<double, 2> collideCenter = position + (other.position - position) * 0.5;

    if ((thisCirclitState ^ otherCirclitState) || (thisCirclitState && otherCirclitState)) {
        int newMass = mass + other.mass;
        gm_vector<double, 2> newMoveVector = (moveVector * mass + other.moveVector * other.mass) * (1.0 / newMass);
        
        reactionResult.push_back(std::make_unique<Quadrit>(collideCenter, newMoveVector, newMass));        
    } else {
        gm_vector<double, 2> boomCenter = position + (other.position - position) * 0.5;
        int boomMoleculesCount = mass + other.mass;
        double newRadius = CIRCLIT_MIN_RADIUS * std::sin(std::numbers::pi / boomMoleculesCount);
        double boomRootationAngle = 2 * std::numbers::pi / boomMoleculesCount;
        gm_vector<double, 2> boomCurMoveVector = gm_vector<double, 2>(0, -1);
    
        for (int i = 0; i < boomMoleculesCount; i++) {
            reactionResult.push_back(std::make_unique<Circlit>(collideCenter + boomCurMoveVector, boomCurMoveVector, 1));
        }
    }
}

void ReactorCore::reactorCoreUpdate() {
    std::cout << "reactorCoreUpdate\n";
}

void ReactorCore::addMolecule(const bool circlitState) {
    gm_vector<double, 2> moleculePosition(randRange(0, cordSysWidth), randRange(0, cordSysHeight));

    double randomAngle = randRange(0, 2 * std::numbers::pi);
    gm_vector<double, 2> moleculetMoveVector = INITIAL_MOVEVECTOR.rotate(randomAngle);

    if (circlitState) {
        moleculeList.push_back(std::make_unique<Circlit>(moleculePosition, moleculetMoveVector, INITIAL_MASS));
    }  else {
        moleculeList.push_back(std::make_unique<Quadrit>(moleculePosition, moleculetMoveVector, INITIAL_MASS));
    }
}

void ReactorCore::addCirclit() {
    addMolecule(/*CirclitState=*/true);
}

void ReactorCore::addQuadrit() {
     addMolecule(/*CirclitState=*/false);
}
