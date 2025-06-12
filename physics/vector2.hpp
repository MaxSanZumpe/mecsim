#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>
#include <iostream>

struct vector2 
{
    double x {};
    double y {};
    
    void reset() { x = 0; y = 0; }
    double norm() const { return std::sqrt(x*x + y*y); }
};


vector2 operator+(const vector2& v, const vector2& w)
{
    return vector2 { v.x + w.x, v.y + w.y };
}

vector2 operator-(const vector2& v, const vector2& w)
{
    return vector2 { v.x - w.x, v.y - w.y };
}

vector2& operator+=(vector2& v, const vector2& w)
{
    v.x += w.x;
    v.y += w.y;
    return v;
}

double operator*(const vector2& v, const vector2& w)
{
    return v.x * w.x + v.y * w.y;
}

template <typename T>
vector2 operator*(T a, const vector2& v)
{
    return vector2 { a * v.x, a * v.y };
}

template <typename T>
vector2 operator*(const vector2& v, T a)
{
    return a * v;
}

template <typename T>
vector2 operator/(const vector2& v, T a) 
{   
    if (a == 0) {
        std::cerr << "ERROR::VECTOR2::DIVISION_BY_ZERO\n";
        return vector2 {};
    } else {
        return vector2 { v.x/a, v.y/a };
    }
}

#endif