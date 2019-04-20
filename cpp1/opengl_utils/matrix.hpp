#ifndef MATRIX_H
#define MATRIX_H

#define PI 3.1415926535

#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace std;

struct matrix {
    float mat[4 * 4]; // 4 x 4 matrix
    
    matrix();
    matrix(float*);
    matrix& operator = (const matrix&);
    
    matrix& zero();
    matrix& identity();
    matrix& perspective(float, float, float, float);
    matrix& orthographic(float, float, float, float, float, float);
    
    matrix& translate(float, float, float);
    matrix& scale(float, float, float);
    matrix& rotate(float, float, float);
    
    matrix& setPosition(float, float, float);
    
    matrix& multiply(const matrix&);
    
    matrix& inverse();
    
    auto operator * (const matrix& other)->decltype(multiply(other));
    auto operator * (float factor)->decltype(scale(factor, factor, factor));
    auto operator / (float factor)->decltype(scale(1/factor, 1/factor, 1/factor));
    
    friend ostream& operator << (ostream&, const matrix&);
    friend istream& operator >> (istream&, matrix&);
};

#endif
