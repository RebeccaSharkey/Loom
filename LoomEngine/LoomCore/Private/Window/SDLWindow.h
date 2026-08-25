// © 2025 Ctrl Alt Delete Games. All rights reserved.
#pragma once

#include "Window/Window.h"

#include "../Utilities/PlatformUtils.h"
#include "Events/EventDispatcher.h"

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

        template<typename EventT>
        void DispatchEvent(const EventT& event)
        {
            EventDispatcher::Broadcast(event);
        }

    public:
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
