#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
out vec3 fragPos;

layout(std140) uniform matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;


void main()
{   
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}    