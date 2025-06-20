#ifndef GLFW_WINDOW_HPP
#define GLFW_WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <array>

#include "camera.hpp"


class Window 
{
    private: 
        GLFWwindow* m_window {};
        std::array<GLfloat, 4> m_background_rgbo { 0.0, 0.0, 0.0, 0.0 }; 
        
        glm::mat4 m_view {};
        glm::mat4 m_projection {};
        unsigned int m_view_projection_ubo {};

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }

    public:
        Window(const unsigned int width, const unsigned int height, const char* title) 
        {
            m_window = glfwCreateWindow(width, height, title, NULL, NULL);
            if (m_window == NULL)
            {
                std::cerr << "ERROR::GLFW::FAILED_WINDOW_CREATION" << '\n';
                glfwTerminate();
            }
        }

        GLFWwindow* get_window_ptr() { return m_window; }

        void set_background_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat opacity)
        {
            m_background_rgbo[0] = red;
            m_background_rgbo[1] = green;
            m_background_rgbo[2] = blue;
            m_background_rgbo[3] = opacity;
        }

        GLFWwindow* activate()
        {
            glfwMakeContextCurrent(m_window);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cerr << "ERROR::GLAD::FAILED_INITIALIZATION" << '\n';
                glfwTerminate();
            }
            glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
            
            glfwSwapInterval(1);
            glfwWindowHint(GLFW_SAMPLES, 16);
            
            glEnable(GL_MULTISAMPLE);

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
            m_view = glm::lookAt(glm::vec3 {0.0f, 0.0f, 10.0f}, glm::vec3 {0.0f, 0.0f, -1.0f}, glm::vec3 { 0.0f, 1.0f, 0.0f });
            m_projection = glm::perspective(glm::radians(45.0f), aspectRatio(), 0.1f, 200.1f);

            glGenBuffers(1, &m_view_projection_ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_ubo);
            glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_view));  
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projection));  
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_view_projection_ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            
            return m_window;
        }

        void processInput(float deltaFrame)
        {
            auto* cam = static_cast<camera::Camera*>(glfwGetWindowUserPointer(m_window));
            if (cam){
                cam->process_camera_inputs(m_window, deltaFrame);

                m_view = glm::lookAt(cam->cameraPos, cam->cameraPos + cam->cameraFront, cam->cameraUp);
                
                m_projection = cam->compute_projection(aspectRatio());

                glBindBuffer(GL_UNIFORM_BUFFER, m_view_projection_ubo);
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_view));  
                glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projection));  
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
        }

        void clear() const
        {
            glClearColor(m_background_rgbo[0], m_background_rgbo[1], m_background_rgbo[2], m_background_rgbo[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        float aspectRatio() const 
        {
            int width, height;
            glfwGetFramebufferSize(m_window, &width, &height);
            return static_cast<float>(width) / static_cast<float>(height);
        }
};


#endif