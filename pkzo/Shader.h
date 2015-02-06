
#ifndef _PKZO_SHADER_H_
#define _PKZO_SHADER_H_

#include <string>

#include <rgm/rgm.h>

#include "config.h"

namespace pkzo
{
    class PKZO_EXPORT Shader
    {
    public:

        Shader();

        ~Shader();

        void set_vertex_code(const std::string& value);

        const std::string& get_vertex_code() const;

        void set_fragment_code(const std::string& value);

        const std::string& get_fragment_code() const;

        void load(const std::string vertex_file, const std::string& fragment_file);

        void compile() const;

        void bind() const;

        void unbind() const;
        
        void release() const;
        
        int get_attribute_location(const std::string& name) const;

        void set_uniform(const std::string& name, int value) const;

        void set_uniform(const std::string& name, unsigned int value) const;

        void set_uniform(const std::string& name, float value) const;

        void set_uniform(const std::string& name, rgm::vec2 value) const;

        void set_uniform(const std::string& name, rgm::vec3 value) const;

        void set_uniform(const std::string& name, rgm::vec4 value) const;

        void set_uniform(const std::string& name, rgm::ivec2 value) const;

        void set_uniform(const std::string& name, rgm::ivec3 value) const;

        void set_uniform(const std::string& name, rgm::ivec4 value) const;

        void set_uniform(const std::string& name, rgm::uvec2 value) const;

        void set_uniform(const std::string& name, rgm::uvec3 value) const;

        void set_uniform(const std::string& name, rgm::uvec4 value) const;

        void set_uniform(const std::string& name, const rgm::mat3& value) const;

        void set_uniform(const std::string& name, const rgm::mat4& value) const;

    private:
        std::string vertex_code;
        std::string fragment_code;
        mutable unsigned int program_id;

        Shader(const Shader&) = delete;
        const Shader& operator = (const Shader&) = delete;
    };
}

#endif
