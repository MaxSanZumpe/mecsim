#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CAMERA_OPTIONS
{
    STATIC,
    ORTHO2D,
    FLY3D,
};


class Camera 
{      
    public:
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        virtual glm::mat4 compute_projection(float aspect_ratio) = 0;

    protected:
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
        {
            auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
            if (cam) 
                cam->process_camera_view(xpos, ypos);
        }

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
        {
            auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
            if (cam) 
                cam->process_camera_zoom(xoffset, yoffset);
        }

        virtual void process_camera_movement(float deltaTime) = 0;
        virtual void process_camera_view(double xpos, double ypos) = 0;
        virtual void process_camera_zoom(double xoffset, double yoffset) = 0;

    public:
        void process_camera_inputs(GLFWwindow* window, float deltaFrame)
        {   
            process_camera_movement(deltaFrame);
            //glfwSetCursorPosCallback(window, mouse_callback);
            glfwSetScrollCallback(window, scroll_callback);
        }

};

class Fly3D : public Camera
{   
    public: 
        float yaw { -90.0f };
        float pitch { 0.0f };
        bool firstMouse { true }; 

        float zoom = 45.0f;
        
        GLFWwindow* m_window { nullptr };
        float lastX {};
        float lastY {};


        Fly3D(GLFWwindow* window, const unsigned world_width, const unsigned world_height) : 
            m_window(window), lastX(world_width/2), lastY(world_height/2)
        {   
            if (!window) {
                std::cerr << "ERROR::CAMERA::FAILED_TO_BIND_WINDOW::NULLPTR" << '\n';
            }
            glfwSetWindowUserPointer(window, this);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        void process_camera_movement(float deltaTime) override
        {
            float cameraSpeed = 1.5f * deltaTime;
            if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
                cameraPos += cameraSpeed * cameraFront;
            if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
                cameraPos -= cameraSpeed * cameraFront;
            if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                cameraPos += glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) *
                cameraSpeed;
            if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                cameraPos -= glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) *
                cameraSpeed;
        }

        void process_camera_view(double xpos, double ypos) override
        {
            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;
            float sensitivity = 0.01f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;
            yaw += xoffset;
            pitch += yoffset;
            if(pitch > 89.0f) {
                pitch = 89.0f;
            }
            if(pitch < -89.0f) {
                pitch = -89.0f;
            }
            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(direction);
        }

        void process_camera_zoom(double xoffset, double yoffset) override
        {
            zoom -= (float)yoffset;
            if (zoom < 10.0f)
                zoom = 10.0f;

            if (zoom > 60.0f) 
                zoom = 60.0f;
        }
};

class Ortho2D : public Camera
{
    public: 
        GLFWwindow* m_window { nullptr };
        
        float view_width {};
        float world_width {};


        Ortho2D(GLFWwindow* window, const unsigned world_width): 
                m_window(window), world_width(world_width)
        {   
            view_width = world_width/2;
            if (!window) {
                std::cerr << "ERROR::CAMERA::FAILED_TO_BIND_WINDOW::NULLPTR" << '\n';
            }
            glfwSetWindowUserPointer(window, this);
        }

        void process_camera_movement(float deltaTime) override
        {
            float cameraSpeed = 5.0f * deltaTime;
            float cameraRotation = 20.0f * deltaTime;
            if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

            if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
                cameraPos += glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) *
                cameraSpeed;
            if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
                cameraPos -= glm::normalize(glm::cross(glm::cross(cameraFront, cameraUp), cameraFront)) *
                cameraSpeed;

            if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
                cameraUp = glm::vec3(glm::rotate(glm::mat4(1.0f), -glm::radians(cameraRotation), cameraFront) * 
                           glm::vec4(cameraUp, 1.0f));

            if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
                cameraUp = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(cameraRotation), cameraFront) * 
                           glm::vec4(cameraUp, 1.0f));
        }

        void process_camera_view(double xpos, double ypos) override {};
        void process_camera_zoom(double xoffset, double yoffset) override
        {   
            float sensitivity = 2.0f;
            view_width -= (float)yoffset * sensitivity;
            if (view_width < 2.0f)
                view_width = 2.0f;
                
            if (view_width > world_width)
                view_width = world_width;
        }

        glm::mat4 compute_projection(float aspect_ratio) override
        {
            glm::mat4 projection = glm::ortho(-view_width/2, view_width/2, 
                                              -view_width/(2*aspect_ratio), 
                                               view_width/(2*aspect_ratio), -1.1f, 1.1f);
        
            return projection;
        }
};


#endif 