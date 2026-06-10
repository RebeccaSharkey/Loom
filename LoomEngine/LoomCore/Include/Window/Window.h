// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <functional>

#include "LoomEngine.h"
#include "Events/IEvent.h"


namespace Loom
{
    struct WindowSpecification
    {
        std::string Title;
        uint32 Width;
        uint32 Height;
        bool VSync = true;

        WindowSpecification(const std::string& title = "Loom Engine", uint32 width = 1280, uint32 height = 720, bool vSync = true)
            : Title(title), Width(width), Height(height), VSync(vSync)
        {
        }
    };

    // Interface for desktop window
    class LOOM_API Window
    {
    public:
        using EventCallbackFn = std::function<void(const IEvent&)>;

        static Window* Create(const WindowSpecification& spec = WindowSpecification());

        virtual ~Window() = default;

        virtual void SetEventCallback(EventCallbackFn callback) = 0;

        virtual void PollEvents() = 0;

        virtual void OnUpdate() = 0;

        virtual uint32 GetWidth() const = 0;
        virtual uint32 GetHeight() const = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;
    };
}
