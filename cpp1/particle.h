#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec3.h"

struct Particle {
    vec3 pos;
    vec3 vel;
    vec3 acc;

    float mass;
    float restDensity;
    float viscosity;
    float gasConstant;

    float density;
    float pressure;

    Particle(const vec3& pos,
             float mass = DEFAULT_MASS,
             float restDensity = DEFAULT_DENSITY,
             float viscosity = DEFAULT_VISOSCITY,
             float gasConstant = DEFAULT_GAS_CONSTANT) {
        this->pos = pos;
        this->vel = vec3();
        this->acc = vec3();

        this->mass = mass;
        this->restDensity = restDensity;
        this->viscosity = viscosity;
        this->gasConstant = gasConstant;

        this->density = 0;
        this->pressure = 0;
    }
};

#endif
