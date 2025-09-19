#include "reactorcore.h"

#include <limits>



void QuadritQuadritReaction(
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
) {
    Molecule *fstMoleculePTR = (*fstMoleculeIT).get();
    Molecule *sndMoleculePTR = (*sndMoleculeIT).get();

    gm_vector<double, 2> collideCenter = fstMoleculePTR->getPosition() + (fstMoleculePTR->getPosition() - fstMoleculePTR->getPosition()) * 0.5;

    int boomMoleculeCnt = fstMoleculePTR->getMass() + fstMoleculePTR->getMass();
    
    
    double newRadius = CIRCLIT_MIN_RADIUS * std::sin(std::numbers::pi / boomMoleculeCnt);
    double boomRootationAngle = 2 * std::numbers::pi / boomMoleculeCnt;
    gm_vector<double, 2> boomCurspeedVector = gm_vector<double, 2>(0, -1);

    for (int i = 0; i < boomMoleculeCnt; i++) {
        moleculeList.push_back(std::make_unique<Circlit>(collideCenter + boomCurspeedVector, boomCurspeedVector, 1));
    }
}

void CirclitQuadritReaction(
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT
) {
    Molecule *fstMoleculePTR = (*fstMoleculeIT).get();
    Molecule *sndMoleculePTR = (*sndMoleculeIT).get();

    gm_vector<double, 2> collideCenter = fstMoleculePTR->getPosition() + (fstMoleculePTR->getPosition() - fstMoleculePTR->getPosition()) * 0.5;

    int newMass = fstMoleculePTR->getMass() + sndMoleculePTR->getMass();
   
    
    gm_vector<double, 2> newspeedVector = (fstMoleculePTR->getSpeedVector() * fstMoleculePTR->getMass() + 
                                          sndMoleculePTR->getSpeedVector() * sndMoleculePTR->getMass()) * (1.0 / newMass);

    
    fstMoleculePTR->sePhysicalState(DEATH);
    sndMoleculePTR->sePhysicalState(DEATH);

    moleculeList.push_back(std::make_unique<Quadrit>(collideCenter, newspeedVector, newMass));
    (*(std::prev(moleculeList.end()))).get()->sePhysicalState(UNRESPONSIVE);        
}



void launchMoleculeReaction (
    std::list<std::unique_ptr<Molecule>> &moleculeList,
    std::list<std::unique_ptr<Molecule>>::iterator fstMoleculeIT,
    std::list<std::unique_ptr<Molecule>>::iterator sndMoleculeIT 
) {
    MoleculeTypes fstMoleculeType = (*fstMoleculeIT).get()->moleculeType;
    MoleculeTypes sndMoleculeType = (*sndMoleculeIT).get()->moleculeType;


    moleculeReaction reactionFunc = moleculeReactionsVTable[fstMoleculeType][sndMoleculeType];

    if (reactionFunc == NULL) {
        std::cout << "Unknown Reaction : " << fstMoleculeType << " + " << sndMoleculeType << "\n";
        assert(0);
        return;
    }

    reactionFunc(moleculeList, fstMoleculeIT, sndMoleculeIT);
}

