// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "../Window.h"

#ifdef LOOM_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace Loom
{
    /// Windows-specific window implementation using Win32 API
    class WindowsWindow final : public Window
    {
    public:
        explicit WindowsWindow(const WindowSpecification& spec);
        ~WindowsWindow() override;

        void ProcessEvents() override;
        void SwapBuffers() override { /* Implemented by render API */ }

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        void* GetNativeWindow() const override { return m_WindowHandle; }
        bool ShouldClose() const override { return m_ShouldClose; }

        void SetVSync(bool enabled) override { m_VSync = enabled; }
        bool IsVSync() const override { return m_VSync; }

    private:
        /// Win32 window procedure callback
        static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void RegisterWindowClass();
        void CreateWindowHandle(const WindowSpecification& spec);

    private:
        HWND m_WindowHandle = nullptr;
        HINSTANCE m_Instance = nullptr;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        bool m_VSync = true;
        bool m_ShouldClose = false;

        static constexpr const wchar_t* s_WindowClassName = L"LoomWindowClass";
    };
}

#endif // LOOM_PLATFORM_WINDOWS
