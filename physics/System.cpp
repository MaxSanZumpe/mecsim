#include "System.hpp"


void System::set_global_gravity_flag(bool flag)
{
    m_config.global_gravity_flag = flag;
}

void System::set_global_vdrag_flag(bool flag)
{
    m_config.global_viscous_drag_flag = flag;
}


void System::set_ode_solver(OdeSolverType type) 
{   
    m_solver = ode_solver_make_unique(type, this);
    if (!m_solver) {
        throw std::runtime_error("ERROR::SYSTEM::IN_MEMBER_FUNCTION:\nSET_ODE_SOLVER::SOLVER_TYPE_NOT_FOUND\n");
    }
    m_config.ode_solver_type = type;
}

void System::set_time_step(double time_step)
{
    if (time_step <= 0) {
        throw std::runtime_error("ERROR::SYSTEM::IN_MEMBER_FUNCTION:\nSET_TIME_STEP::NEGATIVE_TIME_STEP\n");
    }

    m_config.time_step = time_step;
}

void System::set_global_gravity_acceleration(double gravity_acceleration) 
{
    m_config.gravitational_g = gravity_acceleration;
}

void System::set_global_vdrag_constant(double vdrag_constant) 
{
    m_config.viscous_drag_coef = vdrag_constant;
}

void System::accumulate_time(double delta_time)
{
    m_time += delta_time;
}


void System::clear_forces_and_torques() 
{
    for (auto& p : m_particles) {
        p->force_accumulator.x = 0;
        p->force_accumulator.y = 0;
    }

    for (auto& b : m_rigid_bodies) {
        b->force_accumulator.x = 0;
        b->force_accumulator.y = 0;

        b->torque_accumulator = 0;
    }
}

void System::compute_forces_and_torques() 
{
    for (auto& f : m_forces) {
        f->apply_force();
    }
}

void System::step()
{   
    m_solver->step();
}


void System::add_particle(double mass, vector2&& position, vector2&& velocity)
{
    m_particles.emplace_back(
    std::make_unique<Particle>(mass, std::move(position), std::move(velocity)));
    m_particle_indices[m_particles.back()->get_id()] = m_particles.size() - 1;

    global_gravity->increase_max_nparticles();
    global_gravity->add_particle(m_particles.back().get());

    // global_viscous_drag->increase_max_nparticles();
    // global_viscous_drag->add_particle(m_particles.back());
}


void System::add_rigid_body(double mass, double inertia, std::vector<vector2>&& vertices, double angle, 
                            double angular_velocity, vector2&& position, 
                            vector2&& velocity)
{
    m_rigid_bodies.emplace_back(
    std::make_unique<RigidBody>(mass, inertia, std::move(vertices), angle, angular_velocity, std::move(position), 
                                std::move(velocity)));
    m_rigid_body_indices[m_rigid_bodies.back()->get_id()] = m_rigid_bodies.size() - 1;

    global_gravity->increase_max_nrigid_bodies();
    global_gravity->add_rigid_body(m_rigid_bodies.back().get());

    // global_viscous_drag->increase_max_nparticles();
    // global_viscous_drag->add_particle(m_particles.back());
}


void System::del_particle(std::string&& id)
{
    global_gravity->decrease_max_nparticles();
    global_gravity->del_particle(id);

    // global_viscous_drag->decrease_max_nparticles();
    // global_viscous_drag->del_particle(id);

    m_particles.erase(m_particles.begin() + m_particle_indices.at(id));

    for (auto& f : m_forces) {
        if (f->del_particle(id)) {
            del_force(f->get_id());
        }
    }

    size_t i = 0;
    m_particle_indices.clear();
    for (auto& p : m_particles) {
        m_particle_indices[p->get_id()] = i;
        ++i; 
    }
}


void System::del_rigid_body(std::string&& id)
{
    global_gravity->decrease_max_nrigid_bodies();
    global_gravity->del_rigid_body(id);

    // global_viscous_drag->decrease_max_nparticles();
    // global_viscous_drag->del_particle(id);

    m_rigid_bodies.erase(m_rigid_bodies.begin() + m_rigid_body_indices.at(id));

    for (auto& f : m_forces) {
        if (f->del_particle(id)) {
            del_force(f->get_id());
        }
    }

    size_t i = 0;
    m_rigid_body_indices.clear();
    for (auto& p : m_rigid_bodies) {
        m_rigid_body_indices[p->get_id()] = i;
        ++i; 
    }
}

void System::add_force(std::unique_ptr<ForceGenerator>&& force) 
{
    m_force_indices[force->get_id()] = m_forces.size();
    m_forces.push_back(std::move(force));
}

void System::del_force(std::string&& id) 
{
    m_forces.erase(m_forces.begin() + m_force_indices.at(id));

    size_t i = 0;
    m_force_indices.clear();
    for (auto& f : m_forces) {
        m_rigid_body_indices[f->get_id()] = i;
        ++i; 
    }
}


void System::print_config() 
{
    std::cout << "------------ SYSTEM CONFIG ------------\n";
    std::cout << std::fixed << std::setprecision(5) << std::boolalpha;
    std::cout << "  ODE solver    : " << G_ODE_SOLVER_STRINGS_MAP.at(m_config.ode_solver_type) << '\n';
    std::cout << "  Time step     : " << 1000*m_config.time_step << " ms\n";
    std::cout << "  Global Gravity: " << m_config.global_gravity_flag << " (" 
              << m_config.gravitational_g << " m/s^2)\n";
    std::cout << "---------------------------------------\n";
}

void System::print_rigid_body_info()
{   
    std::cout << "------------ RIGID BODIES -------------\n";
    for(auto& b : m_rigid_bodies) {
        std::cout << "  ID: " << b->get_id() << '\n';
        std::cout << "  Angle: " << b->angle << '\n';
        std::cout << "------------------------\n";
    }
    std::cout << "---------------------------------------\n";
}