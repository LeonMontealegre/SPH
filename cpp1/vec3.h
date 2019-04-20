#ifndef VEC_3_H
#define VEC_3_H

#include <iostream>

class vec3 {
public:
    typedef float size_t;

    size_t x, y, z;

    // CONSTRUCTORS, ASSIGNMENT OPERATOR
    vec3(size_t a = 0);
    vec3(size_t x, size_t y, size_t z = 0);
    vec3(const vec3& other);
    vec3& operator = (const vec3& other);

    // OPERATORS
    vec3 operator + (const vec3& other) const;
    vec3 operator - (const vec3& other) const;
    vec3 operator * (const vec3& other) const;
    vec3 operator / (const vec3& other) const;

    vec3 operator + (float a) const;
    vec3 operator - (float a) const;
    vec3 operator * (float s) const;
    vec3 operator / (float s) const;

    size_t dot(const vec3& other) const;

    // METHODS
    vec3 norm() const;
    size_t len() const;
    size_t len2() const;

    // OTHER
    std::ostream& operator << (std::ostream& out) const {
        return out << "(" << this->x << ", " << this->y << ", " << this->z << ")";
    }

private:
    void copy(const vec3& other);
};


// --------------------------------------------------------
// IMPLEMENTATION
// --------------------------------------------------------


//
// CONSTRUCTORS, ASSIGNMENT OPERATOR
//
vec3::vec3(size_t a) {
    this->x = this->y = this->z = a;
}
vec3::vec3(size_t x, size_t y, size_t z) {
    this->x = x;
    this->y = y;
    this->z = z;
}
vec3::vec3(const vec3& other) {
    copy(other);
}

vec3& vec3::operator = (const vec3& other) {
    copy(other);
    return *this;
}


// OPERATORS
vec3 vec3::operator + (const vec3& other) const {
    return vec3(this->x+other.x, this->y+other.y, this->z+other.z);
}
vec3 vec3::operator - (const vec3& other) const {
    return vec3(this->x-other.x, this->y-other.y, this->z-other.z);
}
vec3 vec3::operator * (const vec3& other) const {
    return vec3(this->x*other.x, this->y*other.y, this->z*other.z);
}
vec3 vec3::operator / (const vec3& other) const{
    return vec3(this->x/other.x, this->y/other.y, this->z/other.z);
}

vec3 vec3::operator + (float a) const {
    return vec3(this->x+a, this->y+a, this->z+a);
}
vec3 vec3::operator - (float a) const {
    return vec3(this->x-a, this->y-a, this->z-a);
}
vec3 vec3::operator * (float s) const {
    return vec3(this->x*s, this->y*s, this->z*s);
}
vec3 vec3::operator / (float s) const {
    return vec3(this->x/s, this->y/s, this->z/s);
}

size_t vec3::dot(const vec3& other) const {
    return this->x*other.x + this->y*other.y + this->z*other.z;
}


//
// PRIVATE FUNCTIONS
//
void vec3::copy(const vec3& other) {
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
}


#endif
