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

inline vector2 operator-(const vector2& v) 
{
    return vector2 { -v.x, -v.y};
}

inline vector2 operator+(const vector2& v, const vector2& w)
{
    return vector2 { v.x + w.x, v.y + w.y };
}

inline vector2 operator-(const vector2& v, const vector2& w)
{
    return vector2 { v.x - w.x, v.y - w.y };
}

inline vector2& operator+=(vector2& v, const vector2& w)
{
    v.x += w.x;
    v.y += w.y;
    return v;
}

inline double operator*(const vector2& v, const vector2& w)
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


inline vector2 normalize(const vector2& v) 
{
    return v/v.norm();
}


inline vector2 perpendicular(const vector2& v)
{   
    return vector2 { -v.y, v.x };
}


inline double cross2d(const vector2& v, const vector2& w) 
{
    return v.x * w.y - v.y * w.x;
}


inline vector2 direction(double angle) 
{
    return vector2 { cos(angle), sin(angle) };
}


inline vector2 rotate(const vector2& v, double angle) 
{
    return {v.x * cos(angle)- v.y * sin(angle), v.x * sin(angle) + v.y * cos(angle)};
}

#endif