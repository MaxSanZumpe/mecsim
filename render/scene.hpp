#ifndef SCENE_HPP
#define SCENE_HPP

#include "shapes.hpp"
#include "ShapeBuffer.hpp"
#include "Animator.hpp"

struct Scene
{
    unsigned int m_world_width {};
    unsigned int m_world_height {};

    Animator animator { this };

    CircleBuffer circle_buffer {};
    std::vector<PolygonBuffer> polygon_buffers {};

    Scene(const unsigned int world_width, const unsigned int world_height): 
    m_world_width(world_width), m_world_height(world_height) {}

    void bind_vaos()
    {
        circle_buffer.bind_vao();
        for (auto& b : polygon_buffers) {
            b.bind_vao();
        }
    }

    void bind_shaders(ShaderManager& sman)
    {
        circle_buffer.bind_shader(sman.get_shader(circle_buffer.vertex_path, circle_buffer.fragment_path));
        for (auto& b : polygon_buffers) {
            b.bind_shader(sman.get_shader(b.vertex_path, b.fragment_path));
        }
    }

    void draw() 
    {
        circle_buffer.draw();
        for (auto& b : polygon_buffers) {
            b.draw();
        }
    }
};


#endif