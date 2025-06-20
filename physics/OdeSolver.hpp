#ifndef ODE_SOLVER_HPP
#define ODE_SOLVER_HPP

#include <vector>
#include <memory>
#include <assert.h>


enum OdeSolverType  
{   
    UNDEFINED_ODE,
    FORWARD_EULER,
    IMPROVED_EULER,
    RUNGE_KUTTA4,
    LEAPFROG,
};


const std::unordered_map<OdeSolverType, std::string> G_ODE_SOLVER_STRINGS_MAP 
{
    { FORWARD_EULER,  "Forward Euler" },
    { IMPROVED_EULER, "Improved Euler" },
    { RUNGE_KUTTA4,   "Runge Kutta 4" },
};



class System;

class OdeSolver
{   
    protected:
        System* m_sys { nullptr };

        std::vector<double> m_position_buffer {};
        std::vector<double> m_velocity_buffer {};
        std::vector<double> m_angular_velocity_buffer{};
        
        std::vector<double> m_particle_state_buffer {};
        std::vector<double> m_rigid_body_state_buffer {};
        
        void fill_particle_state_buffer();
        void fill_rigid_body_state_buffer();

        void scatter_particle_state_buffer();
        void scatter_rigid_body_state_buffer();

        // void fill_particle_position_buffer();
        // void fill_particle_velocity_buffer();

        // void fill_rigid_body_position_buffer();
        // void fill_rigid_body_velocity_buffer();
        // void fill_angular_velocity_buffer();

    public:
        OdeSolver(System* sys) : m_sys(sys) {} 
        virtual ~OdeSolver() = default;
        virtual void step() = 0;
};


class ForwardEuler : public OdeSolver
{
    public:
        ForwardEuler(System* sys) : OdeSolver(sys) {}
        void step() override;
};


// class ImprovedEuler : public OdeSolver
// {
//     public:
//         ImprovedEuler(System* sys) : OdeSolver(sys) {}
//         void step() override;
// };


// class RungeKutta4 : public OdeSolver
// {
//     public:
//         RungeKutta4(System* sys) : OdeSolver(sys) {}
//         void step() override;
// };


// class LeapFrog : public OdeSolver
// {
//     public:
//         LeapFrog(System* sys) : OdeSolver(sys) {}
//         void step() override;
// };


std::unique_ptr<OdeSolver> ode_solver_make_unique(OdeSolverType type, System* sys); 

#endif 