
#include "Shader.h"

#include <stdexcept>
#include <GL/glew.h>

#include "fs.h"

namespace pkzo
{
    Shader::Shader()
    : program_id(0) {}

    Shader::~Shader()
    {
        release();
    }

    void Shader::set_vertex_code(const std::string& value)
    {
        vertex_code = value;
    }

    const std::string& Shader::get_vertex_code() const
    {
        return vertex_code;
    }

    void Shader::set_fragment_code(const std::string& value)
    {
        fragment_code = value;
    }

    const std::string& Shader::get_fragment_code() const
    {
        return fragment_code;
    }

    void Shader::load(const std::string vertex_file, const std::string& fragment_file)
    {
        set_vertex_code(fs::read(vertex_file));
        set_fragment_code(fs::read(fragment_file));
    }

    void Shader::compile() const
    {
        if (program_id != 0)
        {
            return;
        }

        int status = 0;
        char logstr[256];
        
        const GLchar* vbuff[1] = {vertex_code.c_str()};

        unsigned int vertex_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_id, 1, vbuff, NULL);
        glCompileShader(vertex_id);
        
        glGetShaderInfoLog(vertex_id, 256, NULL, logstr);

        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &status);
        if(! status)
        {
            glDeleteShader(vertex_id);    
            throw std::runtime_error(logstr);
        }

        const GLchar* fbuff[1] = {fragment_code.c_str()};

        unsigned int fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_id, 1, fbuff, NULL);
        glCompileShader(fragment_id);

        glGetShaderInfoLog(fragment_id, 256, NULL, logstr);

        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &status);
        if(! status)
        {            
            glDeleteShader(vertex_id);
            glDeleteShader(fragment_id);
            throw std::runtime_error(logstr);
        }

        program_id = glCreateProgram();
        glAttachShader(program_id, vertex_id);
        glAttachShader(program_id, fragment_id);
        glLinkProgram(program_id);

        glGetShaderInfoLog(program_id, 256, NULL, logstr);

        glGetShaderiv(program_id, GL_LINK_STATUS, &status);
        if(! status)
        {            
            glDeleteShader(vertex_id);
            glDeleteShader(fragment_id);
            glDeleteProgram(program_id);
            throw std::runtime_error(logstr);
        }

        // NOTE: glDeleteShader() actually does not delete the shader, it only
        // flags the shader for deletion. The shaders will be deleted when
        // the program gets deleted.
        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
    }

    void Shader::bind() const
    {
        compile();
        glUseProgram(program_id);
    }

    void Shader::unbind() const
    {
        glUseProgram(0);
    }
        
    void Shader::release() const
    {
        if (program_id != 0)
        {
            glDeleteProgram(program_id);
            program_id = 0;
        }
    }

    int Shader::get_attribute_location(const std::string& name) const
    {
        return glGetAttribLocation(program_id, name.c_str());
    }

    void Shader::set_uniform(const std::string& name, int value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform1i(location, value);
        }        
    }

    void Shader::set_uniform(const std::string& name, float value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform1f(location, value);
        } 
    }
    
    void Shader::set_uniform(const std::string& name, unsigned int value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform1ui(location, value);
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::vec2 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform2fv(location, 1, value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::vec3 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform3fv(location, 1, value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::vec4 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform4fv(location, 1, value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::ivec2 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform2iv(location, 1, value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::ivec3 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform3iv(location, 1, value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::ivec4 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform4iv(location, 1, value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::uvec2 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform2iv(location, 1, (const GLint*)value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::uvec3 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform3iv(location, 1, (const GLint*)value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, rgm::uvec4 value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniform4iv(location, 1, (const GLint*)value.c_array());
        }  
    }

    void Shader::set_uniform(const std::string& name, const rgm::mat3& value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniformMatrix3fv(location, 1, GL_FALSE, value.c_array());
        }
    }

    void Shader::set_uniform(const std::string& name, const rgm::mat4& value) const
    {
        int location = glGetUniformLocation(program_id, name.c_str());
        if (location != -1)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, value.c_array());
        }
    }
}
