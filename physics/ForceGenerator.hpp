#ifndef FORCES_HPP
#define FORCES_HPP

#include <string>
#include <memory>
#include <vector>
#include <string_view>
#include <unordered_map>

#include "vector2.hpp"
#include "Particle.hpp"
#include "RigidBody.hpp"


enum ForceGeneratorType
{
    GLOBAL_GRAVITY,
    GLOBAL_VISCOUS_DRAG,
    SPRING_CONNECTOR,
    NUM_FORCE_TYPES,
};


const std::unordered_map<ForceGeneratorType, std::string> G_FORCE_GENERATOR_STRINGS_MAP 
{
    { GLOBAL_GRAVITY,      "Global gravity" },
    { SPRING_CONNECTOR,    "Spring-connector" },
    { GLOBAL_VISCOUS_DRAG, "Global viscous drag" },
};


class ForceGenerator
{
    protected:
        ForceGeneratorType m_type {};

        static inline int s_instance_counter {}; 
        const std::string m_id { generate_force_id() };

        size_t m_max_nparticles {};
        size_t m_max_nrigid_bodies {};
        std::vector<Particle*>  m_particles {};
        std::vector<RigidBody*> m_rigid_bodies {};

        static std::string generate_force_id()
        {
            return "fid" + std::to_string(s_instance_counter++);
        }

        ForceGenerator() = default;
        
    public:
        virtual ~ForceGenerator() = default;

        ForceGeneratorType get_type() const { return m_type; }
        std::string        get_id()   const { return m_id; }
        const std::vector<Particle*>&  get_particles() const { return m_particles; }
        const std::vector<RigidBody*>& get_rigid_bodies() const { return m_rigid_bodies; }

        void increase_max_nparticles() { ++m_max_nparticles; }
        void decrease_max_nparticles() { --m_max_nparticles; }

        void increase_max_nrigid_bodies() { ++m_max_nrigid_bodies; }
        void decrease_max_nrigid_bodies() { --m_max_nrigid_bodies; }


        void         add_particle(Particle* particle);
        virtual bool del_particle(std::string_view id) = 0;

        void         add_rigid_body(RigidBody* rigid_body);
        virtual bool del_rigid_body(std::string_view id) = 0;
        
        virtual void apply_force() const = 0;
        //virtual double compute_energy() const = 0;
};


class GravityGenerator : public ForceGenerator
{   
    private:
        double m_g {};

    public:
        GravityGenerator(const std::vector<std::unique_ptr<Particle>>& particles,
                         const std::vector<std::unique_ptr<RigidBody>>& rigid_bodies, double g): m_g(g)
        { 
            m_type = GLOBAL_GRAVITY;
            m_max_nparticles = particles.size();
            for (const auto& p : particles) {
                m_particles.push_back(p.get());
            }

            m_max_nparticles = rigid_bodies.size();
            for (const auto& b : rigid_bodies) {
                m_rigid_bodies.push_back(b.get());
            }
        }

        void set_g(double g) 
        {
            m_g = g;
        }

        bool del_particle(std::string_view id) override;
        bool del_rigid_body(std::string_view id) override;

        void apply_force() const override;
        //double compute_energy() const  override;
};


#endif