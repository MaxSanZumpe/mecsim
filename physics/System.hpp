#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <vector>
#include <memory>
#include <iomanip>
#include <iostream>
#include <unordered_map>


#include "Particle.hpp"
#include "RigidBody.hpp"
#include "OdeSolver.hpp"
#include "ForceGenerator.hpp"


struct SystemConfig
{
    double gravitational_g { 9.81 };
    bool global_gravity_flag { true };

    double viscous_drag_coef { 0.01 }; 
    bool global_viscous_drag_flag { false };

    double time_step { 0.016667/20 };
    OdeSolverType ode_solver_type { OdeSolverType::FORWARD_EULER };
};


class System
{   
    private:
        double m_time { 0 }; 
        SystemConfig m_config {};


        std::vector<std::unique_ptr<Particle>>  m_anchors {};
        std::vector<std::unique_ptr<Particle>>  m_particles {};
        std::vector<std::unique_ptr<RigidBody>> m_rigid_bodies {};

        std::unordered_map<std::string, size_t> m_anchor_indices {};
        std::unordered_map<std::string, size_t> m_particle_indices {};
        std::unordered_map<std::string, size_t> m_rigid_body_indices {};

        std::vector<std::unique_ptr<ForceGenerator>> m_forces {};

        std::unordered_map<std::string, size_t> m_force_indices {};

        std::unique_ptr<GravityGenerator> global_gravity = 
        std::make_unique<GravityGenerator>(m_particles, m_rigid_bodies, m_config.gravitational_g);
        
        // std::unique_ptr<VDragGenerator> global_viscous_drag = 
        // std::make_unique<VDragGenerator>(m_particles, m_config.viscous_drag_coef);
        
        std::unique_ptr<OdeSolver> m_solver { std::make_unique<ForwardEuler>(this) };

    public: 
        static constexpr uint8_t dimension { 2 };

        System() = default;
        System(SystemConfig&& configuration) : m_config(std::move(configuration)) {}

        double get_time() const { return m_time; }
        const SystemConfig& get_config() const { return m_config; }

        const std::vector<std::unique_ptr<Particle>>&  get_particles()    const { return m_particles; }
        const std::vector<std::unique_ptr<RigidBody>>& get_rigid_bodies() const { return m_rigid_bodies; }

        const std::unordered_map<std::string, size_t>& get_particle_indices() const { return m_particle_indices; }
        const std::unordered_map<std::string, size_t>& get_rigid_body_indices() const { return m_rigid_body_indices; }

        
        void set_ode_solver(OdeSolverType type);
        void set_time_step(double time_step);

        void set_global_gravity_flag(bool flag);
        void set_global_vdrag_flag(bool flag);

        void set_global_gravity_acceleration(double gravity_acceleration);
        void set_global_vdrag_constant(double vdrag_constant);

        void accumulate_time(double delta_time);

        void clear_forces_and_torques();
        void compute_forces_and_torques();

        void step();

        // void add_anchor(vector2&& position);
        // void del_anchor(std::string&& id);
        
        void add_particle(double mass, vector2&& position, vector2&& velocity);
        void del_particle(std::string&& id);

        void add_rigid_body(double mass, double inertia, std::vector<vector2>&& vertices, double angle, 
                           double angular_velocity, vector2&& position, vector2&& velocity);
        void del_rigid_body(std::string&& id);

        void add_force(std::unique_ptr<ForceGenerator>&& force);
        void del_force(std::string&& id);

        void print_config();
        void print_rigid_body_info();
};

#endif