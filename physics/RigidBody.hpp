#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

#include <vector>
#include <memory>

#include "vector2.hpp"
#include "Particle.hpp"


class RigidBody {
    private:
        double m_mass { 1 };
        double m_inertia { 1 };
        
        std::vector<vector2> m_vertices {};

        std::string m_id { generate_id() };    
        inline static int m_instance_id { 0 }; 
        
        static std::string generate_id() 
        {
            return "rbid" + std::to_string(m_instance_id++);
        }
    
    public:
        double angle {};
        double angular_velocity{};

        vector2 position {};
        vector2 velocity {};

        vector2 force_accumulator {};
        double torque_accumulator {};
        
        RigidBody(double mass, double inertia, std::vector<vector2>&& vertices, double angle, double angular_velocity, 
                  vector2&& position, vector2&& velocity) : m_mass(mass), m_inertia(inertia),  
                  m_vertices(std::move(vertices)), angle(angle), angular_velocity(angular_velocity),
                  position(std::move(position)), velocity(std::move(velocity))
        {
            if (mass <= 0) {
                std::cerr << "ERROR::RIGID_BODY_2D::CONSTRUCTOR::NEGATIVE_MASS\n";
            }

            if (inertia <= 0) {
                std::cerr << "ERROR::RIGID_BODY_2D::CONSTRUCTOR::NEGATIVE_MOMENT_OF_INERTIA\n";
            }
        }

        double get_mass() const { return m_mass; }
        double get_inertia() const { return m_inertia; }
        std::string get_id() const { return m_id; }
};

#endif