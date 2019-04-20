#ifndef SHADER_H
#define SHADER_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <OpenGL/glu.h>
#include "matrix.hpp"

using namespace std;

enum SHADER_ERRORS {ALREADY_CREATED};

extern const int VERTEX_ATTRIB;
extern const int TCOORD_ATTRIB;
extern const int NORMAL_ATTRIB;

class shader {
public:
    
    shader();
    shader(string, string);
    ~shader();
    
    void init(const char*, const char*);
    
    void enable();
    void disable();
    
    GLuint getUniform(string);
    void setUniform(string, int);
    void setUniform(string, float);
    void setUniform(string, float, float);
    void setUniform(string, float, float, float);
    void setUniform(string, float, float, float, float);
    void setUniform(string, matrix&);
        
private:
    GLuint program;
    bool enabled;
    map<string, GLuint> locationMap;
    
    GLuint addShader(const char*, GLenum);
};

#endif
