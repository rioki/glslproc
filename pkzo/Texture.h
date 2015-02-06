
#ifndef _PKZO_TEXTURE_H_
#define _PKZO_TEXTURE_H_

#include "config.h"

#include <string>
#include <vector>
#include <rgm/rgm.h>

namespace pkzo
{
    enum ColorFormat
    {
        NOCF,
        DEPTH,
        RGB,
        RGBA
    };

    class PKZO_EXPORT Texture
    {
    public:

        Texture();

        Texture(rgm::uvec2 size, ColorFormat format);

        Texture(rgm::uvec2 size, ColorFormat format, std::vector<unsigned char>&& data);

        Texture(const Texture&) = delete;

        Texture(Texture&& other);

        ~Texture();

        const Texture& operator = (const Texture&) = delete;

        const Texture& operator = (Texture&& other);

        unsigned int get_glid() const;

        rgm::uvec2 get_size() const;

        ColorFormat get_format() const;

        const unsigned char* get_data() const;

        void upload();

        void release();

        void bind(unsigned int channel);

        void load(const std::string& file);

        void save(const std::string& file);

        void readback();

    private:
        unsigned int               glid;
        rgm::uvec2                 size;
        ColorFormat                format;
        std::vector<unsigned char> data;
    };

}

#endif
