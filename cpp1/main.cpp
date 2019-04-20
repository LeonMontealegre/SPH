#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <string>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include "opengl_utils/matrix.hpp"
#include "opengl_utils/shader.hpp"
#include "opengl_utils/texture.hpp"

// GLUT requires global variables since it doesn't allow parameter passing to functions.
const int width = 1120, height = 630;

static shader* globeShader;
static shader* airportShader;
static shader* pathShader;

static globe* g;

static texture* earthDayTexture;
static texture* earthNightTexture;

static matrix* projMatrix;
static matrix* viewMatrix;

static int prevX = 0, prevY = 0, curX = 0, curY = 0;
static bool clicked = false;
static bool justClicked = false;

static dijkstra* dijk;
static GLuint airportVAO;
static GLuint airportVBO;

static dijkstra::path* p;
static GLuint pathVAO;
static GLuint pathVBO;

static float zoom = 1.0f;

static airport *a1, *a2;
static int choice = 0;

static map<char, bool> keys;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render map
    earthDayTexture->bind(GL_TEXTURE0);
    earthNightTexture->bind(GL_TEXTURE1);
    globeShader->setUniform("projMatrix", *projMatrix);
    globeShader->setUniform("viewMatrix", *viewMatrix);
    globeShader->setUniform("modelMatrix", *g->getTransform());
    g->render();
    earthNightTexture->unbind(GL_TEXTURE1);
    earthDayTexture->unbind(GL_TEXTURE0);
    
    
    // Draw airports
    airportShader->setUniform("projMatrix", *projMatrix);
    airportShader->setUniform("viewMatrix", *viewMatrix);
    airportShader->setUniform("modelMatrix", *g->getTransform());
    glBindVertexArray(airportVAO);
    glDrawArrays(GL_POINTS, 0, dijk->getAirports().size());
    
    
    // Draw path
    if (pathVAO != 0 && p) {
        pathShader->setUniform("projMatrix", *projMatrix);
        pathShader->setUniform("viewMatrix", *viewMatrix);
        pathShader->setUniform("modelMatrix", *g->getTransform());
        glBindVertexArray(pathVAO);
        glDrawArrays(GL_LINE_STRIP, 0, p->routes.size());
    }
    
    int i = glGetError();
    if (i != GL_NO_ERROR)
        fprintf(stderr, "Error : %i\n", i);
    
    glutSwapBuffers();
}

void generatePath() {
    if (pathVAO != 0) {
        glDeleteVertexArrays(1, &pathVAO);
        glDeleteBuffers(1, &pathVBO);
    }
    
    glGenVertexArrays(1, &pathVAO);
    glBindVertexArray(pathVAO);
    
    GLfloat pathPositions[2*p->routes.size()];
    for (int i = 0; i < p->routes.size(); i++) {
        pathPositions[2*i+0] = 1.0f/360 * p->routes[i]->src->longitude;
        pathPositions[2*i+1] = -1.0f/180 * p->routes[i]->src->latitude;
    }
    
    glGenBuffers(1, &pathVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pathPositions), pathPositions, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    cout << p->routes.size() << endl;
    cout << "Leave " << a1->iata << " (" << a1->city << ") on " << p->routes[0]->line->name << " flying " << p->routes[0]->distance << " miles" << endl;
    
    for (int i = 1; i < p->routes.size()-1; i++)
        cout << "Change planes at " << p->routes[i]->src->iata << " (" << p->routes[i]->src->city << ") to " << p->routes[i]->line->name << " and fly " << p->routes[i]->distance << " miles" << endl;
    
    cout << "Arrive at " << a2->iata << " (" << a2->city << ")" << endl;
}

void update(int i) {
    glutTimerFunc(20, update, -1);
    
    int dx = curX - prevX;
    int dy = prevY - curY;
    
    // Translate map
    if (clicked)
        viewMatrix->translate(dx/400.0f/zoom, dy/400.0f/zoom, 0);
    
    
    // Clamp position
    float w = (float)width/height;
    if (viewMatrix->mat[0 + 3 * 4] - w/2/zoom < -w/2)
        viewMatrix->mat[0 + 3 * 4] = -w/2 + w/2/zoom;
    else if (viewMatrix->mat[0 + 3 * 4] + w/2/zoom > w/2)
        viewMatrix->mat[0 + 3 * 4] = w/2 - w/2/zoom;
    
    if (viewMatrix->mat[1 + 3 * 4] - 0.5f/zoom < -0.5f)
        viewMatrix->mat[1 + 3 * 4] = -0.5f + 0.5f / zoom;
    else if (viewMatrix->mat[1 + 3 * 4] + 0.5f/zoom > 0.5f)
        viewMatrix->mat[1 + 3 * 4] = 0.5f - 0.5f / zoom;
    
    
    // Zoom
    if (keys['e']) {
        zoom += 0.1f;
        projMatrix->orthographic(-w/2/zoom, w/2/zoom, -0.5f/zoom, 0.5f/zoom, 0.1f, 1000.0f);
    }
    if (keys['q'] && zoom > 1.0f) {
        zoom -= 0.1f;
        zoom = (zoom < 1.0f) ? (1.0f) : (zoom);
        projMatrix->orthographic(-w/2/zoom, w/2/zoom, -0.5f/zoom, 0.5f/zoom, 0.1f, 1000.0f);
    }
    
    
    // Select airport
    float curLong = ((float)curX / width  * 2.0f - 1.0f) * 180.0f / zoom - viewMatrix->mat[0 + 3 * 4]/(w/2) * 180.0f;
    float curLat  = (1.0f - (float)curY / height * 2.0f) *  90.0f / zoom - viewMatrix->mat[1 + 3 * 4] * 90.0f * 2;
    
    if (justClicked) {
        justClicked = false;
        std::vector<airport*> airports = dijk->getAirports();
        for (int i = 0; i < airports.size(); i++) {
            float dx = airports[i]->longitude - curLong;
            float dy = airports[i]->latitude  - curLat;
            if (dx * dx + dy * dy <= 1/zoom) {
                
                if (choice == 0) {
                    a1 = airports[i];
                    cout << "Source airport : " << a1->name << " (" << a1->iata << ")" << endl;
                } else {
                    a2 = airports[i];
                    cout << "Target airport : " << a2->name << " (" << a2->iata << ")" << endl;
                }
                airportShader->setUniform("greenPorts[" + std::to_string(choice) + "]", (float)airports[i]->id);
                choice = (choice + 1) % 2;
                
                if (a1 != NULL && a2 != NULL && a1 != a2) {
                    p = dijk->getPath(a1, a2);
                    if (p != NULL) {
                        generatePath();
                    } else {
                        cout << "No path found between " << a1->name << " and " << a2->name << endl;
                    }
                }
                
                break;
            }
        }
    }
    
    prevX = curX;
    prevY = curY;
    
    glutPostRedisplay();
}

void onMouseMove(int x, int y) {
    curX = x;
    curY = y;
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        clicked = (state == GLUT_DOWN);
        justClicked = (state == GLUT_UP);
    }
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Smoothed Particle Hydrodynamics");
    
    glutDisplayFunc(display);
    glutMotionFunc(onMouseMove);
    glutPassiveMotionFunc(onMouseMove);
    glutMouseFunc(onMouse);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Create and link shaders
    globeShader = new shader("res/shaders/globe.vert", "res/shaders/globe.frag");
    airportShader = new shader("res/shaders/airport2.vert", "res/shaders/airport.frag");
    pathShader = new shader("res/shaders/path2.vert", "res/shaders/path.frag");
    
    // Projection matrix
    projMatrix = new matrix;
    float w = (float)width/height;
    projMatrix->orthographic(-w/2, w/2, -0.5f, 0.5f, 0.1f, 1000.0f);
    
    // View matrix
    viewMatrix = new matrix;
    viewMatrix->translate(0, 0, -1.0f);
    
    globeShader->setUniform("projMatrix", *projMatrix);
    globeShader->setUniform("viewMatrix", *viewMatrix);
    globeShader->setUniform("dayTex", 0);
    globeShader->setUniform("nightTex", 1);
    globeShader->setUniform("lightDirection", 0.8, 0.2f, 0.2f);
    globeShader->setUniform("ambientLight", 0.1f);
    
    airportShader->setUniform("projMatrix", *projMatrix);
    airportShader->setUniform("viewMatrix", *viewMatrix);
    
    pathShader->setUniform("projMatrix", *projMatrix);
    pathShader->setUniform("viewMatrix", *viewMatrix);
    
    // Create globe
    g = new globe("res/models/quad.obj");
    g->getTransform()->rotate(-1.57f, 0, 0);
    g->getTransform()->scale((float)width/height, 1.0f, 1);
    earthDayTexture = new texture("res/textures/earth.ppm");
    earthNightTexture = new texture("res/textures/earth_night.ppm");
    
    // Load airports
    dijk = new dijkstra("combined.txt");
    glGenVertexArrays(1, &airportVAO);
    glBindVertexArray(airportVAO);
    
    std::vector<airport*> airports = dijk->getAirports();
    GLfloat airportWorldPos[3*airports.size()];
    for (int i = 0; i < airports.size(); i++) {
        airportWorldPos[3*i+0] = 1.0f/360 * airports[i]->longitude;
        airportWorldPos[3*i+1] = -1.0f/180 * airports[i]->latitude;
        airportWorldPos[3*i+2] = (float)airports[i]->id;
    }
    
    glGenBuffers(1, &airportVBO);
    glBindBuffer(GL_ARRAY_BUFFER, airportVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(airportWorldPos), airportWorldPos, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    a1 = dijk->getAirports()[dijk->airportIDtoIndex[a1id]];
    a2 = dijk->getAirports()[dijk->airportIDtoIndex[a2id]];
    p = dijk->getPath(a1, a2);
    if (p != NULL)
        generatePath();

    glPointSize(4.0f);
    
    // Begin updating + rendering
    glutTimerFunc(0, update, -1);
    
    glutMainLoop();
}




