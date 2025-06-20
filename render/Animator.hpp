#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include <glm/glm.hpp>
#include "System.hpp"

class Scene;

class Animator 
{
    private:
        System* m_sys {};
        Scene*  m_scene {};

        double time_step {};
        double time_accumulator {};

        void update_physics(double delta_time) 
        {       
                int counter = 0;
                time_accumulator += delta_time;
                while (time_accumulator >= time_step) {
                    m_sys->step();
                    time_accumulator -= time_step;
                    ++counter;
                }
        }

        void update_buffers();

    public: 
        Animator(Scene* scene) : m_scene(scene) {}

        void bind_physics_system(System* system) 
        {
            m_sys = system;
            time_step = system->get_config().time_step;
        }

        void update_scene(double delta_time) 
        {   
            if (m_sys) {
                update_physics(delta_time);
                update_buffers();
            }
        }
};

#endif