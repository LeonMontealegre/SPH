#ifndef MESH_H
#define MESH_H

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

#include <cstdlib>
#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "shader.hpp"

using namespace std;

class mesh {
public:
    mesh(int, int, float*, unsigned int*, float*, float*);
    ~mesh();
    
    void bind();
    void unbind();
    void draw();
    void render();
private:
    GLuint vao, vbo, ibo, tbo, nbo;
    int count;
};

#endif
