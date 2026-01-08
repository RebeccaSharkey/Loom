// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "LoomEngine.h"
#include <string>

namespace Loom
{
    struct WindowSpecification
    {
        std::string Title = "Loom Engine";
        uint32_t Width = 1280;
        uint32_t Height = 720;
        bool VSync = true;
    };

    class LOOM_API Window
    {
    public:
        virtual ~Window() = default;

        virtual void ProcessEvents() = 0;

        virtual void SwapBuffers() = 0;

        virtual uint32_t GetWidth() const = 0;

        virtual uint32_t GetHeight() const = 0;

        virtual void* GetNativeWindow() const = 0;

        virtual bool ShouldClose() const = 0;

        virtual void SetVSync(bool enabled) = 0;

        virtual bool IsVSync() const = 0;

        static Window* Create(const WindowSpecification& spec = WindowSpecification());
    };
}
