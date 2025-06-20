#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "vector2.hpp"

class Particle {
    private:
        double m_mass { 1 };

        std::string m_id { generate_id() };    
        inline static int m_instance_id { 0 }; 
        
        static std::string generate_id() 
        {
            return "pid" + std::to_string(m_instance_id++);
        }

    public: 
        vector2 position {};
        vector2 velocity {};
        
        vector2 force_accumulator {};

        Particle() = default;
        Particle(double mass, vector2&& position, vector2&& velocity) : 
        m_mass(mass), position(std::move(position)), velocity(std::move(velocity))
        {
            if (mass <= 0) {
                std::cerr << "ERROR::RIGID_BODY_2D::CONSTRUCTOR::MASS_MUST_BE_POSITVE\n";
            }
        }   

        double get_mass() const { return m_mass; }
        std::string get_id() const { return m_id; }
};

#endif // PARTICLE_HPP
