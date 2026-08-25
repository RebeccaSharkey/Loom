// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <functional>
#include <utility>

#include "LoomEngine.h"

namespace Loom
{
    struct WindowSpecification
    {
        std::string Title;
        uint32 Width;
        uint32 Height;
        bool VSync;

        WindowSpecification(const std::string& title = "Loom Engine", const uint32 width = 1280, const uint32 height = 720, const bool vSync = true)
            : Title(title), Width(width), Height(height), VSync(vSync) {};
    };

    // Interface for desktop window
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(const void*)>;

        static Window* Create(const WindowSpecification& spec = WindowSpecification());
        virtual ~Window() = default;

        virtual void PollEvents() = 0;
        virtual void OnUpdate() = 0;

        virtual uint32 GetWidth() const = 0;
        virtual void SetWidth(uint32 width) = 0;

        virtual uint32 GetHeight() const = 0;
        virtual void SetHeight(uint32 height) = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;
    };
}
