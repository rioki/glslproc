#include "Window.h"

#include <GL/glew.h>
#include <GL/wglew.h>

#include "utils.h"

namespace pkzo
{
    LRESULT	CALLBACK ogl_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch(msg)
        {
            case WM_CREATE:
            {
                CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lparam);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
                break;
            }            
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
        }

        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    void create_ogl_window_class(HINSTANCE hInst)
    {
        static bool done = false;

        if (done == false)
        {
            WNDCLASSEX wc;
            ZeroMemory(&wc, sizeof(WNDCLASSEX));
            wc.cbClsExtra    = NULL;
            wc.cbSize        = sizeof(WNDCLASSEX);
            wc.cbWndExtra    = NULL;
            wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
            wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
            wc.hIcon         = NULL;
            wc.hIconSm       = NULL;
            wc.hInstance     = hInst;
            wc.lpfnWndProc   = (WNDPROC)ogl_wnd_proc;
            wc.lpszClassName = L"OGL_WINDOW";
            wc.lpszMenuName  = NULL;
            wc.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;

            if(!RegisterClassEx(&wc))
            {
                throw std::runtime_error(get_last_error());
            }

            done = true;
        }
    }

    Window::Window(const std::string& title, rgm::ivec2 pos, rgm::uvec2 s)
    : hwnd(NULL), hdc(NULL), hrc(NULL), size(s)
    {
        // TODO clean up when an exception happens

        HINSTANCE hInstance = GetModuleHandle(NULL);
        create_ogl_window_class(hInstance);
    
        hwnd = CreateWindowEx(NULL, L"OGL_WINDOW", widen(title).c_str(), WS_OVERLAPPEDWINDOW, pos[0], pos[1], size[0], size[1], NULL, NULL, hInstance, this);
        if(hwnd == NULL)
        {
            throw std::runtime_error(get_last_error());
        }
        
        hdc = GetDC(hwnd);
        if (hdc == NULL)
        {
            throw std::runtime_error(get_last_error());
        }
        
        PIXELFORMATDESCRIPTOR pfd; 
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));                          
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);                               
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;                                          
        pfd.cColorBits = 32;                                                     
        pfd.cDepthBits = 32;                                                     
        pfd.iLayerType = PFD_MAIN_PLANE;                                         

        int pf = ChoosePixelFormat(hdc, &pfd);
        if (pf == 0)
        {
            throw std::runtime_error(get_last_error());
        }

        BOOL br = SetPixelFormat(hdc, pf, &pfd);
        if (br == FALSE)
        {
            throw std::runtime_error(get_last_error());
        }

        HGLRC tmprc = wglCreateContext(hdc);
        if (tmprc == NULL)
        {
            throw std::runtime_error(get_last_error());
        }

        br = wglMakeCurrent(hdc, tmprc);
        if (br == FALSE)
        {
            throw std::runtime_error(get_last_error());
        }

        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            throw std::runtime_error((const char*)glewGetErrorString(err));
        }  
        
        int attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            WGL_CONTEXT_FLAGS_ARB, 
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
            0
        };

        hrc = wglCreateContextAttribsARB(hdc, NULL, attributes);
        if (hrc == NULL)
        {
            throw std::runtime_error(get_last_error());
        }

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tmprc);
        wglMakeCurrent(hdc, hrc);

        int glVersion[2] = {-1, -1}; 
        glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
        glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

        glViewport(0, 0, size[0], size[1]);
    }        

    Window::~Window()
    {
        wglDeleteContext(hrc);
        DestroyWindow(hwnd);
    }

    void Window::show(int cmd)
    {
        ShowWindow(hwnd, cmd);
    }

    void Window::hide()
    {
        show(SW_HIDE);
    }

    void Window::close()
    {
        DestroyWindow(hwnd);
    }

    void Window::on_draw(std::function<void ()> cb)
    {
        draw_cb = cb;
    }

    void Window::draw()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, size[0], size[1]);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        if (draw_cb)
        {
            draw_cb();
        }

        SwapBuffers(hdc); 
    }

    Texture Window::read_color()
    {
        std::vector<unsigned char> data(size[0] * size[1] * 4);        
        glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return Texture(size, RGBA, std::move(data));
    }


    void Window::run()
    {
        MSG msg;
        ZeroMemory(&msg,sizeof(MSG));

        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                draw();
            }
        }        
    }
}
