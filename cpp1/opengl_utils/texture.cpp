#include "texture.hpp"

texture::texture(std::string path) {
    std::ifstream input(path);
    
    std::string line;
    input >> line >> width >> height >> line;
    
    float* pixels = new float[width * height * 3];
    char token;
    input.get(token);
    for (int i = 0; !input.eof() && i < width*height*3; input.get(token), pixels[i++] = (((int)token + 256) % 256) / 256.0f);
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    delete [] pixels;
}

texture::~texture() {
    glDeleteTextures(1, &id);
}

int texture::getWidth() {
    return width;
}

int texture::getHeight() {
    return height;
}

void texture::bind(GLenum unit) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void texture::unbind(GLenum unit) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}
