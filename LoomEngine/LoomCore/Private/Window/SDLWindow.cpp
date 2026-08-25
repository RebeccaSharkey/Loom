// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "SDLWindow.h"
#include "SDL3/SDL.h"
#include "Events/Events/WindowEvents.h"


namespace Loom
{
    Window *Window::Create(const WindowSpecification &spec)
    {
        return new SDLWindow(spec);
    }

    SDLWindow::SDLWindow(const WindowSpecification &spec)
    {
        CreateSDLWindow(spec);
    }

    SDLWindow::~SDLWindow()
    {
        window.reset();
    }

    void SDLWindow::CreateSDLWindow(const WindowSpecification &spec)
    {
        window.reset(SDL_CreateWindow(spec.Title.c_str(), spec.Width, spec.Height, SDL_WINDOW_RESIZABLE));
        LOOM_VERIFY(window.get(), SDL_GetError());
    }

    void SDLWindow::OnUpdate()
    {

    }

    void SDLWindow::PollEvents()
    {
        SDL_Event event{0};
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {

                case SDL_EVENT_QUIT:
                    DispatchEvent(WindowCloseEvent());
                    break;

                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    DispatchEvent(WindowGainedFocusEvent());
                    break;

                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    DispatchEvent(WindowLostFocusEvent());
                    break;

                default:
                    break;
            }
        }
    }

    void SDLWindow::SetWidth(uint32 width)
    {

    }

    uint32 SDLWindow::GetWidth() const
    {
        return 0;
    }

    void SDLWindow::SetHeight(uint32 height)
    {

    }

    uint32 SDLWindow::GetHeight() const
    {
        return 0;
    }

    void SDLWindow::SetVSync(bool enabled)
    {

    }

    bool SDLWindow::IsVSync() const
    {
        return false;
    }
}
