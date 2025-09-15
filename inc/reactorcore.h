#ifndef REACTORCORE_H
#define REACTORCORE_H

#include "gm_primitives.hpp"
#include <list>
#include <typeinfo>
#include <cstring>
#include <numbers>

static const size_t MAX_CLASS_NAME_LEN = 10;

static const gm_vector<unsigned char, 3> CIRCLIT_COLOR(255, 0, 0);
static const gm_vector<unsigned char, 3> QUADRIT_COLOR(0, 0, 255);


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

    virtual void collide(const Molecule &other, std::list<Molecule> &reactionResult) { 
        bool thisCirclitState = strncmp(typeid(*this).name(), "Circlit", MAX_CLASS_NAME_LEN);
        bool otherCirclitState = strncmp(typeid(other).name(), "Circlit", MAX_CLASS_NAME_LEN);

        gm_vector<double, 2> collideCenter = position + (other.position - position) * 0.5;

        if ((thisCirclitState ^ otherCirclitState) || (thisCirclitState && otherCirclitState)) {
            int newMass = mass + other.mass;
            gm_vector<double, 2> newMoveVector = (moveVector * mass + other.moveVector * other.mass) * (1.0 / newMass);
            
            Quadrit newMolecule(collideCenter, newMoveVector, newMass);
            reactionResult.push_back(newMolecule);        
        } else {
            gm_vector<double, 2> boomCenter = position + (other.position - position) * 0.5;
            int boomMoleculesCount = mass + other.mass;
            double newRadius = Circlit::MIN_RADIUS * std::sin(std::numbers::pi / boomMoleculesCount);
            double boomRootationAngle = 2 * std::numbers::pi / boomMoleculesCount;

            gm_vector<double, 2> boomCurMoveVector = gm_vector<double, 2>(0, -1);
            for (int i = 0; i < boomMoleculesCount; i++) {
                Circlit newMolecule(collideCenter + boomCurMoveVector, boomCurMoveVector, 1);
                reactionResult.push_back(newMolecule);
            }
        }
    }
};

class Circlit : public Molecule {
    double radius; // radius >= minRaius
public:
    static constexpr double MIN_RADIUS = 1;

    Circlit
    (
        const gm_vector<double, 2> &position, 
        const gm_vector<double, 2> &moveVector, 
        const int mass, 
        gm_vector<unsigned char, 3> color=CIRCLIT_COLOR
    ): 
        Molecule(position, moveVector, mass, color) {}
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
        Molecule(position, moveVector, mass, color) {}
};


#endif // REACTORCORE_H