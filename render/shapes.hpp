#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <string>
#include <string_view>

enum ShapeType 
{
    ELIPSE,
    RECTANGLE,
    SPRING,
    NUM_SHAPE_TYPES
};

constexpr std::array<std::string, NUM_SHAPE_TYPES> SHAPE_TYPE_STRINGS {
    "elipse",
    "rectangle",
    "spring",
};

struct Shape 
{ 
    std::string id {};
    glm::mat4 model { 1.0f };
    glm::mat4 translation { 1.0f };
    glm::mat4 rotation { 1.0f };
    glm::mat4 scaling { 1.0f };

    Shape(float xscaling, float yscaling, float angle, glm::vec2 position, std::string_view id) : id(id)
    {
        glm::mat4 ident { 1.0f };
        scaling     = glm::scale(ident, glm::vec3 { xscaling, yscaling, 0.0f });
        rotation    = glm::rotate(ident, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        translation = glm::translate(ident, glm::vec3{ position, 0.0f });
        model       = translation * rotation * scaling;
    }


    Shape(glm::vec2 position, float radius, std::string_view id) : id(id)
    {   
        glm::mat4 ident { 1.0f };
        scaling = glm::scale(ident, glm::vec3 { radius, radius, 0.0f });
        model = glm::translate(ident, glm::vec3{ position, 0.0f });
        model = model * scaling;
    }


    Shape(glm::vec2 start, glm::vec2 end, std::string_view id) : id(id)
    {   
        float length = glm::length(end- start);
        float angle = atan2(end.y - start.y, end.x - start.x);

        model = glm::translate(glm::mat4 { 1.0f }, glm::vec3{ start, 0.0f });
        model = glm::rotate(model, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        model = glm::scale(model, glm::vec3 { length, 1.0f, 1.0f });
    }
};

#endif

