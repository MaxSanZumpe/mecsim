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

        std::unique_ptr<Vao> m_vao {};
        std::shared_ptr<Shader> m_shader {};
        
    public:
        std::string_view vertex_path   {};
        std::string_view fragment_path {};

        void bind_shader(std::shared_ptr<Shader> shader) { m_shader = shader; }

        virtual void bind_vao() = 0;
        virtual void draw() = 0;
};


class CircleBuffer : public ShapeBuffer
{
    public: 
        size_t ncircles { 0 };
        std::vector<Circle> circles {};
        
        CircleBuffer() 
        {   
            m_vao_type = QUAD_VAO;
            m_vao = std::make_unique<QuadVAO>();
            vertex_path   = "../shaders/circle_shader.vx.glsl";
            fragment_path = "../shaders/circle_shader.ft.glsl";
        }

        void add_circle(float x, float y, float radius, std::string_view id)
        {
            circles.emplace_back(glm::vec2 { x, y }, radius, id);
            ++ncircles;
        }

        void bind_vao() override
        {   
            m_vao->bind_buffers();
            m_vao->set_vbo_data(sizeof(vertices), vertices.data());
            m_vao->set_ebo_data(sizeof(indices), indices.data());
        }

        void draw() override
        {   
            int modelLoc = glGetUniformLocation(m_shader->ID, "model");
            m_vao->bind();
            m_shader->use();
            for(auto& circle : circles) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(circle.model));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            glBindVertexArray(0);
        }   
    
    private: 
        const std::array<float, 24> vertices = 
        {
            // positions          // colors
            -1.0f, -1.0f, 0.0f,   0.9f, 0.9f, 0.9f,
             1.0f, -1.0f, 0.0f,   0.9f, 0.9f, 0.9f,
             1.0f,  1.0f, 0.0f,   0.9f, 0.9f, 0.9f,
            -1.0f,  1.0f, 0.0f,   0.9f, 0.9f, 0.9f,
        };
        
        const std::array<unsigned int, 6> indices = 
        {
            0, 1, 2,
            2, 3, 0
        };                                                                                    
};


class RegularPolygonBuffer : public ShapeBuffer
{
    public: 
        int nsides { 3 };
        size_t npolygons { 0 };
        std::vector<Polygon> polygons {};
        
        RegularPolygonBuffer(int num_sides) : nsides(num_sides)
        {   
            m_vao_type = STD_VAO;
            m_vao = std::make_unique<StdVAO>();
            vertex_path   = "../shaders/shader.vx.glsl";
            fragment_path = "../shaders/shader.ft.glsl";

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
        }

        void add_polygon(float x, float y, float angle, float radius, std::string_view id)
        {
            polygons.emplace_back(glm::vec2 { x, y }, angle, radius, id);
            ++npolygons;
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
            for(auto& polygon : polygons) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(polygon.model));
                glDrawArrays(GL_TRIANGLE_FAN, 0, nsides + 2);
            }
            glBindVertexArray(0);
        }   
    
    private: 
        std::vector<float> m_vertices {};                                                                                   
};


class PolygonBuffer : public ShapeBuffer
{
    public: 
        int nvertices { 3 };
        size_t npolygons { 0 };
        std::vector<Polygon> polygons {};
        
        PolygonBuffer(std::vector<float>& vertices) : m_vertices(vertices)
        {   
            m_vao_type = STD_VAO;
            m_vao = std::make_unique<StdVAO>();
            vertex_path   = "../shaders/shader.vx.glsl";
            fragment_path = "../shaders/shader.ft.glsl";

            nvertices = vertices.size();
        }

        void add_polygon(float x, float y, float angle, float scaling, std::string_view id)
        {
            polygons.emplace_back(glm::vec2 { x, y }, angle, scaling, id);
            ++npolygons;
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
            for(auto& polygon : polygons) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(polygon.model));
                glDrawArrays(GL_TRIANGLE_FAN, 0, nvertices + 2);
            }
            glBindVertexArray(0);
        }   
    
    private: 
        std::vector<float> m_vertices {};                                                                                   
};


class Line2DBuffer
{
    StdVAO m_vao {};
    std::shared_ptr<Shader> m_shader {}; 


    public:
        const VaoType vao_id = STD_VAO;
        std::string_view vertex_path { "../shaders/shader.vx.glsl" };
        std::string_view fragment_path { "../shaders/shader.ft.glsl" };
    
    unsigned int nlines { 0 };
    std::vector<Line2d> lines {};

    void bind_vao()
    {   
        m_vao.bind_buffers();
        m_vao.set_vbo_data(sizeof(vertices), vertices.data());
    }

    void bind_shader(std::shared_ptr<Shader> shader)
    {
        m_shader = shader;
    }

    void add_line(glm::vec2 p1, glm::vec2 p2, std::string_view id)
    {
        lines.emplace_back(p1, p2, id);
        ++nlines;
    }

    void draw()
    {   
        int modelLoc = glGetUniformLocation(m_shader->ID, "model");
        
        m_vao.bind();
        m_shader->use();
        glLineWidth(1.5f);
        for(auto& line : lines) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(line.model));
            glDrawArrays(GL_LINES, 0, 2);
        }
        glBindVertexArray(0);
    }   

    
    const std::array<float, 12> vertices = {
        // positions       // colors
        0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    };
};


struct SpringSpriteBuffer
{
    StdVAO m_vao {};
    std::shared_ptr<Shader> m_shader {}; 

    int m_modelLoc {};

    const VaoType vao_id = STD_VAO;
    std::string_view vertex_path { "../shaders/shader.vx.glsl" };
    std::string_view fragment_path { "../shaders/shader.ft.glsl" };

    unsigned int counter { 0 };
    std::vector<SpringSprite> sprites {};

    void bind_vao()
    {   
        m_vao.bind_buffers();
        m_vao.set_vbo_data(sizeof(vertices), vertices.data());
    }

    void bind_shader(std::shared_ptr<Shader> shader)
    {
        m_shader = shader;
        m_modelLoc = glGetUniformLocation(m_shader->ID, "model");
    }

    void add_spring(glm::vec2 p1, glm::vec2 p2, std::string_view id)
    {
        sprites.emplace_back(p1, p2, id);
        ++counter;
    }

    void draw()
    {   
        m_vao.bind();
        m_shader->use();
        glLineWidth(3.5f);

        for(auto& sprite : sprites) {
            glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(sprite.model));
            glDrawArrays(GL_LINES, 0, 22);
        }
        glLineWidth(1.0f);
        glBindVertexArray(0);
    }   

    static constexpr float h = 0.2f;
    static constexpr float w = 0.1f;

    static constexpr std::array<float, 132> vertices = {
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

#endif