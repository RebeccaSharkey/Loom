// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "SDLWindow.h"
#include "SDL3/SDL_main.h"

#include "Events/Events/WindowEvents.h"

namespace Loom
{

    Window *Window::Create(const WindowSpecification &spec)
    {
        return new SDLWindow(spec);
    }

    SDLWindow::SDLWindow(const WindowSpecification &spec)
    {
        LOOM_LOG_TRACE("SDLWindow", "Creating SDL Window: %s (%ux%u)", spec.Title.c_str(), spec.Width, spec.Height);

        SDL_SetMainReady();

        LOOM_ASSERT(SDL_Init(SDL_INIT_VIDEO), "Failed to Init SDL. Error: %s", SDL_GetError());

        CreateSDLWindow(spec);

        LOOM_LOG_INFO("SDLWindow", "SDL Window created successfully");
    }

    SDLWindow::~SDLWindow()
    {
        LOOM_LOG_TRACE("SDLWindow", "Destroying SDL Window");

        SDL_DestroyWindow(window);
        window = nullptr;

        SDL_Quit();
    }

    void SDLWindow::CreateSDLWindow(const WindowSpecification &spec)
    {
        window = SDL_CreateWindow(spec.Title.c_str(), spec.Width, spec.Height, 0);
        LOOM_ASSERT(window, "Failed to Create SDL Window. Error: %s", SDL_GetError());
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
                    DispatchEvent(WindowGainFocusEvent());
                    break;

                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    DispatchEvent(WindowLostFocusEvent());
                    break;

                default:
                    break;
            }
        }
    }

    uint32 SDLWindow::GetWidth() const
    {
        return 0;
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
