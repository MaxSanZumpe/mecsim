#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

#include <array>
#include <vector>
#include <memory>
#include <sstream>

#include "util.hpp"
#include "vector2.hpp"
#include "collisions.hpp"


enum RigidBodyType 
{
    DYNAMIC_BODY,
    ROTATIONAL_ONLY,
    STATIC_BODY,
};


enum AnchorType
{
    OFFSET_0_ANGLE_0,

    OFFSET_25_ANGLE_0,
    OFFSET_25_ANGLE_30,
    OFFSET_25_ANGLE_60,
    OFFSET_25_ANGLE_90,

    OFFSET_50_ANGLE_0,
    OFFSET_50_ANGLE_30,
    OFFSET_50_ANGLE_60,
    OFFSET_50_ANGLE_90,
    OFFSET_50_ANGLE_180,
    OFFSET_50_ANGLE_270,

    OFFSET_100_ANGLE_0,
    NUM_ANCHOR_TYPES,
};


constexpr std::array<std::pair<double, double>, NUM_ANCHOR_TYPES> anchor_data {
    std::pair { 0.0, 0.0 },

    std::pair { 0.25, 0   },
    std::pair { 0.25, 30  },
    std::pair { 0.25, 60  },
    std::pair { 0.25, 90  },
    
    std::pair { 0.5, 0.0 },
    std::pair { 1.0, 0.0 },
};

class RigidBody {
    protected:
        double m_inv_mass { 1 };
        double m_inv_inertia { 1 };

        std::vector<double> m_anchor_angles {};
        std::vector<double> m_anchor_offsets {};
        
        std::vector<vector2> m_vertices {};
        std::vector<vector2> m_world_vertices {};

        std::vector<vector2> m_normals {};
        std::vector<vector2> m_rotated_normals {};

        AABB m_bounding_box {};

        std::string m_id { generate_id() };    
        inline static int m_instance_id { 0 }; 
        
        static std::string generate_id() 
        {
            return "rbid" + std::to_string(m_instance_id++);
        }

        void compute_normals()
        {   
            size_t n = m_vertices.size();
            m_normals.resize(n);

            for (size_t i = 0; i < n; ++i) {
                vector2 a = m_vertices[i];
                vector2 b = m_vertices[(i + 1) % n];

                vector2 ab = b - a;
                vector2 normal = vector2 { -ab.y, ab.x };
                normal = normal/normal.norm();

                m_normals[i] = normal;
            }
        }


        void covert_aabb_to_world_space() {
            m_bounding_box = compute_bouding_box(m_world_vertices.data(), m_world_vertices.size());
        }

        void rotate_normals() 
        {
            for (size_t i = 0; i < m_normals.size();  ++i) {
                m_rotated_normals[i] = rotate(m_normals[i], angle);
            }
        }

        void convert_vertices_to_world_space() 
        {
            for (size_t i = 0; i < m_vertices.size(); ++i) {
                m_world_vertices[i] = position + rotate(m_vertices[i], angle);
            }
        }
    
    public:
        double angle {};
        double angular_velocity{};

        vector2 position {};
        vector2 velocity {};

        vector2 force_accumulator {};
        double torque_accumulator {};

        RigidBodyType type { DYNAMIC_BODY };
        
        RigidBody(double mass, std::vector<vector2>&& vertices, double angle, double angular_velocity, 
                  vector2& pos, vector2& velocity, RigidBodyType _type) : 
                  m_vertices(std::move(vertices)), angle(angle), angular_velocity(angular_velocity),
                  position(std::move(pos)), velocity(std::move(velocity)), type(_type)
        {   
            if (mass <= 0) {
                throw std::runtime_error("ERROR::RIGID_BODY::CONSTRUCTOR::NON_POSITVE_MASS");
            }

            if (!is_convex(vertices.data(), vertices.size())) {
                std::ostringstream oss;
                oss << "ERROR::RIGID_BODY(" << m_id << ")::CONSTRUCTOR::ONLY_CONVEX_POLYGONS_SUPPORTED";
                throw std::runtime_error(oss.str());
            }   

            center_vertices_to_centroid(m_vertices);
            double inertia = moment_of_inertia_per_unit_mass_about_origin(m_vertices.data(), m_vertices.size());

            switch (_type) {
                case RigidBodyType::DYNAMIC_BODY:
                    m_inv_mass = 1/mass;
                    m_inv_inertia = 1/(mass * inertia);
                    break;

                case RigidBodyType::ROTATIONAL_ONLY:
                    m_inv_mass = 0;
                    m_inv_inertia = 1/(mass * inertia);
                    break;

                case RigidBodyType::STATIC_BODY:
                    m_inv_mass = 0;
                    m_inv_inertia = 0;
                    break;
            }

            for (auto& p : anchor_data) {
                double angle = p.second;
                double distance_factor = p.first;
                double distance = distance_to_edge(angle, m_vertices.data(), m_vertices.size());

                m_anchor_angles.push_back(angle);
                m_anchor_offsets.push_back(distance_factor * distance);
            }
            
            compute_normals(); 
            m_bounding_box = compute_bouding_box(m_vertices.data(), m_vertices.size());
            
            m_world_vertices.resize(m_vertices.size());
            m_rotated_normals.resize(m_normals.size());

            update_polygon_features();
        }

        double get_inverse_mass()    const { return m_inv_mass; }
        double get_inverse_inertia() const { return m_inv_inertia; }
        std::string get_id() const { return m_id; }

        const AABB& get_aabb() const { return m_bounding_box; }

        const std::vector<vector2>& get_vertices() const { return m_vertices; }
        const std::vector<vector2>& get_world_vertices() const { return m_world_vertices; }
        const std::vector<vector2>& get_normals()  const { return m_rotated_normals; }


        vector2 get_anchor_position(AnchorType anchor)
        {   
            try {
                return position + m_anchor_offsets.at(anchor) * direction(angle + m_anchor_angles.at(anchor));
            } catch (const std::out_of_range& e) {
                std::cerr << "WARNING::RIGID_BODY(" << m_id << ")::INVALID_ANCHOR_INDEX\n";
                return position;
            }
        }

        void update_polygon_features() 
        {
            convert_vertices_to_world_space();
            covert_aabb_to_world_space();
            rotate_normals();
        }

        void add_anchor(double relative_offset, double angle) 
        {
            m_anchor_angles.emplace_back(angle);
            m_anchor_offsets.emplace_back(relative_offset);
        }
};


#endif