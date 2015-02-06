
#ifndef _PKZO_FRAME_BUFFER_H_
#define _PKZO_FRAME_BUFFER_H_

#include <rgm/rgm.h>

#include "config.h"
#include "Texture.h"

namespace pkzo
{
    class PKZO_EXPORT FrameBuffer
    {
    public:
        
        FrameBuffer(rgm::uvec2 size);

        FrameBuffer(const FrameBuffer&) = delete;

        ~FrameBuffer();

        const FrameBuffer& operator = (const FrameBuffer&) = delete;

        Texture& get_depth();

        Texture& get_color();        

        void bind();

        void clear();

    private:
        unsigned int id;
        rgm::uvec2 size;
        Texture depth;
        Texture color;
    };
}

#endif
