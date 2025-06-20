#include "renderer.hpp"
#include "System.hpp"

#include <string>

int main() 
{   
    System sys {};
    Scene scene { 100, 100 };
    sys.add_particle(1, { 0, 0 }, { 1, 0 });
    scene.circle_buffer.add_circle(0, 0, 0.5, sys.get_particles()[0]->get_id());

    sys.add_rigid_body(1, 1, {{ 1.0, 1.0 }}, 0, 1, {0, 0}, {0, 0});
    sys.add_rigid_body(3, 3, {{ 1.0, 1.0 }}, 0, 1, {3, 3}, {0, 0});


    int nsides = 6;
    std::vector<float> m_vertices {};
    m_vertices.push_back(0.0f);  
    m_vertices.push_back(0.0f);  
    m_vertices.push_back(0.0f);  
    m_vertices.push_back(1.0f);  
    m_vertices.push_back(1.0f);  
    m_vertices.push_back(1.0f);  

    for (int i = 0; i <= nsides; ++i) {
        float angle = 2.0f * M_PI * i / nsides;
        float x = std::cos(angle);
        float y = std::sin(angle);
        m_vertices.push_back(x);    
        m_vertices.push_back(y);
        m_vertices.push_back(0.0f);
        
        m_vertices.push_back(1.0f); 
        m_vertices.push_back(1.0f); 
        m_vertices.push_back(1.0f); 
    }

    scene.polygon_buffers.emplace_back(m_vertices);
    scene.polygon_buffers[0].add_polygon(0, 0, 0, 1, sys.get_rigid_bodies()[0]->get_id());
    scene.polygon_buffers[0].add_polygon(0, 0, 0, 1, sys.get_rigid_bodies()[1]->get_id());


    scene.animator.bind_physics_system(&sys);
    Renderer renderer {};
    renderer.render(&scene);

    return 0;
};