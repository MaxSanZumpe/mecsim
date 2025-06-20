#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <ranges>
#include <string>
#include "shader.hpp"
#include "vao.hpp"


struct Circle
{
    std::string id { "Not asigned" };
    glm::mat4 model { 1.0f };

    glm::mat4 scaling { 1.0f };

    Circle(glm::vec2 position, float radius, std::string_view id) : id(id)
    {   
        glm::mat4 ident { 1.0f };
        scaling = glm::scale(ident, glm::vec3 { radius, radius, 0.0f });
        model = glm::translate(ident, glm::vec3{ position, 0.0f });
        model = model * scaling;
    }

    void update_model(glm::vec2& position)
    {   
        model = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ position, 0.0f });
        model = model * scaling;
    }
};

struct Polygon
{
    std::string id { "Not asigned" };
    glm::mat4 model { 1.0f };
    glm::mat4 scaling { 1.0f };

    Polygon(glm::vec2 position, float angle, float radius, std::string_view id) : id(id)
    {   
        glm::mat4 ident { 1.0f };
        scaling  = glm::scale(ident, glm::vec3 { radius, radius, 1.0f });
        glm::mat4 rotation = glm::rotate(ident, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        glm::mat4 translation = glm::translate(ident, glm::vec3{ position, 0.0f });
        model = translation * rotation * scaling;
    }

    void update_model(glm::vec2& position, float angle)
    {   
        glm::mat4 ident { 1.0f };
        glm::mat4 rotation = glm::rotate(ident, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        glm::mat4 translation = glm::translate(ident, glm::vec3{ position, 0.0f });
        model = translation * rotation * scaling;
    }
};

struct Line2d
{
    std::string ID { "Not asigned" };
    glm::mat4 model { 1.0f };

    Line2d(glm::vec2 p1, glm::vec2 p2, std::string_view id) : ID(id)
    {   
        float length = glm::length(p2 - p1);
        float angle = atan2(p2.y - p1.y, p2.x - p1.x);

        model = glm::translate(glm::mat4 { 1.0f }, glm::vec3{ p1, 0.0f });
        model = glm::rotate(model, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        model = glm::scale(model, glm::vec3 { length, 1.0f, 1.0f });
    }

    void update_model(glm::vec2 p1, glm::vec2 p2)
    {   
        float length = glm::length(p2 - p1);
        float angle = atan2(p2.y - p1.y, p2.x - p1.x);

        model    = glm::translate(glm::mat4 { 1.0f }, glm::vec3{ p1, 0.0f });
        model    = glm::rotate(model, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        model    = glm::scale(model, glm::vec3 { length, 1.0f, 1.0f });
    }
};


struct SpringSprite
{
    std::string ID { "Not asigned" };
    glm::mat4 model { 1.0f };

    SpringSprite(glm::vec2 p1, glm::vec2 p2, std::string_view id) : ID(id)
    {   
        float length = glm::length(p2 - p1);
        float angle = atan2(p2.y - p1.y, p2.x - p1.x);

        model    = glm::translate(glm::mat4 { 1.0f }, glm::vec3{ p1, 0.0f });
        model    = glm::rotate(model, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        model    = glm::scale(model, glm::vec3 { length, 1.0f, 1.0f });
    }

    void update_model(glm::vec2 p1, glm::vec2 p2)
    {   
        float length = glm::length(p2 - p1);
        float angle = atan2(p2.y - p1.y, p2.x - p1.x);

        model    = glm::translate(glm::mat4 { 1.0f }, glm::vec3{ p1, 0.0f });
        model    = glm::rotate(model, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
        model    = glm::scale(model, glm::vec3 { length, 1.0f, 1.0f });
    }
};


#endif

