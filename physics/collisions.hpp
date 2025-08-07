#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

#include "vector2.hpp"

class RigidBody;

struct AABB 
{
    double min_x, max_x;
    double min_y, max_y;
};


struct Contact {
    RigidBody* a {};
    RigidBody* b {}; 
    vector2 normal {};
    double penetration {};
    std::vector<vector2> contact_points {};
};

AABB compute_bouding_box(const vector2* vertices, size_t size);

std::vector<std::pair<size_t, size_t>> sort_and_sweep_aabb_boxes(const std::vector<RigidBody*>& bodies);

bool detect_collisions(const std::vector<std::unique_ptr<RigidBody>>& bodies, 
                             std::vector<Contact>& contacts, double epsilon);

void resolve_contact(Contact& contact);

#endif