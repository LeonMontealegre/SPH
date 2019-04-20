#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "matrix.hpp"

class texture {
public:
    texture(std::string path);
    ~texture();
    
    int getWidth();
    int getHeight();
    
    void bind(GLenum = GL_TEXTURE0);
    void unbind(GLenum = GL_TEXTURE0);
private:
    GLuint id;
    int width, height;
};

#endif
