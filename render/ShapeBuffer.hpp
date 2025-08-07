#ifndef SHAPE_BUFFER_HPP
#define SHAPE_BUFFER_HPP

#include <vector>
#include <ranges>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vao.hpp"
#include "shapes.hpp"
#include "shader.hpp"

class ShapeBuffer 
{
    protected:
        VaoType m_vao_type {};

        ShapeBuffer() = default;

        size_t m_num_shapes { 0 };
        std::vector<std::unique_ptr<Shape>> m_shapes {};

        std::unique_ptr<Vao> m_vao {};
        std::shared_ptr<Shader> m_shader {};
        
    public:
        std::string_view vertex_path   {};
        std::string_view fragment_path {};

        void bind_shader(std::shared_ptr<Shader> shader) { m_shader = shader; }

        void add_shape(std::unique_ptr<Shape> shape)
        {
            m_shapes.push_back(std::move(shape));
        }

        virtual void bind_vao() = 0;
        virtual void draw() = 0;
};


class CircleBuffer : public ShapeBuffer
{
    private: 
        size_t m_num_vertices { 500 };

    public:
        CircleBuffer()
        {
            m_vao = std::make_unique<StdVAO>();
            vertex_path   = "../shaders/shader.vx.glsl";
            fragment_path = "../shaders/shader.ft.glsl";

            set_num_vertices(m_num_vertices);
        }

        void set_num_vertices(size_t num_vertices)
        {   
            m_vertices.clear();
            double angle_step = 2.0 * M_PI / num_vertices;
            for (size_t i = 0; i < num_vertices; ++i) {
                double angle = i * angle_step;
                float x = cos(angle);
                float y = sin(angle);
                m_vertices.push_back(x);  
                m_vertices.push_back(y);  
                m_vertices.push_back(0.0f);  
                m_vertices.push_back(1.0f);  
                m_vertices.push_back(1.0f);  
                m_vertices.push_back(1.0f);
            } 
        }

        void bind_vao() override
        {   
            m_vao->bind_buffers();
            m_vao->set_vbo_data(sizeof(float) * m_vertices.size(), m_vertices.data());
        }

        void draw() override
        {   
            int modelLoc = glGetUniformLocation(m_shader->ID, "model");
            m_vao->bind();
            m_shader->use();
            for(auto& circle : m_shapes) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(circle->model));
                glDrawArrays(GL_TRIANGLE_FAN, 0, m_num_vertices + 2);
            }
            glBindVertexArray(0);
        }   
    
        private:
            std::vector<float> m_vertices {};
};


class RectangleBuffer : public ShapeBuffer
{   
    public:
        RectangleBuffer()
        {
            m_vao_type = STD_VAO;
            m_vao = std::make_unique<StdVAO>();
            vertex_path   = "../shaders/shader.vx.glsl";
            fragment_path = "../shaders/shader.ft.glsl";
        }

        void bind_vao()
        {   
            m_vao->bind_buffers();
            m_vao->set_vbo_data(sizeof(m_vertices), m_vertices.data());
        }

        void draw()
        {   
            int modelLoc = glGetUniformLocation(m_shader->ID, "model");
            
            m_vao->bind();
            m_shader->use();
            for(auto& line : m_shapes) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(line->model));
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            }
            glBindVertexArray(0);
        }  
    
    private:
        const std::array<float, 24> m_vertices = {
            // positions       // colors
            -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,
        };
};


// class PolygonBuffer : public ShapeBuffer
// {
//     public: 
//         int nvertices { 3 };
//         size_t npolygons { 0 };
//         std::vector<Shape> polygons {};
        
//         PolygonBuffer(std::vector<float>& vertices) : m_vertices(vertices)
//         {   
//             m_vao_type = STD_VAO;
//             m_vao = std::make_unique<StdVAO>();
//             vertex_path   = "../shaders/shader.vx.glsl";
//             fragment_path = "../shaders/shader.ft.glsl";

//             nvertices = vertices.size();
//         }

//         void add_polygon(float x, float y, float angle, float scaling, std::string_view id)
//         {
//             polygons.emplace_back(glm::vec2 { x, y }, angle, scaling, id);
//             ++npolygons;
//         }

//         void bind_vao() override
//         {   
//             m_vao->bind_buffers();
//             m_vao->set_vbo_data(sizeof(float) * m_vertices.size(), m_vertices.data());
//         }

//         void draw() override
//         {   
//             int modelLoc = glGetUniformLocation(m_shader->ID, "model");
//             m_vao->bind();
//             m_shader->use();
//             for(auto& polygon : polygons) {
//                 glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(polygon.model));
//                 glDrawArrays(GL_TRIANGLE_FAN, 0, nvertices + 2);
//             }
//             glBindVertexArray(0);
//         }   
    
//     private: 
//         std::vector<float> m_vertices {};                                                                                   
// };


struct SpringSpriteBuffer : ShapeBuffer
{
    int m_modelLoc {};
    unsigned int counter { 0 };

    SpringSpriteBuffer()
    {   
        m_vao_type = STD_VAO;
        m_vao = std::make_unique<StdVAO>();
        vertex_path   = "../shaders/shader.vx.glsl";
        fragment_path = "../shaders/shader.ft.glsl";
    }

    void bind_vao() override
    {   
        m_vao->bind_buffers();
        m_vao->set_vbo_data(sizeof(m_vertices), m_vertices.data());
    }

    void draw()
    {   
        m_vao->bind();
        m_shader->use();
        glLineWidth(4.5f);

        for(auto& sprite : m_shapes) {
            glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(sprite->model));
            glDrawArrays(GL_LINES, 0, 22);
        }
        glLineWidth(1.0f);
        glBindVertexArray(0);
    }   

    static constexpr float h = 0.2f;
    static constexpr float w = 0.1f;

    static constexpr std::array<float, 132> m_vertices = {
        // positions       // colors
        0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
           w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
           w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         2*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         2*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         3*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         3*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         4*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         4*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         5*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         5*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         6*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         6*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         7*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         7*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         8*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         8*w,   -h, 0.0f,  1.0f, 1.0f, 1.0f,
         9*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,
         9*w,    h, 0.0f,  1.0f, 1.0f, 1.0f,           
        1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    };
};


// class Line2DBuffer
// {
//     StdVAO m_vao {};
//     std::shared_ptr<Shader> m_shader {}; 


//     public:
//         const VaoType vao_id = STD_VAO;
//         std::string_view vertex_path { "../shaders/shader.vx.glsl" };
//         std::string_view fragment_path { "../shaders/shader.ft.glsl" };
    
//     unsigned int nlines { 0 };
//     std::vector<Line2d> lines {};

//     void bind_vao()
//     {   
//         m_vao.bind_buffers();
//         m_vao.set_vbo_data(sizeof(vertices), vertices.data());
//     }

//     void bind_shader(std::shared_ptr<Shader> shader)
//     {
//         m_shader = shader;
//     }

//     void add_line(glm::vec2 p1, glm::vec2 p2, std::string_view id)
//     {
//         lines.emplace_back(p1, p2, id);
//         ++nlines;
//     }

//     void draw()
//     {   
//         int modelLoc = glGetUniformLocation(m_shader->ID, "model");
        
//         m_vao.bind();
//         m_shader->use();
//         glLineWidth(1.5f);
//         for(auto& line : lines) {
//             glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(line.model));
//             glDrawArrays(GL_LINES, 0, 2);
//         }
//         glBindVertexArray(0);
//     }   

    
//     const std::array<float, 12> vertices = {
//         // positions       // colors
//         0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
//         1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
//     };
// };


#endif