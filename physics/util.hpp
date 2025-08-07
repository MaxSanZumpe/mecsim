#ifndef UTIL_HPP
#define UTIL_HPP

#include <cmath>
#include <vector>
#include <ranges>
#include <algorithm>

#include "vector2.hpp"

inline double radians(double angle_in_degrees) 
{   
    double angle = angle_in_degrees * M_PI/180;
    int n = angle / (2 * M_PI);

    if (abs(n) >= 1) {
        angle = angle - 2 * n * M_PI;
    }

    return angle;
}


inline bool is_convex(const vector2* vertices, size_t size)
{
    double sign = 0;

    for (size_t i = 0; i < size; ++i) {
        vector2 a = vertices[i];
        vector2 b = vertices[(i + 1) % size];
        vector2 c = vertices[(i + 2) % size];

        vector2 ab = b - a;
        vector2 bc = c - b;

        double cross = cross2d(ab, bc);

        if (sign == 0)
            sign = cross;
        else if ((sign > 0) != (cross > 0))
            return false;
    }

    return true;
}


inline vector2 compute_centroid(vector2* vertices, size_t size) {
    double A = 0.0;
    double Cx = 0.0;
    double Cy = 0.0;

    for (size_t i = 0; i < size; ++i) {
        size_t j = (i + 1) % size;
        double cross = vertices[i].x * vertices[j].y - vertices[j].x * vertices[i].y;
        A += cross;
        Cx += (vertices[i].x + vertices[j].x) * cross;
        Cy += (vertices[i].y + vertices[j].y) * cross;
    }

    A  *= 0.5;
    Cx /= (6.0 * A);
    Cy /= (6.0 * A);

    vector2 centroid = { Cx, Cy };
    return centroid;
}


inline double moment_of_inertia_per_unit_mass_about_origin(const vector2* vertices, size_t size) {
    double inertia = 0.0;
    double area = 0.0;

    for (size_t i = 0; i < size; ++i) {
        size_t j = (i + 1) % size;

        double xi = vertices[i].x;
        double yi = vertices[i].y;
        double xj = vertices[j].x;
        double yj = vertices[j].y;

        double cross = xi * yj - xj * yi;

        double term = xi*xi + xi*xj + xj*xj + yi*yi + yi*yj + yj*yj;

        inertia += cross * term;
        area += cross;
    }

    area = 0.5 * area;
    inertia = fabs(inertia) / 12.0;

    return inertia / fabs(area);
}


inline void center_vertices_to_centroid(std::vector<vector2>& vertices) 
{
    vector2 centroid = compute_centroid(vertices.data(), vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i] = vertices[i] - centroid;
    }
}


inline void sort_vertices_by_angle(std::vector<vector2>& vertices) {
    std::sort(vertices.begin(), vertices.end(), [](const vector2& a, const vector2& b) {
        double angle_a = std::atan2(a.y, a.x);
        double angle_b = std::atan2(b.y, b.x);
        return angle_a < angle_b;
    });
}


inline double distance_to_edge(double angle, const vector2* vertices, size_t size) 
{    
    vector2 dir = { std::cos(angle), std::sin(angle) };

    for (size_t i = 0; i < size; ++i) {
        vector2 p1 = vertices[i];
        vector2 p2 = vertices[(i + 1) % size];
        vector2 edge = p2 - p1;

        double det = cross2d(dir, edge);
        if (std::abs(det) < 1e-8) continue; 

        vector2 diff = p1;
        double t = cross2d(diff, edge) / det;
        double s = cross2d(diff, dir) / det;

        if (t > 0 && s >= 0 && s <= 1) {
            return t;
        }
    }

    throw std::runtime_error("Ray does not intersect polygon");
}

std::vector<double>  operator*(double a, const std::vector<double>& v);
std::vector<double>  operator*(const std::vector<float>& v, const std::vector<double>& w);
std::vector<double>  operator*(const std::vector<double>& v, const std::vector<double>& w);
std::vector<double>  operator-(const std::vector<double>& v, const std::vector<double>& w);
std::vector<double>  operator+(const std::vector<double>& v, const std::vector<double>& w);
std::vector<double>& operator+=(std::vector<double>& v, const std::vector<double>& w);
std::vector<double>& operator-=(std::vector<double>& v, const std::vector<double>& w);

double dot(const std::vector<double>& v, const std::vector<double>& w);
double norm(const std::vector<double>& v);


#endif