#include "System.hpp"

int main() 
{   
    System sys {};
    sys.add_rigid_body(1, 0.5, {{1, 2},{1, 1} }, 0, 1, {1, 1}, {1, 1});
    sys.add_rigid_body(1, 0.5, {{1, 2},{1, 1} }, 0, 0, {1, 1}, {1, 1});

    sys.set_global_gravity_flag(false);
    sys.print_config();
    sys.print_rigid_body_info();
    sys.step();
    sys.step();
    sys.print_rigid_body_info();

    return 0;
}