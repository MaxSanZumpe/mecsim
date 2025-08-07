#ifndef SIM_OBJECT
#define SIM_OBJECT

#include "shapes.hpp" 
#include "RigidBody.hpp"


class SimObject
{    
    public:
        SimObject() = default;
        
        virtual void update_drawable() = 0;
};


class Body : public SimObject
{   
    private:
        Shape* m_shape {};
        RigidBody* m_body{};

    public:
        Body(Shape* shape, RigidBody* body) : m_shape(shape), m_body(body) {}

        void update_drawable() override
        {
            float angle = m_body->angle;
            glm::vec2 position { m_body->position.x, m_body->position.y };
            
            glm::mat4 ident { 1.0f };
            glm::mat4 rotation = glm::rotate(ident, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
            glm::mat4 translation = glm::translate(ident, glm::vec3{ position, 0.0f });

            m_shape->model = translation * rotation * m_shape->scaling;
        }
};


class Spring : public SimObject
{
    private: 
        Shape* m_shape {};
        SpringGenerator* m_force {};

    public:
        Spring(Shape* shape, SpringGenerator* force) : m_shape(shape), m_force(force) {}

        void update_drawable() override 
        {
            glm::vec2 pos1 { m_force->get_bodies()[0]->get_anchor_position(m_force->anchor1).x, 
                             m_force->get_bodies()[0]->get_anchor_position(m_force->anchor1).y };
            glm::vec2 pos2 { m_force->get_bodies()[1]->get_anchor_position(m_force->anchor2).x, 
                             m_force->get_bodies()[1]->get_anchor_position(m_force->anchor2).y };

            float length = glm::length(pos2 - pos1);
            float angle = atan2(pos2.y - pos1.y, pos2.x - pos1.x);
            
            m_shape->model = glm::translate(glm::mat4 { 1.0f }, glm::vec3{ pos1, 0.0f });
            m_shape->model = glm::rotate(m_shape->model, angle, glm::vec3 { 0.0f, 0.0f, 1.0f });
            m_shape->model = glm::scale(m_shape->model, glm::vec3 { length, 1.0f, 1.0f });
        }
};  

#endif