#include "Animator.hpp"
#include "scene.hpp"

void Animator::update_buffers()
{   
    for (auto& p : m_scene->circle_buffer.circles) {
                    glm::vec2 pos { m_sys->get_particles()[m_sys->get_particle_indices().at(p.id)]->position.x, 
                                    m_sys->get_particles()[m_sys->get_particle_indices().at(p.id)]->position.y };
                    p.update_model(pos);
    }

    for (auto& b : m_scene->polygon_buffers) {
        for (auto& p : b.polygons) {
            float angle = m_sys->get_rigid_bodies()[m_sys->get_rigid_body_indices().at(p.id)]->angle;
            glm::vec2 pos { m_sys->get_rigid_bodies()[m_sys->get_rigid_body_indices().at(p.id)]->position.x, 
                            m_sys->get_rigid_bodies()[m_sys->get_rigid_body_indices().at(p.id)]->position.y };
            p.update_model(pos, angle);
        }
    }
}
    