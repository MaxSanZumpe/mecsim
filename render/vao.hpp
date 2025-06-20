#ifndef VAO_HPP
#define VAO_HPP

#include <cstddef>
#include <glad/glad.h>

enum VaoType
{
    STD_VAO,  
    QUAD_VAO,
};


class Vao
{    
    public:
        virtual ~Vao() {}; 
        virtual void bind() = 0 ;
        virtual void bind_buffers() = 0;
        virtual void set_vbo_data(size_t size, const float* data) = 0;
        virtual void set_ebo_data(size_t size, const unsigned int* data) = 0;
};


class StdVAO : public Vao 
{   
    unsigned int vao {}, vbo {};

    public:
        StdVAO() = default;

        ~StdVAO() 
        {   
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }

        void bind_buffers() override;
        void set_vbo_data(size_t size, const float* data) override;

        void bind() override { glBindVertexArray(vao); }

    private:
        void set_ebo_data(size_t size, const unsigned int* data) override 
        {
            (void)size;
            (void)data;
        }
};


class QuadVAO : public Vao 
{   
    unsigned int vao {}, vbo {}, ebo {};

    public: 
        QuadVAO() = default;

        ~QuadVAO() 
        {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }

        void bind_buffers() override;
        void set_vbo_data(size_t size, const float* data) override;
        void set_ebo_data(size_t size, const unsigned int* data) override;

        void bind() override { glBindVertexArray(vao); }
};

#endif
