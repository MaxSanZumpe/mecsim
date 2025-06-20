#include "System.hpp"
#include "OdeSolver.hpp"


void OdeSolver::fill_particle_state_buffer()
{   
    size_t i = 0;
    m_particle_state_buffer.resize(4 * m_sys->get_particles().size());
    for (auto& p : m_sys->get_particles()) {
        m_particle_state_buffer[i++] = p->position.x;
        m_particle_state_buffer[i++] = p->position.y;
        m_particle_state_buffer[i++] = p->velocity.x;
        m_particle_state_buffer[i++] = p->velocity.y;
    }
}

void OdeSolver::fill_rigid_body_state_buffer()
{   
    size_t i = 0;
    m_particle_state_buffer.resize(6 * m_sys->get_rigid_bodies().size());
    for (auto& b : m_sys->get_rigid_bodies()) {
        m_particle_state_buffer[i++] = b->position.x;
        m_particle_state_buffer[i++] = b->position.y;
        m_particle_state_buffer[i++] = b->angle;
        m_particle_state_buffer[i++] = b->velocity.x;
        m_particle_state_buffer[i++] = b->velocity.y;
        m_particle_state_buffer[i++] = b->angular_velocity;
    }
}

void OdeSolver::scatter_particle_state_buffer()
{   
    size_t i = 0;
    for (auto& p : m_sys->get_particles()) {
        p->position.x = m_particle_state_buffer[i++];
        p->position.y = m_particle_state_buffer[i++];
        p->velocity.x = m_particle_state_buffer[i++];
        p->velocity.y = m_particle_state_buffer[i++];
    }
}

void OdeSolver::scatter_rigid_body_state_buffer()
{   
    size_t i = 0;
    m_particle_state_buffer.resize(6 * m_sys->get_particles().size());
    for (auto& b : m_sys->get_rigid_bodies()) {
        b->position.x       = m_particle_state_buffer[i++];
        b->position.y       = m_particle_state_buffer[i++];
        b->angle            = m_particle_state_buffer[i++];
        b->velocity.x       = m_particle_state_buffer[i++];
        b->velocity.y       = m_particle_state_buffer[i++];
        b->angular_velocity = m_particle_state_buffer[i++];
    }
}

void ForwardEuler::step()
{
    //m_sys->clear_forces_and_torques();
    //m_sys->compute_forces_and_torques();
    double h = m_sys->get_config().time_step;
    
    for (auto& p : m_sys->get_particles())
    {
        p->position += h * p->velocity;
        p->velocity += h * p->force_accumulator / p->get_mass();;
    }

    for (auto& b : m_sys->get_rigid_bodies())
    {
        b->angle            += h * b->angular_velocity;
        b->angular_velocity += h * b->torque_accumulator / b->get_inertia();

        b->position += h * b->velocity;
        b->velocity += h * b->force_accumulator / b->get_mass();
    }
    
    m_sys->accumulate_time(h);
}

// void ImprovedEuler::step()
// {   
//     double h = m_sys->get_config().time_step;
    
//     m_sys->clear_forces();
//     m_sys->compute_forces();

//     get_phase_state();

//     // Stage 1
//     for (auto& p : m_sys->get_particles())
//     {
//         p->position += h * p->velocity;      
//         p->velocity += h * p->accumulator / p->get_mass(); 
//     }
    
//     m_sys->time += h;
//     m_sys->compute_forces();
//     scatter_phase_state();

//     // Stage 2
//     for (auto& p : m_sys->get_particles())
//     {
//         p->velocity += h/2 * p->accumulator / p->get_mass(); 
//         p->position += h/2 * p->velocity;      
//     }
// }

// void RungeKutta4::step()
// {
//     double h = m_sys->get_config().time_step;
    
//     // Stage 1
//     m_sys->clear_forces();
//     m_sys->compute_forces();
    
//     get_phase_state();
//     fill_position_buffer();
//     fill_velocity_buffer();

//     size_t i = 0, j = 0, k = 0;
//     for (auto& p : m_sys->get_particles()) {
//         float m = p->get_mass();
//         m_state_buffer[i++] += h/6 * p->velocity.x;
//         m_state_buffer[i++] += h/6 * p->velocity.y;

//         m_state_buffer[i++] += h/6 * p->accumulator.x / m; 
//         m_state_buffer[i++] += h/6 * p->accumulator.y / m; 

//         p->position.x = m_position_buffer[j++] + h/2 * p->velocity.x; 
//         p->position.y = m_position_buffer[j++] + h/2 * p->velocity.y; 

//         p->velocity.x = m_velocity_buffer[k++] + h/2 * p->accumulator.x / m;
//         p->velocity.y = m_velocity_buffer[k++] + h/2 * p->accumulator.y / m;
//     }

//     //Stage 2
//     m_sys->time += h/2;
//     m_sys->clear_forces();
//     m_sys->compute_forces();

//     i = 0, j = 0, k = 0;
//     for (auto& p : m_sys->get_particles()) {
//         float m = p->get_mass();
//         m_state_buffer[i++] += h/3 * p->velocity.x;
//         m_state_buffer[i++] += h/3 * p->velocity.y;

//         m_state_buffer[i++] += h/3 * p->accumulator.x / m; 
//         m_state_buffer[i++] += h/3 * p->accumulator.y / m;

//         p->position.x = m_position_buffer[j++] + h/2 * p->velocity.x; 
//         p->position.y = m_position_buffer[j++] + h/2 * p->velocity.y;  

//         p->velocity.x = m_velocity_buffer[k++] + h/2 * p->accumulator.x / m;
//         p->velocity.y = m_velocity_buffer[k++] + h/2 * p->accumulator.y / m;
//     }

//     // Stage 3
//     m_sys->clear_forces();
//     m_sys->compute_forces();

//     i = 0, j = 0, k = 0;
//     for (auto& p : m_sys->get_particles()) {    
//         float m = p->get_mass();
//         m_state_buffer[i++] += h/3 * p->velocity.x;
//         m_state_buffer[i++] += h/3 * p->velocity.y;

//         m_state_buffer[i++] += h/3 * p->accumulator.x / m; 
//         m_state_buffer[i++] += h/3 * p->accumulator.y / m;


//         p->position.x = m_position_buffer[j++] + h/2 * p->velocity.x; 
//         p->position.y = m_position_buffer[j++] + h/2 * p->velocity.y;

//         p->velocity.x = m_velocity_buffer[k++] + h/2 * p->accumulator.x / m;
//         p->velocity.y = m_velocity_buffer[k++] + h/2 * p->accumulator.y / m;
//     }

//     // Stage 4
//     m_sys->time += h/2;
//     m_sys->clear_forces();
//     m_sys->compute_forces();

//     i = 0;
//     for (auto& p : m_sys->get_particles()) {    
//         float m = p->get_mass();
//         m_state_buffer[i++] += h/6 * p->velocity.x;
//         m_state_buffer[i++] += h/6 * p->velocity.y;

//         m_state_buffer[i++] += h/6 * p->accumulator.x / m; 
//         m_state_buffer[i++] += h/6 * p->accumulator.y / m;
//     }

//     scatter_phase_state();
// }


// void LeapFrog::step() 
// {
//     double h = m_sys->get_config().time_step;

//     m_sys->clear_forces();
//     m_sys->compute_forces();

//     for (auto& p : m_sys->get_particles()) {
//         float m = p->get_mass();
//         p->velocity += h/2 * p->accumulator / m;
//         p->position += h   * p->velocity; 
//     }

//     m_sys->time += h;
//     m_sys->clear_forces();
//     m_sys->compute_forces();

//     for (auto& p : m_sys->get_particles()) {
//         float m = p->get_mass();
//         p->velocity += h/2 * p->accumulator / m;
//     }
// }


std::unique_ptr<OdeSolver> ode_solver_make_unique(OdeSolverType type, System* sys) 
{
    switch (type)
    {   
        case OdeSolverType::UNDEFINED_ODE:  return nullptr;
        case OdeSolverType::FORWARD_EULER:  return std::make_unique<ForwardEuler>(sys);
        // case OdeSolverType::IMPROVED_EULER: return std::make_unique<ImprovedEuler>(sys);
        // case OdeSolverType::RUNGE_KUTTA4:   return std::make_unique<RungeKutta4>(sys);
        // case OdeSolverType::LEAPFROG:       return std::make_unique<LeapFrog>(sys);
    }

    return nullptr;
}