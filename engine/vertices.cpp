#include "vertices.hpp"
#include "parameters.hpp"


vertices elipse_vertices(ShapeParameters* parameters)
{   
    parameters = dynamic_cast<ElipseParameters*>(parameters);
    std::vector<vector2> vertices;
    size_t num_vertices = 51;
    vertices.reserve(num_vertices);

    double angle_step = -2.0 * M_PI / num_vertices;
    for (size_t i = 0; i < num_vertices; ++i) {
        double angle = i * angle_step;
        float x = static_cast<float>(parameters->xscaling * cos(angle));
        float y = static_cast<float>(parameters->yscaling * sin(angle));
        vertices.emplace_back(vector2 { x, y }); 
    }

    return vertices;
}


vertices rectangle_vertices(ShapeParameters* parameters)
{
    parameters = dynamic_cast<RectangleParameters*>(parameters);
    std::vector<vector2> vertices;

    vertices.push_back(vector2 { -parameters->xscaling/2, -parameters->yscaling/2 });
    vertices.push_back(vector2 { -parameters->xscaling/2,  parameters->yscaling/2 });
    vertices.push_back(vector2 {  parameters->xscaling/2,  parameters->yscaling/2 });
    vertices.push_back(vector2 {  parameters->xscaling/2, -parameters->yscaling/2 });

    return vertices;
}