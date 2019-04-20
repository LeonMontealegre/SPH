#ifndef SPH_H
#define SPH_H

#include <vector>

#include "particle.h"

class SPH {
public:
    SPH();

private:
    std::vector<Particle*> particles;
};

#endif
