#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <vector>
#include <memory>
#include <iomanip>
#include <iostream>
#include <unordered_map>

#include "util.hpp"
#include "sparse.hpp"
#include "sparse.hpp"
#include "RigidBody.hpp"
#include "OdeSolver.hpp"
#include "collisions.hpp"
#include "Constraint.hpp"
#include "ForceGenerator.hpp"
#include "LinearSolver.hpp"


struct SystemConfig
{
    double gravitational_g { 9.81 };
    bool global_gravity_flag { true };

    double viscous_drag_coef { 0.1 }; 
    bool global_viscous_drag_flag { false };

    double time_step { 0.0167/10 };
    OdeSolverType ode_solver_type { OdeSolverType::LEAPFROG };

    double penetration_threshhold { 0.01 };

    float xi = 1.0;
    float N = 30;
    double stabilization_freq = 2*M_PI/(N * time_step);
    double ks { 2*xi*stabilization_freq }; 
    double kd { stabilization_freq * stabilization_freq };
};


class System
{   
    private:
        double m_time { 0 };
        SystemConfig m_config {};
        double m_variable_step { m_config.time_step };

        std::vector<Contact> m_contacts {};

        std::vector<std::unique_ptr<RigidBody>> m_anchors {};
        std::unordered_map<std::string, size_t> m_anchor_indices {};

        std::vector<std::unique_ptr<RigidBody>> m_bodies {};
        std::unordered_map<std::string, size_t> m_body_indices {};

        std::vector<std::unique_ptr<ForceGenerator>> m_forces {};
        std::unordered_map<std::string, size_t> m_force_indices {};

        std::vector<std::unique_ptr<Constraint>> m_constraints {};
        std::unordered_map<std::string, size_t> m_constraint_indices {};

        std::unique_ptr<GravityGenerator> global_gravity = 
        std::make_unique<GravityGenerator>(m_bodies, m_config.gravitational_g);
        
        // std::unique_ptr<VDragGenerator> global_viscous_drag = 
        // std::make_unique<VDragGenerator>(m_particles, m_config.viscous_drag_coef);

        std::vector<double> m_mass_buffer {};
        std::vector<double> m_constraint_buffer {};
        std::vector<double> m_constr_dot_buffer {};

        std::vector<double> m_angular_and_linear_velocity_buffer {};
        std::vector<double> m_torque_and_force_buffer {};

        void fill_mass_buffer();
        void fill_angular_and_linear_velocity_buffer();
        void fill_torque_and_force_buffer();
        
        std::unique_ptr<OdeSolver> m_solver { std::make_unique<LeapFrog>(this) };
        
        public: 
        static constexpr uint8_t dimension { 2 };
        
        System() = default;
        System(SystemConfig&& configuration) : m_config(std::move(configuration)) {}
        
        double get_time() const { return m_time; }
        const SystemConfig& get_config() const { return m_config; }
        
        const std::vector<std::unique_ptr<RigidBody>>& get_rigid_bodies() const { return m_bodies; }
        const std::unordered_map<std::string, size_t>& get_rigid_body_indices() const { return m_body_indices; }
        
        const std::vector<std::unique_ptr<RigidBody>>& get_anchors() const { return m_anchors; }
        const std::unordered_map<std::string, size_t>& get_anchor_indices() const { return m_anchor_indices; }
        
        const std::vector<std::unique_ptr<ForceGenerator>>& get_forces()  const { return m_forces; }
        const std::unordered_map<std::string, size_t> get_force_indices() const { return m_force_indices; }
        
        
        void set_ode_solver(OdeSolverType type);
        void set_time_step(double time_step);
        
        void set_global_gravity_flag(bool flag);
        void set_global_vdrag_flag(bool flag);
        
        void set_global_gravity_acceleration(double gravity_acceleration);
        void set_global_vdrag_constant(double vdrag_constant);
        
        
        void accumulate_time(double delta_time);
        void backtrack_time(double delta_time);
        
        
        void clear_forces_and_torques();
        void compute_forces_and_torques();
        void compute_constraints();
        
        double compute_angular_momentum();
        

        double step();
        
        RigidBody* add_dynamic_body(double mass, std::vector<vector2>&& vertices, double angle, 
                           double angular_velocity, vector2& position, vector2& velocity);

        RigidBody* add_rotational_body(double mass, std::vector<vector2>&& vertices, double angle, 
                                       double angular_velocity, vector2& position);
                           
        RigidBody* add_static_body(std::vector<vector2>&& vertices, double angle, vector2& position);

        SpringGenerator* add_spring_connector(RigidBody* body1, RigidBody* body2, AnchorType anchor1, 
                                              AnchorType anchor2, double spring_constant, double spring_length);
                           

        void del_rigid_body(std::string&& id);

        void del_force(std::string&& id);

        void add_constraint(std::unique_ptr<Constraint>&& constraint);

        void print_config();
        void print_rigid_body_info();

        //void print_force_info();
};

#endif