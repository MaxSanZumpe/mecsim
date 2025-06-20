#include "ForceGenerator.hpp"

void ForceGenerator::add_particle(Particle* particle)
{   
    if (m_particles.size() < m_max_nparticles) {
        m_particles.push_back(particle);
    } else {
        throw std::runtime_error("ERROR::FORCE_GENRATOR::MAX_CAPACITY\n");
    }
}


bool GravityGenerator::del_particle(std::string_view id)
{
    for (auto it = m_particles.begin(); it != m_particles.end(); ++it) {
        if ((*it)->get_id() == id)
        {
            m_particles.erase(it);
            break;
        }
    }

    return false;
}

void ForceGenerator::add_rigid_body(RigidBody* particle)
{   
    if (m_rigid_bodies.size() < m_max_nrigid_bodies) {
        m_rigid_bodies.push_back(particle);
    } else {
        throw std::runtime_error("ERROR::FORCE_GENRATOR::MAX_CAPACITY\n");
    }
}


bool GravityGenerator::del_rigid_body(std::string_view id)
{
    for (auto it = m_rigid_bodies.begin(); it != m_rigid_bodies.end(); ++it) {
        if ((*it)->get_id() == id)
        {
            m_rigid_bodies.erase(it);
            break;
        }
    }

    return false;
}


void GravityGenerator::apply_force() const
{
    for (auto& p : m_particles) {
        double mass = p->get_mass();
        p->force_accumulator +=  vector2 {0.0, -m_g * mass};
    }

    for(auto& b : m_rigid_bodies) {
        double mass = b->get_mass();
        b->force_accumulator +=  vector2 {0.0, -m_g * mass};
    }
}