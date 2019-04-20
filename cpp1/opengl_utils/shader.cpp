#include "shader.hpp"

const int VERTEX_ATTRIB = 0;
const int TCOORD_ATTRIB = 1;
const int NORMAL_ATTRIB = 2;

static shader* current = NULL;

shader::shader() {
    program = 0;
    enabled = false;
}

shader::shader(string vertFile, string fragFile) {
    program = 0;
    
    ifstream vfs(vertFile);
    string vert((istreambuf_iterator<char>(vfs)), (istreambuf_iterator<char>()));
    vfs.close();
    
    ifstream ffs(fragFile);
    string frag((istreambuf_iterator<char>(ffs)), (istreambuf_iterator<char>()));
    ffs.close();
    
    init(vert.c_str(), frag.c_str());
}

shader::~shader() {
    disable();
    glDeleteProgram(program);
    program = -1;
    locationMap.clear();
}

void shader::init(const char* vert, const char* frag) {
    if (program > 0)
        throw ALREADY_CREATED;
    
    program = glCreateProgram();
    GLuint vertID = addShader(vert, GL_VERTEX_SHADER);
    GLuint fragID = addShader(frag, GL_FRAGMENT_SHADER);
    
    // Extremely important, binds attributes to correct indices
    // Otherwise it can be entirely 'random' and cause strange errors
    glBindAttribLocation(program, VERTEX_ATTRIB, "inPosition");
    glBindAttribLocation(program, TCOORD_ATTRIB, "inTexCoords");
    glBindAttribLocation(program, NORMAL_ATTRIB, "inNormal");
    
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vertID);
    glDeleteShader(fragID);
    
    enabled = false;
}

void shader::enable() {
    if (current != NULL)
        current->disable();
    
    current = this;
    glUseProgram(program);
    enabled = true;
}

void shader::disable() {
    if (current == this) {
        current = NULL;
        glUseProgram(0);
    }
    enabled = false;
}

GLuint shader::getUniform(string name) {
    if (locationMap.count(name) > 0)
        return locationMap[name];
    
    GLuint result = glGetUniformLocation(program, name.c_str());
    if (result == -1)
        fprintf(stderr, "Could not find uniform variable '%s'!\n", name.c_str());
    else
        locationMap[name] = result;
    
    return result;
}

void shader::setUniform(string name, int value) {
    if (!enabled) enable();
    glUniform1i(getUniform(name), value);
}

void shader::setUniform(string name, float value) {
    if (!enabled) enable();
    glUniform1f(getUniform(name), value);
}

void shader::setUniform(string name, float x, float y) {
    if (!enabled) enable();
    glUniform2f(getUniform(name), x, y);
}

void shader::setUniform(string name, float x, float y, float z) {
    if (!enabled) enable();
    glUniform3f(getUniform(name), x, y, z);
}

void shader::setUniform(string name, float x, float y, float z, float w) {
    if (!enabled) enable();
    glUniform4f(getUniform(name), x, y, z, w);
}

void shader::setUniform(string name, matrix& mat) {
    if (!enabled) enable();
    glUniformMatrix4fv(getUniform(name), 1, false, mat.mat);
}

GLuint shader::addShader(const char* text, GLenum shaderType) {
    GLuint id = glCreateShader(shaderType);
    
    const GLchar* p[1] = { text };
    GLint lengths[1] = { static_cast<GLint>(std::strlen(text)) };
    glShaderSource(id, 1, p, lengths);
    
    glCompileShader(id);
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(id, 1024, NULL, infoLog);
        fprintf(stderr, "Error compiling shader type %d:\n'%s'\n", shaderType, infoLog);
        return id;
    }
    
    glAttachShader(program, id);
    return id;
}
