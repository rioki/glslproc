
#include "FrameBuffer.h"

#include <GL/glew.h>

namespace pkzo
{
    FrameBuffer::FrameBuffer(rgm::uvec2 s)
    : id(0), size(s), depth(size, DEPTH), color(size, RGBA)
    {
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);
                
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.get_glid(), 0);        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.get_glid(), 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);    
    }


    FrameBuffer::~FrameBuffer()
    {
        glDeleteFramebuffers(1, &id);
    }

    Texture& FrameBuffer::get_depth()
    {
        return depth;
    }

    Texture& FrameBuffer::get_color()
    {
        return color;
    }

    void FrameBuffer::bind() 
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glViewport(0, 0, size[0], size[1]);
    }

    void FrameBuffer::clear()
    {
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }
}
