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
    global_gravity->set_g(gravity_acceleration);
}

void System::set_global_vdrag_constant(double vdrag_constant) 
{
    m_config.viscous_drag_coef = vdrag_constant;
}


void System::fill_mass_buffer() 
{
    m_mass_buffer.resize(3 * m_bodies.size());

    size_t i = 0;
    for (auto& b : m_bodies) {
        m_mass_buffer[i++] = b->get_inverse_inertia();
        m_mass_buffer[i++] = b->get_inverse_mass();
        m_mass_buffer[i++] = b->get_inverse_mass();
    }
}


void System::fill_angular_and_linear_velocity_buffer()
{   
    m_angular_and_linear_velocity_buffer.resize(3 * m_bodies.size());

    size_t i = 0;
    for (auto& b : m_bodies) {
        m_angular_and_linear_velocity_buffer[i++] = b->angular_velocity;
        m_angular_and_linear_velocity_buffer[i++] = b->velocity.x;
        m_angular_and_linear_velocity_buffer[i++] = b->velocity.y;
    }
}


void System::fill_torque_and_force_buffer() 
{
    m_torque_and_force_buffer.resize(3 * m_bodies.size());

    size_t i = 0;
    for (auto& b : m_bodies) {
        m_torque_and_force_buffer[i++] = b->torque_accumulator;
        m_torque_and_force_buffer[i++] = b->force_accumulator.x;
        m_torque_and_force_buffer[i++] = b->force_accumulator.y;
    }
}


void System::accumulate_time(double delta_time)
{
    m_time += delta_time;
}

void System::backtrack_time(double delta_time) 
{
    m_time -= delta_time;
}

void System::compute_constraints()
{
    size_t np = m_bodies.size();
    size_t nc = m_constraints.size();

    SparseBlockMatrix jacobian { nc, np , 3 };
    SparseBlockMatrix jaco_dot { nc, np , 3 };

    m_constraint_buffer.resize(nc);
    m_constr_dot_buffer.resize(nc);

    fill_mass_buffer();
    fill_angular_and_linear_velocity_buffer();
    fill_torque_and_force_buffer(); 

    if(m_constraints.empty()) {
        return;
    }

    size_t i = 0;
    for (auto& constraint : m_constraints) 
    {   
        m_constraint_buffer[i++] = constraint->evaluate_constraint();
        auto blocks = constraint->jacobian_blocks(m_body_indices, m_constraint_indices);

        jacobian.add_block_row(std::move(blocks.first));
        jaco_dot.add_block_row(std::move(blocks.second));
    }

    jacobian.validate();
    jaco_dot.validate();

    auto jacobian_func = [jacobian](const std::vector<double> inverse_mass,
                                    const std::vector<double>& v) -> std::vector<double>
    {
        std::vector<double> result (jacobian.dimensions().second);
        result = inverse_mass * transpose_sparse_mult(jacobian, v);

        return sparse_mult(jacobian, result);
    };

    std::vector<double> mass_buffer = m_mass_buffer;
    auto double_jacobian_func = [jacobian, mass_buffer](const std::vector<double>& v) -> std::vector<double>
    {
        std::vector<double> result(jacobian.dimensions().second);
        std::vector<double> result2(jacobian.dimensions().first);

        result = mass_buffer * transpose_sparse_mult(jacobian, v);

        result2 = sparse_mult(jacobian, result);

        result = mass_buffer * transpose_sparse_mult(jacobian, result2);

        result2 = sparse_mult(jacobian, result);

        return result2;
    };

    std::vector<double> b (nc, 0.0);
    b = -1*sparse_mult(jaco_dot, m_angular_and_linear_velocity_buffer) 
        -sparse_mult(jacobian, m_mass_buffer * m_torque_and_force_buffer)
        -m_config.ks * sparse_mult(jacobian, m_angular_and_linear_velocity_buffer)
        -m_config.kd * m_constraint_buffer;

    b = jacobian_func(m_mass_buffer, b);

    ConjugateGradSleSolver le_solver { double_jacobian_func, std::move(b), nc };

    std::vector<double> solution (nc);
    solution = le_solver.solve();

    solution = transpose_sparse_mult(jacobian, solution);
    
    i = 0;
    for (auto& p : m_bodies) {
        p->torque_accumulator  += solution[i++];
        p->force_accumulator.x += solution[i++];
        p->force_accumulator.y += solution[i++];
    }
}


void System::clear_forces_and_torques() 
{
    for (auto& b : m_bodies) {
        b->torque_accumulator = 0;
        b->force_accumulator.x = 0;
        b->force_accumulator.y = 0;
    }
}

void System::compute_forces_and_torques() 
{   
    if (m_config.global_gravity_flag) {
        global_gravity->apply_force();
    }

    for (auto& f : m_forces) {
        f->apply_force();
    }

    //compute_constraints();
}

double System::step()
{   
    double time_step = m_config.time_step;
    m_solver->step(time_step);
    bool penetration = detect_collisions(m_bodies, m_contacts, m_config.penetration_threshhold);
    
    size_t i = 0;
    while (penetration) {
        m_solver->backtrack(time_step);
        time_step = time_step / 2;
        m_solver->step(time_step);
        penetration = detect_collisions(m_bodies, m_contacts, m_config.penetration_threshhold);
        ++i;
    }

    if (m_contacts.empty()) {
        return time_step; 
    } 

    for (auto contact : m_contacts) {
        resolve_contact(contact);
    }

    return time_step;
}


double System::compute_angular_momentum()
{   
    double angular_momentum = 0;

    for (auto& b : m_bodies) {
        if (b->type == DYNAMIC_BODY) {
            angular_momentum += cross2d(b->position, 1/b->get_inverse_mass() * b->velocity);
        }
        if (b->type == ROTATIONAL_ONLY) {
            angular_momentum += 1/b->get_inverse_inertia() * b->angular_velocity; 
        }
    }

    return angular_momentum;
}


RigidBody* System::add_dynamic_body(double mass, std::vector<vector2>&& vertices, double angle, 
                            double angular_velocity, vector2& position, 
                            vector2& velocity)
{   
    RigidBodyType type = DYNAMIC_BODY;
    m_bodies.emplace_back(
    std::make_unique<RigidBody>(mass, std::move(vertices), angle, angular_velocity, position, 
                                velocity, type));
    m_body_indices[m_bodies.back()->get_id()] = m_bodies.size() - 1;

    global_gravity->increase_max_nbodies();
    global_gravity->add_body(m_bodies.back().get());

    // global_viscous_drag->increase_max_nparticles();
    // global_viscous_drag->add_particle(m_particles.back());

    return m_bodies.back().get();
}


RigidBody* System::add_rotational_body(double mass, std::vector<vector2>&& vertices, double angle, 
                               double angular_velocity, vector2& position) 
{
    RigidBodyType type = ROTATIONAL_ONLY;
    vector2 velocity { 0, 0 };
    m_bodies.emplace_back(
    std::make_unique<RigidBody>(mass, std::move(vertices), angle, angular_velocity, position, 
                                velocity, type));
    m_body_indices[m_bodies.back()->get_id()] = m_bodies.size() - 1;

    return m_bodies.back().get();
}


RigidBody* System::add_static_body(std::vector<vector2>&& vertices, double angle, vector2& position)
{   
    RigidBodyType type = RigidBodyType::STATIC_BODY;
    vector2 velocity { 0, 0 };
    m_bodies.emplace_back(
    std::make_unique<RigidBody>(1, std::move(vertices), angle, 0, position, velocity, type));
    m_body_indices[m_bodies.back()->get_id()] = m_bodies.size() - 1;

    return m_bodies.back().get();
}


SpringGenerator* System::add_spring_connector(RigidBody* body1, RigidBody* body2, AnchorType anchor1, 
                                    AnchorType anchor2, double spring_constant, double spring_length) 
{
    m_forces.emplace_back(
        std::make_unique<SpringGenerator>(std::pair { body1, body2 }, spring_length, spring_constant, 
                                          anchor1, anchor2)
    );

    m_force_indices[m_forces.back()->get_id()] = m_forces.size() - 1;
    return dynamic_cast<SpringGenerator*>(m_forces.back().get());
}





void System::del_rigid_body(std::string&& id)
{
    global_gravity->decrease_max_nbodies();
    global_gravity->del_body(id);

    m_bodies.erase(m_bodies.begin() + m_body_indices.at(id));

    for (auto& f : m_forces) {
        if (f->del_body(id)) {
            del_force(f->get_id());
        }
    }

    size_t i = 0;
    m_body_indices.clear();
    for (auto& p : m_bodies) {
        m_body_indices[p->get_id()] = i;
        ++i; 
    }
}


void System::del_force(std::string&& id) 
{
    m_forces.erase(m_forces.begin() + m_force_indices.at(id));

    size_t i = 0;
    m_force_indices.clear();
    for (auto& f : m_forces) {
        m_body_indices[f->get_id()] = i;
        ++i; 
    }
}



void System::add_constraint(std::unique_ptr<Constraint>&& constraint)
{
    m_constraint_indices[constraint->get_id()] = m_constraints.size();
    m_constraints.push_back(std::move(constraint));
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
    for(auto& b : m_bodies) {
        std::cout << "  ID: " << b->get_id() << '\n';
        // std::cout << "  Mass: " << b->get_mass() << '\n';
        // std::cout << "  Moment: " << b->get_inertia() << '\n';
        std::cout << "  Angle: "  << b->angle << '\n';
        std::cout << "  Angular velocity: " << b->angular_velocity << '\n';
        std::cout << "  Position: (" << b->position.x << ", "<< b->position.y <<  ")\n";
        std::cout << "  Velocity: (" << b->velocity.x << ", "<< b->velocity.y <<  ")\n";
        std::cout << "  Force: (" << b->force_accumulator.x << ", " << b->force_accumulator.y << ")\n";
        std::cout << "  Torque: " << b->torque_accumulator << '\n';
        
        std::cout << "  Body vertices:\n";
        for (auto& v : b->get_vertices()) {
            std::cout << "    (" << v.x << ", " << v.y << ")\n";
        }

        std::cout << "------------------------\n";
    }
    std::cout << "---------------------------------------\n\n";
}


// void System::print_forces_info()
// {
//     std::cout << "------------ FORCES ----------------\n";
//     for(auto& f : m_forces) {
//         std::cout << "  ID: " << f->get_id() << '\n';
//         std::cout << "  Type"
//         std::cout << "  Force: (" << f->force_accumulator.x << ", " << f->force_accumulator.y << ")\n";
//         std::cout << "------------------------\n";
//     }
//     std::cout << "---------------------------------------\n\n";
// }