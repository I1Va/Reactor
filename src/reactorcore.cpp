#include "reactorcore.h"

#include <limits>



// void ReactorCore::moleculeLaunchReaction() {
//     bool otherCirclitState = typeid(other) == typeid(Circlit);



//     gm_vector<double, 2> collideCenter = position + (other.position - position) * 0.5;

//     if ((thisCirclitState ^ otherCirclitState) || (thisCirclitState && otherCirclitState)) {
//         int newMass = mass + other.mass;
//         gm_vector<double, 2> newMoveVector = (moveVector * mass + other.moveVector * other.mass) * (1.0 / newMass);
        
//         reactionResult.push_back(std::make_unique<Quadrit>(collideCenter, newMoveVector, newMass));        
//     } else {
//         gm_vector<double, 2> boomCenter = position + (other.position - position) * 0.5;
//         int boomMoleculesCount = mass + other.mass;
//         double newRadius = CIRCLIT_MIN_RADIUS * std::sin(std::numbers::pi / boomMoleculesCount);
//         double boomRootationAngle = 2 * std::numbers::pi / boomMoleculesCount;
//         gm_vector<double, 2> boomCurMoveVector = gm_vector<double, 2>(0, -1);
    
//         for (int i = 0; i < boomMoleculesCount; i++) {
//             reactionResult.push_back(std::make_unique<Circlit>(collideCenter + boomCurMoveVector, boomCurMoveVector, 1));
//         }
//     }
// }

void ReactorCore::reactorCoreUpdate(const double deltaSecs) {
    emit reactorCoreUpdated();

    updateClosestKeyEventInfo();
    double closestEventDelta = closestEventTimePoint - currentReactorCoreTime;
    
    std::cout << "closestEventDelta : " << closestEventDelta << "\n";

    if (std::isnan(closestEventDelta) || deltaSecs < closestEventDelta) {
        stableProcessContinue(deltaSecs);
        return;
    }

    if ((std::fabs(deltaSecs - closestEventDelta)) < std::numeric_limits<double>::epsilon()) {
        reactorCoreUpdate(currentReactorCoreTime - closestEventTimePoint);
        return;
    }

    // deltaSecs > closestEventDelta
    stableProcessContinue(closestEventDelta); // waiting for closest event
    processReactorCoreEvent();

    reactorCoreUpdate(deltaSecs - closestEventDelta);
    
}

void ReactorCore::reactorCoreUpdateHandle() {
    reactorCoreUpdate(REACTOR_CORE_UPDATE_SECS);
}