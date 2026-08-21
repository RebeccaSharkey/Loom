// © 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Window/Window.h"

#include "../Utilities/PlatformUtils.h"

namespace Loom
{
    class SDLWindow final : public Window
    {
    public:

        explicit SDLWindow(const WindowSpecification& spec);
        ~SDLWindow() override;

    private:
        SDLWindowPtr window;

        void CreateSDLWindow(const WindowSpecification &spec);


        // --- Event Handling ---

        EventCallbackFn m_EventCallback;

        template<typename EventT>
        void DispatchEvent(const EventT& event)
        {
            if (m_EventCallback)
            {
                m_EventCallback(event);
            }
        }

    public:
        void SetEventCallback(EventCallbackFn callback) override;

        virtual void OnUpdate() override;
        virtual void PollEvents() override;

        void SetWidth(uint32 width) override;
        uint32 GetWidth() const override;

        void SetHeight(uint32 height) override;
        uint32 GetHeight() const override;

        void SetVSync(bool enabled) override;
        bool IsVSync() const override;
    };
}