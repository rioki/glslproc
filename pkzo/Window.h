
#ifndef _GLSLPROC_WINDOW_H_
#define _GLSLPROC_WINDOW_H_

#include "config.h"

#include <functional>
#include <windows.h>
#include <rgm/rgm.h>

#include "Texture.h"

namespace pkzo
{
    class PKZO_EXPORT Window
    {
    public:
        Window(const std::string& title, rgm::ivec2 pos, rgm::uvec2 size);

        Window(const Window&);

        ~Window();

        const Window& operator = (const Window&);

        void show(int cmd = SW_SHOW);

        void hide();

        void close();

        void on_draw(std::function<void ()> cb);

        void draw();

        Texture read_color();
        
        void run();

    private:
        HWND  hwnd;
        HDC   hdc;
        HGLRC hrc;

        rgm::uvec2 size;

        std::function<void ()> draw_cb;
    };
}

#endif
