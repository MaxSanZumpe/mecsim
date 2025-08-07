#include <string>

#include "System.hpp"
#include "sim_scene.hpp"
#include "parameters.hpp"

int main() 
{   
    SimScene sim {};

    sim.physics.set_global_gravity_flag(true);

    ElipseParameters elipse { 1, 1 };
    RectangleParameters rectangle { 1, 2 };

    RigidBody* b0 = sim.add_rotational_object(1, 0, 0, { 0, 0 }, &rectangle);
    RigidBody* b1 = sim.add_dynamic_object(1, 0, 0, { 3, 3 }, { 0, 0 }, &elipse);

    sim.add_spring_connector(b0, b1, OFFSET_25_ANGLE_30, OFFSET_25_ANGLE_30, 2, 1);

    sim.draw();

    return 0;
};