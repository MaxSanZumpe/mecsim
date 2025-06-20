#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "glfw_window.hpp"
#include "shapes.hpp"
#include "scene.hpp"


struct GlSettings 
{
    bool msaa_flag  { true  };
    bool vsync_flag { true  };
    bool wirem_flag { false };
};


struct GLFW
{
    GLFW(int major_gl_version, int minor_gl_version) 
    {
        if(!glfwInit()) {
            std::cerr << "ERROR::GLFW_INIT_FAILED" << '\n';
            glfwTerminate();
        }
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_gl_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_gl_version);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    ~GLFW()
    {
        glfwTerminate();
    }
};


struct FpsTimer 
{
    int nbFrames {};
    double currentTime {};
    double deltaTime {};
    double lastTime = glfwGetTime();
    double lastFPSTime = lastTime;

    double fps {};

    float update() 
    {
        currentTime   = glfwGetTime();
        deltaTime     = currentTime - lastTime;
        lastTime = currentTime;
        nbFrames++;

        if (currentTime - lastFPSTime >= 1.0) {
            fps = static_cast<double>(nbFrames) / (currentTime - lastFPSTime);

            nbFrames = 0;  
            lastFPSTime = currentTime; 
        }

        return deltaTime;
    }
};


class Renderer
{
    private:
        FpsTimer timer {};
        GlSettings settings {};

        ShaderManager shader_manager {};
    
    public:
        void render(Scene* scene)
        {   
            GLFW glwf { 3, 3 };

            Window glfw_window = Window { 1000, 700, "Placeholder" };
            GLFWwindow* window = glfw_window.activate();
            camera::Ortho2D camera { window, scene->m_world_width };


            glfw_window.set_background_color(0.1f, 0.1f, 0.1f, 1.0f);
            scene->bind_vaos();
            scene->bind_shaders(shader_manager);


            float deltaTime {};

            while (!glfwWindowShouldClose(window))
            {   
                deltaTime = timer.update();
                glfw_window.processInput(deltaTime);
                glfw_window.clear();
                
                scene->animator.update_scene(deltaTime);
                scene->draw();

                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }
};

#endif