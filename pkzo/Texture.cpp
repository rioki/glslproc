
#include "Texture.h"

#include <cstdio>
#include <GL/glew.h>
#include <png.h>

#include "path.h"
#include "compose.h"

namespace pkzo
{
    Texture::Texture() 
    : glid(0), size(0, 0) {}

    Texture::Texture(rgm::uvec2 s, ColorFormat f)
    : glid(0), size(s), format(f) {}

    Texture::Texture(rgm::uvec2 s, ColorFormat f, std::vector<unsigned char>&& d)
    : glid(0), size(s), format(f), data(d) {}

    Texture::Texture(Texture&& other)
    : glid(other.glid), size(other.size), format(other.format), data(other.data)
    {
        other.glid   = 0;
        other.size   = rgm::uvec2(0, 0);
        other.format = NOCF;
    }
        

    Texture::~Texture() 
    {
        release();         
    }

    const Texture& Texture::operator = (Texture&& other)
    {
        glid   = other.glid;
        size   = other.size;
        format = other.format;
        
        other.glid   = 0;
        other.size   = rgm::uvec2(0, 0);
        other.format = NOCF;

        data   = std::move(other.data);

        return *this;
    }

    unsigned int Texture::get_glid() const
    {
        if (glid == 0)
        {
            const_cast<Texture*>(this)->upload();
        }
        return glid;
    }

    rgm::uvec2 Texture::get_size() const
    {
        return size;
    }

    ColorFormat Texture::get_format() const
    {
        return format;
    }

    const unsigned char* Texture::get_data() const
    {
        return &data[0];
    }

    void Texture::upload()
    {
        if (glid != 0)
        {
            return;
        }
        
        glGenTextures(1, &glid);                    
        glBindTexture(GL_TEXTURE_2D, glid);
        
        // TODO filter modes...
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        //float aniso = 0.0f;
        //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        
        int mode = 0;
        switch (format)
        {
            case DEPTH:
                mode = GL_DEPTH;
                break;
            case RGB:
                mode = GL_RGB;    
                break;
            case RGBA:
                mode = GL_RGBA;
                break;
            default:
                throw std::logic_error("Unknown pixel format.");
        }
        
        void* d = !data.empty() ? &data[0] : NULL;

        // TOOD different format types...
        glTexImage2D(GL_TEXTURE_2D, 0, mode, size[0], size[1], 0, mode, GL_UNSIGNED_BYTE, d);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::release()
    {
        if (glid != 0)
        {
            glDeleteTextures(1, &glid);
            glid = 0;
        } 
    }

    void Texture::bind(unsigned int channel)
    {
        upload();

        glActiveTexture(GL_TEXTURE0 + channel);
        glBindTexture(GL_TEXTURE_2D, glid);
    }

    Texture load_png(const std::string& file)
    {
        unsigned char header[8];

        FILE *fp = fopen(file.c_str(), "rb");
        if (fp == NULL)
        {
            throw std::runtime_error(compose("Failed to open %0 for reading.", file));
        }
        fread(header, 1, 8, fp);
               
        if (png_sig_cmp(header, 0, 8))
        {
            throw std::runtime_error(compose("%0 is not a PNG.", file));
        }
        
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop info_ptr = png_create_info_struct(png_ptr);    
        
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            throw std::runtime_error(compose("Error while reading %0.", file));
        }
        
        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        int      width      = png_get_image_width(png_ptr, info_ptr);
        int      height     = png_get_image_height(png_ptr, info_ptr);
        png_byte color_type = png_get_color_type(png_ptr, info_ptr);
        png_byte bit_depth  = png_get_bit_depth(png_ptr, info_ptr);

        
        ColorFormat cf;
        switch (color_type)
        {
            case PNG_COLOR_TYPE_RGB:
                cf = RGB;
                break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                cf = RGBA;
                break;
            default:
                throw std::runtime_error("Unsupported color type.");
        }

        if (bit_depth != 8)
        {
            // yes, yes... 
            throw std::runtime_error("Unsupported color depth.");
        }

        int number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);                

        std::vector<png_byte> buffer(height * png_get_rowbytes(png_ptr, info_ptr));
        std::vector<png_bytep> ptrs(height);

        for (int y = 0; y < height; y++)
        {
            ptrs[y] = &buffer[y * png_get_rowbytes(png_ptr, info_ptr)];
        }                
        png_read_image(png_ptr, &ptrs[0]);

        fclose(fp);
        
        return Texture(rgm::uvec2(width, height), cf, std::move(buffer));    
    }

    void write_png(Texture& texture, const std::string& file)
    {
        FILE *fp = fopen(file.c_str(), "wb");
        if (fp == NULL)
        {
            throw std::runtime_error(compose("Failed to open %0 for writing.", file));
        }

        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop info_ptr = png_create_info_struct(png_ptr);    

        if (setjmp(png_jmpbuf(png_ptr)))
        {
             throw std::runtime_error(compose("Error while writing %0.", file));        
        }

        png_init_io(png_ptr, fp);

        int      width      = texture.get_size()[0];
        int      height     = texture.get_size()[0];
        png_byte color_type = texture.get_format() == RGB ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;
        png_byte bit_depth  = 8;

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        
        png_write_info(png_ptr, info_ptr);


        png_byte* buffer = (png_byte*)texture.get_data();
        std::vector<png_bytep> ptrs(height);

        for (int y = 0; y < height; y++)
        {
            ptrs[y] = &buffer[y * png_get_rowbytes(png_ptr, info_ptr)];
        }
        png_write_image(png_ptr, &ptrs[0]);

        png_write_end(png_ptr, NULL);

        fclose(fp);
    }

    void Texture::load(const std::string& file)
    {
        std::string ext = path::ext(file); // tolower
        
        if (ext == "png")
        {
            *this = load_png(file);
        }
        else
        {
            throw std::logic_error(compose("Unknown texture extention %0.", ext));
        }
    }

    void Texture::save(const std::string& file)
    {
        std::string ext = path::ext(file); // tolower
        
        if (ext == "png")
        {
            write_png(*this, file);
        }
        else
        {
            throw std::logic_error(compose("Unknown texture extention %0.", ext));
        }
    }

    void Texture::readback()
    {
        int mode = 0;
        size_t isize = 0;
        switch (format)
        {            
            case RGB:
                mode = GL_RGB;    
                isize = 3;
                break;
            case RGBA:
                mode = GL_RGBA;
                isize = 4;
                break;
            default:
                throw std::logic_error("Unknown pixel format.");
        }

        if (data.empty())
        {
            data.resize(size[0] * size[1] * isize);
        }

        glBindTexture(GL_TEXTURE_2D, glid);
        glGetTexImage(GL_TEXTURE_2D, 0, mode, GL_UNSIGNED_BYTE, &data[0]);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
