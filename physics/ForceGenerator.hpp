#ifndef FORCES_HPP
#define FORCES_HPP

#include <string>
#include <memory>
#include <vector>
#include <string_view>
#include <unordered_map>

#include "vector2.hpp"
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

        size_t m_max_nbodies {};
        std::vector<RigidBody*> m_bodies {};

        static std::string generate_force_id()
        {
            return "fid" + std::to_string(s_instance_counter++);
        }

        ForceGenerator() = default;
        
    public:
        virtual ~ForceGenerator() = default;

        ForceGeneratorType get_type() const { return m_type; }
        std::string        get_id()   const { return m_id; }
    
        const std::vector<RigidBody*>& get_bodies() const { return m_bodies; }

        void increase_max_nbodies() { ++m_max_nbodies; }
        void decrease_max_nbodies() { --m_max_nbodies; }

        void         add_body(RigidBody* body);
        virtual bool del_body(std::string_view id) = 0;
        
        virtual void apply_force() const = 0;
        virtual double compute_energy() const = 0;
};


class GravityGenerator : public ForceGenerator
{   
    private:
        double m_g {};

    public:
        GravityGenerator(const std::vector<std::unique_ptr<RigidBody>>& bodies, double g): m_g(g)
        { 
            m_type = GLOBAL_GRAVITY;

            m_max_nbodies = bodies.size();
            for (const auto& b : bodies) {
                m_bodies.emplace_back(b.get());
            }
        }

        void set_g(double g) { m_g = g; }

        bool del_body(std::string_view id) override;

        void apply_force() const override;
        double compute_energy() const  override;
};


class SpringGenerator : public ForceGenerator
{
    private: 
        RigidBody* m_b1 {};
        RigidBody* m_b2 {};

        double m_spring_length {};
        double m_spring_constant {};

    public: 
        AnchorType anchor1 {};
        AnchorType anchor2 {};

        SpringGenerator(std::pair<RigidBody*, RigidBody*>&& bodies, double spring_length, double spring_constant, 
        AnchorType _anchor1, AnchorType _anchor2) : m_b1(bodies.first), m_b2(bodies.second), m_spring_length(spring_length), 
        m_spring_constant(spring_constant), anchor1(_anchor1), anchor2(_anchor2)
        {   
            m_type = SPRING_CONNECTOR;
            m_max_nbodies = 2;
            m_bodies.push_back(bodies.first);
            m_bodies.push_back(bodies.second);

            if (spring_length < 0) {
                throw std::runtime_error("ERROR::SPRING_GENERATOR::SPRING_LENGTH_NEGATIVE\n");
            }

            if (spring_constant <= 0) {
                throw std::runtime_error("ERROR::SPRING_GENERATOR::SPRING_CONSTANT_NON_POSITIVE\n");
            }
        }
        
        bool del_body(std::string_view id) override;

        void apply_force() const override;
        double compute_energy() const override;
};

#endif