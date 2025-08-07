#pragma once

#include <vector>

#include <array>
#include "vector2.hpp"


enum VertexFunction
{
    ELIPSE_VERTICES,
    RECTANGLE_VERTICES,
    NUM_VERTEX_FUNCTIONS,
};

typedef std::vector<vector2> vertices;

class ShapeParameters;
vertices elipse_vertices(ShapeParameters* parameters);
vertices rectangle_vertices(ShapeParameters* parameters);


constexpr std::array<vertices (*)(ShapeParameters*), NUM_VERTEX_FUNCTIONS> VERTEX_FUNCTIONS {
    elipse_vertices,
    rectangle_vertices,
}; 