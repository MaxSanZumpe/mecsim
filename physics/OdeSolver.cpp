#include "System.hpp"
#include "OdeSolver.hpp"


void OdeSolver::fill_body_state_buffer()
{   
    size_t i = 0;
    m_body_state_buffer.resize(6 * m_sys->get_rigid_bodies().size());
    for (auto& b : m_sys->get_rigid_bodies()) {
        m_body_state_buffer[i++] = b->angle;
        m_body_state_buffer[i++] = b->angular_velocity;
        m_body_state_buffer[i++] = b->position.x;
        m_body_state_buffer[i++] = b->position.y;
        m_body_state_buffer[i++] = b->velocity.x;
        m_body_state_buffer[i++] = b->velocity.y;
    }
}


void OdeSolver::scatter_body_state_buffer()
{   
    size_t i = 0;
    for (auto& b : m_sys->get_rigid_bodies()) {
        b->angle            = m_body_state_buffer[i++];
        b->angular_velocity = m_body_state_buffer[i++];
        b->position.x       = m_body_state_buffer[i++];
        b->position.y       = m_body_state_buffer[i++];
        b->velocity.x       = m_body_state_buffer[i++];
        b->velocity.y       = m_body_state_buffer[i++];
    }
}


void OdeSolver::backtrack(double time_step)
{
    scatter_body_state_buffer();
    m_sys->backtrack_time(time_step);
}


void ForwardEuler::step(double time_step)
{   
    fill_body_state_buffer();
    m_sys->clear_forces_and_torques();
    m_sys->compute_forces_and_torques();
    double h = time_step;

    for (auto& b : m_sys->get_rigid_bodies())
    {
        b->angle            += h * b->angular_velocity;
        b->angular_velocity += h * b->torque_accumulator * b->get_inverse_inertia();
        
        b->position += h * b->velocity;
        b->velocity += h * b->force_accumulator * b->get_inverse_mass();

        b->update_polygon_features();
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


void LeapFrog::step(double time_step) 
{   
    fill_body_state_buffer();
    double h = time_step;

    m_sys->clear_forces_and_torques();
    m_sys->compute_forces_and_torques();

    for (auto& b : m_sys->get_rigid_bodies()) {
        b->velocity += 0.5 * h * b->force_accumulator * b->get_inverse_mass();
        b->position += h * b->velocity;
    
        b->angular_velocity += 0.5 * h * b->torque_accumulator * b->get_inverse_inertia();
        b->angle    += h * b->angular_velocity;
    }

    m_sys->accumulate_time(h);
    m_sys->clear_forces_and_torques();
    m_sys->compute_forces_and_torques();

    for (auto& b : m_sys->get_rigid_bodies()) {
        b->velocity         += 0.5 * h * b->force_accumulator  * b->get_inverse_mass();
        b->angular_velocity += 0.5 * h * b->torque_accumulator * b->get_inverse_inertia();
        b->update_polygon_features();
    }
}


std::unique_ptr<OdeSolver> ode_solver_make_unique(OdeSolverType type, System* sys) 
{
    switch (type)
    {   
        case OdeSolverType::UNDEFINED_ODE:  return nullptr;
        case OdeSolverType::FORWARD_EULER:  return std::make_unique<ForwardEuler>(sys);
        case OdeSolverType::IMPROVED_EULER: return nullptr;
        case OdeSolverType::RUNGE_KUTTA4:   return nullptr;
        case OdeSolverType::LEAPFROG:       return std::make_unique<LeapFrog>(sys);
    }

    return nullptr;
}