// © 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Window/Window.h"

struct SDL_Window;

namespace Loom
{
    class SDLWindow final : public Window
    {
    public:
        explicit SDLWindow(const WindowSpecification& spec);
        ~SDLWindow();

    private:
        SDL_Window* window = nullptr;
        EventCallbackFn m_EventCallback;

        void CreateSDLWindow(const WindowSpecification &spec);

        template<typename EventT>
        void DispatchEvent(const EventT& event)
        {
            if (m_EventCallback)
                m_EventCallback(event);
        }

    public:
        virtual void SetEventCallback(EventCallbackFn callback) override { m_EventCallback = std::move(callback); }

        virtual void OnUpdate() override;
        virtual void PollEvents() override;

        uint32 GetWidth() const override;
        uint32 GetHeight() const override;

        void SetVSync(bool enabled) override;
        bool IsVSync() const override;
    };
}