#ifndef SCENE_HPP
#define SCENE_HPP

#include "shapes.hpp"
#include "ShapeBuffer.hpp"

#include <memory>

struct Scene
{
    unsigned int m_world_width {};
    unsigned int m_world_height {};

    ShaderManager shader_manager {};

    std::vector<std::unique_ptr<ShapeBuffer>> buffers {};
    std::unordered_map<std::string, size_t> buffer_indices {};

    Scene(const unsigned int world_width, const unsigned int world_height): 
    m_world_width(world_width), m_world_height(world_height) 
    {   
        buffers.emplace_back(std::make_unique<SpringSpriteBuffer>());
        buffer_indices[SHAPE_TYPE_STRINGS[ShapeType::SPRING]] = buffers.size() - 1;
        
        buffers.emplace_back(std::make_unique<CircleBuffer>());
        buffer_indices[SHAPE_TYPE_STRINGS[ShapeType::ELIPSE]] = buffers.size() - 1;

        buffers.emplace_back(std::make_unique<RectangleBuffer>());
        buffer_indices[SHAPE_TYPE_STRINGS[ShapeType::RECTANGLE]] = buffers.size() - 1;
    }

    void bind_vaos()
    {
        for (auto& b : buffers) {
            b->bind_vao();
        }
    }

    void bind_shaders()
    {
        for (auto& b : buffers) {
            b->bind_shader(shader_manager.get_shader(b->vertex_path, b->fragment_path));
        }
    }

    void draw() 
    {
        for (auto& b : buffers) {
            b->draw();
        }
    }

    void set_circle_edges(size_t num_edges)
    {
        size_t index = buffer_indices[SHAPE_TYPE_STRINGS[ShapeType::ELIPSE]];

        CircleBuffer* ptr = dynamic_cast<CircleBuffer*>(buffers[index].get());
        ptr->set_num_vertices(num_edges);
    }

    Shape* add_shape(float xscaling, float yscaling, float angle, 
                      float xposition, float yposition, ShapeType type, std::string&& id)
    {
        size_t index = buffer_indices[SHAPE_TYPE_STRINGS[type]];

        glm::vec2 position { xposition, yposition };
        std::unique_ptr<Shape> shape = std::make_unique<Shape>(xscaling, yscaling, angle, position, id);

        Shape* ptr = shape.get();

        buffers[index]->add_shape(std::move(shape));

        return ptr;
    }   
};


#endif