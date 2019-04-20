#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "mesh.hpp"

using namespace std;

struct index_group;
struct face;

void processFaceVertex(index_group&, vector<float>&, vector<float>&, vector<int>&, float*, float*);
mesh& reorderAttributes(vector<float>&, vector<float>&, vector<float>&, vector<face>&);
mesh& loadOBJ(string);

#endif
