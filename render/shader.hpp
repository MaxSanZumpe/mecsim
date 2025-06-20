#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <functional>

class Shader
{
    public:
        unsigned int ID;

        ~Shader() 
        {
            glDeleteProgram(ID);
        }

        Shader(std::string_view vertex_shader_path, std::string_view fragment_shader_path)
        {
            std::string vertex_source {};
            std::string fragment_source {};

            std::ifstream v_shader_file(vertex_shader_path.data());
            if (!v_shader_file.is_open()) {
                std::cerr << "ERROR::SHADER::VERTEX::FILE_NOT_FOUND: " << vertex_shader_path << '\n';
                return;
            }

            std::ifstream f_shader_file(fragment_shader_path.data());
            if (!f_shader_file.is_open()) {
                std::cerr << "ERROR::SHADER::FRAGMENT::FILE_NOT_FOUND: " << fragment_shader_path << '\n';
                return;
            }

            std::stringstream v_stream, f_stream;
            v_stream << v_shader_file.rdbuf();
            f_stream << f_shader_file.rdbuf();

            v_shader_file.close();
            f_shader_file.close();

            vertex_source = v_stream.str();
            fragment_source = f_stream.str();


            const char* vShader_source = vertex_source.c_str();
            const char* fShader_source = fragment_source.c_str();


            int success;
            char infoLog[512];
            unsigned int vertex, fragment;

            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShader_source, NULL);
            glCompileShader(vertex);
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
                infoLog << '\n';
            };

            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShader_source, NULL);
            glCompileShader(fragment);
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(fragment, 512, NULL, infoLog);
                std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
                infoLog << '\n';
            };
            
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            glLinkProgram(ID);
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
                infoLog << std::endl;
            }

            // Bind to standard window ubo at bind point 0
            unsigned int uniformBlockIndex = glGetUniformBlockIndex(ID, "matrices");
            glUniformBlockBinding(ID, uniformBlockIndex, 0);

            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }

        void use()
        {
            glUseProgram(ID);
        }
        
        void setBool(std::string& name, bool value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }

        void setInt(const std::string& name, int value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }

        void setFloat(const std::string& name, float value) const
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
};

inline size_t generate_hash(std::string_view str1, std::string_view str2) 
{
    std::hash<std::string_view> hash_fn;

    // Hash both string_views individually
    size_t hash1 = hash_fn(str1);
    size_t hash2 = hash_fn(str2);

    // Combine the hashes using XOR and shifting
    return hash1 ^ (hash2 << 1);  // XOR and shift to combine the hashes
}

struct ShaderManager 
{
    std::unordered_map<size_t, std::shared_ptr<Shader>> active_shaders {};

    std::shared_ptr<Shader> load_shader(std::string_view vertex_shader_path, std::string_view fragment_shader_path)
    {   
        size_t h = generate_hash(vertex_shader_path, fragment_shader_path);
        std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertex_shader_path, fragment_shader_path);
        active_shaders.try_emplace(h, shader);
        return shader;
    }

    std::shared_ptr<Shader> get_shader(std::string_view vertex_shader_path, std::string_view fragment_shader_path)
    {   
        size_t h = generate_hash(vertex_shader_path, fragment_shader_path);
        auto it = active_shaders.find(h);
        if (it != active_shaders.end()) {
            return it->second;
        } else {
            return load_shader(vertex_shader_path, fragment_shader_path);  
        }
    }
};

#endif