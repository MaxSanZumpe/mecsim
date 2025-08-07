#include "ForceGenerator.hpp"


void ForceGenerator::add_body(RigidBody* body)
{   
    if (m_bodies.size() < m_max_nbodies) {
        m_bodies.emplace_back(body);
    } else {
        throw std::runtime_error("ERROR::FORCE_GENRATOR::MAX_CAPACITY\n");
    }
}


bool GravityGenerator::del_body(std::string_view id)
{
    for (auto it = m_bodies.begin(); it != m_bodies.end(); ++it) {
        if ((*it)->get_id() == id)
        {
            m_bodies.erase(it);
            break;
        }
    }

    return false;
}


void GravityGenerator::apply_force() const
{
    for(auto& b : m_bodies) {
        double mass = 1/b->get_inverse_mass();
        b->force_accumulator +=  vector2 {0.0, -m_g * mass};
    }
}


double GravityGenerator::compute_energy() const 
{
    double energy = 0;
    for(auto& b : m_bodies) {
        energy += 1/b->get_inverse_mass() * m_g * b->position.y;
    }

    return energy;
}


bool SpringGenerator::del_body(std::string_view id) 
{   
    for (auto it = m_bodies.begin(); it != m_bodies.end(); ++it) {
        if ((*it)->get_id() == id)
        {
            m_bodies.erase(it);
            return true;
        }
    }

    return false; 
}


void SpringGenerator::apply_force() const
{   
    vector2 anchor_pos1 = m_b1->get_anchor_position(anchor1);
    vector2 anchor_pos2 = m_b2->get_anchor_position(anchor2);
    
    vector2 delta = anchor_pos2 - anchor_pos1;
    
    double dist = delta.norm();
    vector2 force_dir = delta / dist;
    double stretch = dist - m_spring_length;
    vector2 force = m_spring_constant * stretch * force_dir;

    m_b1->force_accumulator += force;
    m_b2->force_accumulator += -force;

    m_b1->torque_accumulator += cross2d(anchor_pos1 - m_b1->position, force);
    m_b2->torque_accumulator += cross2d(anchor_pos2 - m_b2->position, -force);
}


double SpringGenerator::compute_energy() const 
{
    return 0 ;
}