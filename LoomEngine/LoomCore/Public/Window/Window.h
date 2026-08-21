// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <functional>
#include <utility>

#include "LoomEngine.h"

namespace Loom
{
    class IEvent;

    struct WindowSpecification
    {
        using EventCallbackFn = std::function<void(const IEvent&)>;

        std::string Title;
        uint32 Width;
        uint32 Height;
        bool VSync;

        EventCallbackFn EventCallback;

        WindowSpecification(const std::string& title = "Loom Engine", const uint32 width = 1280, const uint32 height = 720,
            const bool vSync = true, EventCallbackFn callback = nullptr)
            : Title(title), Width(width), Height(height), VSync(vSync), EventCallback(std::move(callback))
        {
        }
    };

    // Interface for desktop window
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(const IEvent&)>;

        static Window* Create(const WindowSpecification& spec = WindowSpecification());
        virtual ~Window() = default;

        virtual void PollEvents() = 0;
        virtual void OnUpdate() = 0;

        virtual void SetEventCallback(EventCallbackFn callback) = 0;

        virtual uint32 GetWidth() const = 0;
        virtual void SetWidth(uint32 width) = 0;

        virtual uint32 GetHeight() const = 0;
        virtual void SetHeight(uint32 height) = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;
    };
}
