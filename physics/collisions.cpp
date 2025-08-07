#include "collisions.hpp"
#include "RigidBody.hpp"

AABB compute_bouding_box(const vector2* vertices, size_t size) 
{
    AABB box {};

    for (size_t i = 0; i < size; ++i) {
        box.min_x = std::min(box.min_x, vertices[i].x);
        box.max_x = std::max(box.max_x, vertices[i].x);
        box.min_y = std::min(box.min_y, vertices[i].y);
        box.max_y = std::max(box.max_y, vertices[i].y);
    }

    return box;
}

void translate_bounding_box(const vector2& position, AABB& box) 
{
    box.min_x += position.x; 
    box.max_x += position.x;
    box.min_y += position.y;
    box.max_y += position.y;
}


std::vector<std::pair<size_t, size_t>> sort_and_sweep_aabb_boxes(const std::vector<std::unique_ptr<RigidBody>>& bodies) 
{
    std::vector<size_t> sorted_indices(bodies.size());
    std::iota(sorted_indices.begin(), sorted_indices.end(), 0);

    std::sort(sorted_indices.begin(), sorted_indices.end(),
    [&](size_t i, size_t j) {
        return bodies[i]->get_aabb().min_x < bodies[i]->get_aabb().min_x;
    });

    std::vector<std::pair<size_t, size_t>> candidate_pairs;

    for (size_t i = 0; i < sorted_indices.size(); ++i) {
        const AABB& a = bodies[sorted_indices[i]]->get_aabb();
        for (size_t j = i + 1; j < sorted_indices.size(); ++j) {
            const AABB& b = bodies[sorted_indices[j]]->get_aabb();
            if (b.min_x > a.max_x) break;
            if (a.max_y >= b.min_y && b.max_y >= a.min_y) {
                candidate_pairs.emplace_back(sorted_indices[i], sorted_indices[j]);
            }
        }
    }

    return candidate_pairs;
}


std::pair<double, double> project_polygon(const vector2& axis, const vector2* vertices, size_t size) 
{   
    double projection = vertices[0]*axis;
    double max = projection;
    double min = projection;

    for (size_t i = 1; i < size; ++i) {
        projection = vertices[i]*axis;
        max = std::max(max, projection);
        min = std::min(min, projection);
    }

    return std::pair<double, double> { min, max };
}

int find_best_edge(const std::vector<vector2>& verts, const vector2& normal) {
    int best = 0;
    double max_dot = -std::numeric_limits<double>::infinity();

    for (int i = 0; i < (int)verts.size(); ++i) {
        int j = (i + 1) % verts.size();
        vector2 edge = verts[j] - verts[i];
        vector2 edge_normal = normalize(perpendicular(edge));
        double d = edge_normal * normal;
        if (d > max_dot) {
            max_dot = d;
            best = i;
        }
    }
    return best;
}


int clip_edge(std::vector<vector2>& out_pts, vector2 p1, vector2 p2, vector2 normal, double offset) {
    out_pts.clear();

    double d1 = normal * p1 - offset;
    double d2 = normal * p2 - offset;

    if (d1 >= 0.0) out_pts.push_back(p1);
    if (d2 >= 0.0) out_pts.push_back(p2);

    if (d1 * d2 < 0.0) {
        // line intersects the plane
        double t = d1 / (d1 - d2);
        vector2 intersect = p1 + t * (p2 - p1);
        out_pts.push_back(intersect);
    }

    return out_pts.size();
}


bool detect_collisions(const std::vector<std::unique_ptr<RigidBody>>& bodies, 
                       std::vector<Contact>& contacts, double epsilon)
{
    contacts.clear();

    bool deep_penetration_found = false;

    std::vector<std::pair<size_t, size_t>> candidate_pairs = sort_and_sweep_aabb_boxes(bodies);

    if (candidate_pairs.empty()) {
        return deep_penetration_found; 
    }

    for (auto& pair : candidate_pairs) {
        RigidBody* a = bodies[pair.first].get();
        RigidBody* b = bodies[pair.second].get();
            
        const std::vector<vector2>& vertices_a = a->get_world_vertices();
        const std::vector<vector2>& vertices_b = b->get_world_vertices();
        
        bool separating_found = false;
        double min_depth = std::numeric_limits<double>::max();
        vector2 collision_normal;

        Contact contact;
        RigidBody* ref = nullptr;

        // SAT for a's normals
        for (auto& normal : a->get_normals()) {
            auto [min_a, max_a] = project_polygon(normal, vertices_a.data(), vertices_a.size());
            auto [min_b, max_b] = project_polygon(normal, vertices_b.data(), vertices_b.size());

            if (max_a < min_b || min_a > max_b) {
                separating_found = true;
                break;
            } 

            double depth = std::min(max_a, max_b) - std::max(min_a, min_b);
            if (depth < min_depth) {
                min_depth = depth;
                collision_normal = normal;
                ref = a;
            }
        }

        if (separating_found) {
            continue;
        }

        // SAT for b's normals
        for (auto& normal : b->get_normals()) {
            auto [min_a, max_a] = project_polygon(normal, vertices_a.data(), vertices_a.size());
            auto [min_b, max_b] = project_polygon(normal, vertices_b.data(), vertices_b.size());

            if (max_a < min_b || min_a > max_b) {
                separating_found = true;
                break;
            } 

            double depth = std::min(max_a, max_b) - std::max(min_a, min_b);
            if (depth < min_depth) {
                min_depth = depth;
                collision_normal = normal;
                ref = b;
            }
        }

        if (separating_found) {
            continue;
        }

        if (min_depth >= epsilon) {
            deep_penetration_found = true;
            return deep_penetration_found;
        }

        RigidBody* inc = (ref == a) ? b : a;
        const auto& ref_verts = ref->get_world_vertices();
        const auto& inc_verts = inc->get_world_vertices();

        // Find reference and incident edges
        int ref_edge_idx = find_best_edge(ref_verts, collision_normal);
        int inc_edge_idx = find_best_edge(inc_verts, -collision_normal);

        vector2 ref_v1 = ref_verts[ref_edge_idx];
        vector2 ref_v2 = ref_verts[(ref_edge_idx + 1) % ref_verts.size()];
        vector2 inc_v1 = inc_verts[inc_edge_idx];
        vector2 inc_v2 = inc_verts[(inc_edge_idx + 1) % inc_verts.size()];

        vector2 ref_edge = ref_v2 - ref_v1;
        vector2 ref_normal = normalize(perpendicular(ref_edge));
        double ref_offset = ref_normal * ref_v1;

        vector2 side_normal1 = normalize(ref_edge);
        double side_offset1 = side_normal1 * ref_v1;

        vector2 side_normal2 = -side_normal1;
        double side_offset2 = side_normal2 * ref_v2;

        // Clip incident edge to reference edge side planes
        std::vector<vector2> clipped_points;
        int np = clip_edge(clipped_points, inc_v1, inc_v2, side_normal1, side_offset1);
        if (np < 2) continue;

        np = clip_edge(clipped_points, clipped_points[0], clipped_points[1], side_normal2, side_offset2);
        if (np < 2) continue;

        contact.contact_points.clear();
        contact.penetration = 0.0;

        for (const auto& pt : clipped_points) {
            double separation = ref_normal * pt - ref_offset;
            if (separation <= 0) {
                contact.contact_points.push_back(pt);
            }
        }

        if (contact.contact_points.empty()) {
            continue;
        }

        contact.a = inc;
        contact.b = ref;
        
        if (collision_normal * (inc->position - ref->position) < 0) {
            contact.normal = -collision_normal;
        } else {
            contact.normal = collision_normal;  
        }
        
        contacts.push_back(contact);
    }

    return deep_penetration_found;
}


void resolve_contact(Contact& contact)
{
    double restitution = 1.0;

    for (const vector2& p : contact.contact_points)
    {
        vector2 r_a = p - contact.a->position;
        vector2 r_b = p - contact.b->position;

        vector2 vel_a = contact.a->velocity + contact.a->angular_velocity * perpendicular(r_a);
        vector2 vel_b = contact.b->velocity + contact.b->angular_velocity * perpendicular(r_b);

        double relative_vel = contact.normal * (vel_a - vel_b);

        if (relative_vel >= 0.0)
            continue; // already separating or at rest

        double ra_cross_n = cross2d(r_a, contact.normal);
        double rb_cross_n = cross2d(r_b, contact.normal);

        double inv_mass_sum =
            contact.a->get_inverse_mass() +
            contact.b->get_inverse_mass() +
            ra_cross_n * ra_cross_n * contact.a->get_inverse_inertia() +
            rb_cross_n * rb_cross_n * contact.b->get_inverse_inertia();

        double impulse_scalar = -(1.0 + restitution) * relative_vel / inv_mass_sum;

        vector2 impulse = impulse_scalar * contact.normal;

        contact.a->velocity +=  impulse * contact.a->get_inverse_mass();
        contact.b->velocity += -impulse * contact.b->get_inverse_mass();

        contact.a->angular_velocity += contact.a->get_inverse_inertia() * cross2d(r_a, impulse);
        contact.b->angular_velocity -= contact.b->get_inverse_inertia() * cross2d(r_b, impulse);
    }
}
