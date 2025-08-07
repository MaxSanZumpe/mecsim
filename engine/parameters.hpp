#pragma once

#include "vertices.hpp"
#include "shapes.hpp"


class ShapeParameters 
{   
    public:
        ShapeType shape_type {};
        VertexFunction vertex_function {};

        double xscaling {};
        double yscaling {};

        ShapeParameters() = default;

        virtual void f() = 0;
};


class ElipseParameters : public ShapeParameters
{   
    public:
        ElipseParameters(double xaxis, double yaxis)
        {
            shape_type = ELIPSE;
            vertex_function = ELIPSE_VERTICES;
            
            xscaling = xaxis;
            yscaling = yaxis;
        }

    private: 
        void f() override {};
};


class RectangleParameters : public ShapeParameters
{   
    public:
        RectangleParameters(double xlength, double ylength)
        {
            shape_type = RECTANGLE;
            vertex_function = RECTANGLE_VERTICES;
            
            xscaling = xlength;
            yscaling = ylength;
        }

    private: 
        void f() override {};
};