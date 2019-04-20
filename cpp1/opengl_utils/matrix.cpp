#include "matrix.hpp"

matrix::matrix() {
    identity();
}

matrix::matrix(float* m) {
    for (int i = 0; i < 16; ++i)
        mat[i] = m[i];
}

matrix& matrix::operator = (const matrix& other) {
    if (this != &other) {
        for (int i = 0; i < 16; ++i)
            mat[i] = other.mat[i];
    }
    
    return *this;
}

matrix& matrix::zero() {
    for (int i = 0; i < 16; ++i)
        mat[i] = 0;
    
    return *this;
}

matrix& matrix::identity() {
    zero();
    
    mat[0 + 0 * 4] = 1.0f;
    mat[1 + 1 * 4] = 1.0f;
    mat[2 + 2 * 4] = 1.0f;
    mat[3 + 3 * 4] = 1.0f;
    
    return *this;
}

matrix& matrix::perspective(float fov, float aspect, float near, float far) {
    identity();
    
    float yScale = 1.0f / tan(fov / 2.0f * PI / 180.0f);
    float xScale = yScale / aspect;
    float frustumLen = far - near;
    
    mat[0 + 0 * 4] = xScale;
    mat[1 + 1 * 4] = yScale;
    
    mat[2 + 2 * 4] = -((far + near) / frustumLen);
    mat[3 + 2 * 4] = -1;
    
    mat[2 + 3 * 4] = -((2 * near * far) / frustumLen);
    mat[3 + 3 * 4] = 0;
    
    return *this;
}

matrix& matrix::orthographic(float left, float right, float bottom, float top, float near, float far) {
    identity();
    
    mat[0 + 0 * 4] = 2.0f / (right - left);
    mat[1 + 1 * 4] = 2.0f / (top - bottom);
    mat[2 + 2 * 4] = 2.0f / (near - far);
    
    mat[0 + 3 * 4] = (left + right) / (left - right);
    mat[1 + 3 * 4] = (bottom + top) / (bottom - top);
    mat[2 + 3 * 4] = (near + far) / (near - far);

    return *this;
}

matrix& matrix::translate(float dx, float dy, float dz) {
    mat[0 + 3 * 4] += dx;
    mat[1 + 3 * 4] += dy;
    mat[2 + 3 * 4] += dz;
    
    return *this;
}

matrix& matrix::scale(float sx, float sy, float sz) {
    matrix scaleMatrix;
    
    scaleMatrix.mat[0 + 0 * 4] = sx;
    scaleMatrix.mat[1 + 1 * 4] = sy;
    scaleMatrix.mat[2 + 2 * 4] = sz;
    
    return this->multiply(scaleMatrix);
}

matrix& matrix::rotate(float roll, float pitch, float yaw) {
    float x = mat[0 + 3 * 4], y = mat[1 + 3 * 4], z = mat[2 + 3 * 4];
    
    this->translate(-x, -y, -z); // move to origin for rotation
    
    if (roll != 0) {
        matrix rotationMatrix;
        
        float c = cos(roll);
        float s = sin(roll);
        rotationMatrix.mat[1 + 1 * 4] = c;
        rotationMatrix.mat[2 + 1 * 4] = -s;
        rotationMatrix.mat[1 + 2 * 4] = s;
        rotationMatrix.mat[2 + 2 * 4] = c;
        
        rotationMatrix.multiply(*this);
        (*this) = rotationMatrix;
    }
    if (pitch != 0) {
        matrix rotationMatrix;
        
        float c = cos(pitch);
        float s = sin(pitch);
        rotationMatrix.mat[0 + 0 * 4] = c;
        rotationMatrix.mat[2 + 0 * 4] = s;
        rotationMatrix.mat[0 + 2 * 4] = -s;
        rotationMatrix.mat[2 + 2 * 4] = c;
        
        rotationMatrix.multiply(*this);
        (*this) = rotationMatrix;
    }
    if (yaw != 0) {
        matrix rotationMatrix;
        
        float c = cos(yaw);
        float s = sin(yaw);
        rotationMatrix.mat[0 + 0 * 4] = c;
        rotationMatrix.mat[1 + 0 * 4] = -s;
        rotationMatrix.mat[0 + 1 * 4] = s;
        rotationMatrix.mat[1 + 1 * 4] = c;
        
        rotationMatrix.multiply(*this);
        (*this) = rotationMatrix;
    }
    
    this->translate(x, y, z); // translate back to original position
    
    return *this;
}

matrix& matrix::setPosition(float x, float y, float z) {
    mat[0 + 3 * 4] = x;
    mat[1 + 3 * 4] = y;
    mat[2 + 3 * 4] = z;
    
    return *this;
}

matrix& matrix::multiply(const matrix& other) {
    float result[4*4];
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            float sum = 0;
            for (int i = 0; i < 4; ++i)
                sum += mat[x + i * 4] * other.mat[i + y * 4];
            result[x + y * 4] = abs(sum) < 1e-15 ? 0 : sum;
        }
    }
    for (int i = 0; i < 16; ++i)
        mat[i] = result[i];
    
    return *this;
}

matrix& matrix::inverse() {
    float inv[16], det;
    int i;
    
    inv[0] = mat[5]  * mat[10] * mat[15] - 
             mat[5]  * mat[11] * mat[14] - 
             mat[9]  * mat[6]  * mat[15] + 
             mat[9]  * mat[7]  * mat[14] +
             mat[13] * mat[6]  * mat[11] - 
             mat[13] * mat[7]  * mat[10];

    inv[4] = -mat[4]  * mat[10] * mat[15] + 
              mat[4]  * mat[11] * mat[14] + 
              mat[8]  * mat[6]  * mat[15] - 
              mat[8]  * mat[7]  * mat[14] - 
              mat[12] * mat[6]  * mat[11] + 
              mat[12] * mat[7]  * mat[10];

    inv[8] = mat[4]  * mat[9]  * mat[15] - 
             mat[4]  * mat[11] * mat[13] - 
             mat[8]  * mat[5]  * mat[15] + 
             mat[8]  * mat[7]  * mat[13] + 
             mat[12] * mat[5]  * mat[11] - 
             mat[12] * mat[7]  * mat[9];

    inv[12] = -mat[4]  * mat[9]  * mat[14] + 
               mat[4]  * mat[10] * mat[13] +
               mat[8]  * mat[5]  * mat[14] - 
               mat[8]  * mat[6]  * mat[13] - 
               mat[12] * mat[5]  * mat[10] + 
               mat[12] * mat[6]  * mat[9];

    inv[1] = -mat[1]  * mat[10] * mat[15] + 
              mat[1]  * mat[11] * mat[14] + 
              mat[9]  * mat[2]  * mat[15] - 
              mat[9]  * mat[3]  * mat[14] - 
              mat[13] * mat[2]  * mat[11] + 
              mat[13] * mat[3]  * mat[10];

    inv[5] = mat[0]  * mat[10] * mat[15] - 
             mat[0]  * mat[11] * mat[14] - 
             mat[8]  * mat[2]  * mat[15] + 
             mat[8]  * mat[3]  * mat[14] + 
             mat[12] * mat[2]  * mat[11] - 
             mat[12] * mat[3]  * mat[10];

    inv[9] = -mat[0]  * mat[9]  * mat[15] + 
              mat[0]  * mat[11] * mat[13] + 
              mat[8]  * mat[1]  * mat[15] - 
              mat[8]  * mat[3]  * mat[13] - 
              mat[12] * mat[1]  * mat[11] + 
              mat[12] * mat[3]  * mat[9];

    inv[13] = mat[0]  * mat[9]  * mat[14] - 
              mat[0]  * mat[10] * mat[13] - 
              mat[8]  * mat[1]  * mat[14] + 
              mat[8]  * mat[2]  * mat[13] + 
              mat[12] * mat[1]  * mat[10] - 
              mat[12] * mat[2]  * mat[9];

    inv[2] = mat[1]  * mat[6] * mat[15] - 
             mat[1]  * mat[7] * mat[14] - 
             mat[5]  * mat[2] * mat[15] + 
             mat[5]  * mat[3] * mat[14] + 
             mat[13] * mat[2] * mat[7] - 
             mat[13] * mat[3] * mat[6];

    inv[6] = -mat[0]  * mat[6] * mat[15] + 
              mat[0]  * mat[7] * mat[14] + 
              mat[4]  * mat[2] * mat[15] - 
              mat[4]  * mat[3] * mat[14] - 
              mat[12] * mat[2] * mat[7] + 
              mat[12] * mat[3] * mat[6];

    inv[10] = mat[0]  * mat[5] * mat[15] - 
              mat[0]  * mat[7] * mat[13] - 
              mat[4]  * mat[1] * mat[15] + 
              mat[4]  * mat[3] * mat[13] + 
              mat[12] * mat[1] * mat[7] - 
              mat[12] * mat[3] * mat[5];

    inv[14] = -mat[0]  * mat[5] * mat[14] + 
               mat[0]  * mat[6] * mat[13] + 
               mat[4]  * mat[1] * mat[14] - 
               mat[4]  * mat[2] * mat[13] - 
               mat[12] * mat[1] * mat[6] + 
               mat[12] * mat[2] * mat[5];

    inv[3] = -mat[1] * mat[6] * mat[11] + 
              mat[1] * mat[7] * mat[10] + 
              mat[5] * mat[2] * mat[11] - 
              mat[5] * mat[3] * mat[10] - 
              mat[9] * mat[2] * mat[7] + 
              mat[9] * mat[3] * mat[6];

    inv[7] = mat[0] * mat[6] * mat[11] - 
             mat[0] * mat[7] * mat[10] - 
             mat[4] * mat[2] * mat[11] + 
             mat[4] * mat[3] * mat[10] + 
             mat[8] * mat[2] * mat[7] - 
             mat[8] * mat[3] * mat[6];

    inv[11] = -mat[0] * mat[5] * mat[11] + 
               mat[0] * mat[7] * mat[9] + 
               mat[4] * mat[1] * mat[11] - 
               mat[4] * mat[3] * mat[9] - 
               mat[8] * mat[1] * mat[7] + 
               mat[8] * mat[3] * mat[5];

    inv[15] = mat[0] * mat[5] * mat[10] - 
              mat[0] * mat[6] * mat[9] - 
              mat[4] * mat[1] * mat[10] + 
              mat[4] * mat[2] * mat[9] + 
              mat[8] * mat[1] * mat[6] - 
              mat[8] * mat[2] * mat[5];

    det = mat[0] * inv[0] + mat[1] * inv[4] + mat[2] * inv[8] + mat[3] * inv[12];

    if (det == 0)
        return *this;

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        mat[i] = inv[i] * det;

    return *this;
}

ostream& operator << (ostream& out, const matrix& m) {
    if (&out == &cout) {
        for (int x = 0; x < 4; ++x) {
            printf("[");
            for (int y = 0; y < 4; ++y)
                printf("%8.4f", m.mat[x + y * 4]);
            printf("]\n");
        }
    } else {
        for (int i = 0; i < 16; ++i)
            out << m.mat[i] << "\n";
    }
    
    return out;
}

istream& operator >> (istream& in, matrix& m) {
    if (&in == &cin) {
        for (int x = 0; x < 4; ++x) {
            for (int y = 0; y < 4; ++y) {
                printf("Enter value at [%i][%i] : ", x, y);
                in >> m.mat[x + y * 4];
            }
        }
    } else {
        for (int i = 0; i < 16; ++i)
            in >> m.mat[i];
    }
    
    return in;
}
