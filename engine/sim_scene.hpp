#ifndef SIM_SCENE
#define SIM_SCENE

#include "scene.hpp"
#include "System.hpp"
#include "vector2.hpp"
#include "sim_object.hpp"
#include "vertices.hpp"

#include "shader.hpp"
#include "glfw_window.hpp"
#include "parameters.hpp"

class SimScene
{
    private: 
        std::vector<std::unique_ptr<SimObject>> m_objects {};

        size_t elipse_num_vertices = 53;

        double speed_factor = 2;
        
    public:
        System physics {};
        Scene  render { 100, 100 };

        SimScene() = default;

        void update_objects()
        {
            for (auto& o : m_objects) {
                o->update_drawable();
            }
        }

        void draw()
        {
            GLFW glwf { 3, 3 };
            Window window = Window { 1000, 700, "MecSim" };
            window.activate();
            window.bind_camera(render.m_world_width);

            render.set_circle_edges(elipse_num_vertices);

            window.set_background_color(0.1f, 0.1f, 0.1f, 1.0f);
            render.bind_vaos();
            render.bind_shaders();

            double time_accumulator {};
            float deltaTime {};
            FpsTimer timer {};

            while (!window.shouldClose())
            {   
                deltaTime = timer.update();
                window.processInput(deltaTime);
                
                time_accumulator += deltaTime;
                while (time_accumulator >= physics.get_config().time_step) {
                    time_accumulator -= speed_factor * physics.step();
                }

                update_objects();
                
                window.clear();
                render.draw();

                window.swap_buffers();
            }
        }

        RigidBody* add_static_object(double angle, vector2&& position, ShapeParameters* parameters)
        {
            RigidBody* body_ptr = physics.add_static_body(
                VERTEX_FUNCTIONS[parameters->vertex_function](parameters), angle, position
            );

            Shape* shape_ptr = render.add_shape(
                parameters->xscaling, parameters->yscaling, angle, position.x, position.y, 
                parameters->shape_type, body_ptr->get_id()
            );
            
            m_objects.emplace_back(std::make_unique<Body>(shape_ptr, body_ptr));

            return body_ptr;
        }


        RigidBody* add_rotational_object(double mass, double angle, double angular_velocity, 
                                        vector2&& position, ShapeParameters* parameters)
        {
            RigidBody* body_ptr = physics.add_rotational_body(mass, 
                VERTEX_FUNCTIONS[parameters->vertex_function](parameters), angle, angular_velocity, position
            );

            Shape* shape_ptr = render.add_shape(
                parameters->xscaling, parameters->yscaling, angle, position.x, position.y, 
                parameters->shape_type, body_ptr->get_id()
            );
            
            m_objects.emplace_back(std::make_unique<Body>(shape_ptr, body_ptr));

            return body_ptr;
        }


        RigidBody* add_dynamic_object(double mass, double angle, double angular_velocity, vector2&& position, 
                                      vector2&& velocity, ShapeParameters* parameters)
        {
            RigidBody* body_ptr = physics.add_dynamic_body(mass, 
                VERTEX_FUNCTIONS[parameters->vertex_function](parameters), angle, angular_velocity, position,
                velocity
            );

            Shape* shape_ptr = render.add_shape(
                parameters->xscaling, parameters->yscaling, angle, position.x, position.y, 
                parameters->shape_type, body_ptr->get_id()
            );
            
            m_objects.emplace_back(std::make_unique<Body>(shape_ptr, body_ptr));

            return body_ptr;
        }

        
        void add_spring_connector(RigidBody* body1, RigidBody* body2, AnchorType anchor1, 
                                  AnchorType anchor2, double spring_constant, double spring_length)
        {
            SpringGenerator* spring = 
                physics.add_spring_connector(body1, body2, anchor1, anchor2, spring_constant, spring_length);

            Shape* shape_ptr = render.add_shape(1, 1, 1, 1, 1, ShapeType::SPRING,
                                               spring->get_id());

            m_objects.emplace_back(std::make_unique<Spring>(shape_ptr, spring));
        }
};

#endif