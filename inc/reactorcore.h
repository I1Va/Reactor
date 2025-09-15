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

    virtual void collide(const Molecule &other, std::list<Molecule> &reactionResult);
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