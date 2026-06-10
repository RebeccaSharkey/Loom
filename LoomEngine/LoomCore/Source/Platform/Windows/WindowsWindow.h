// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Window/Window.h"

#ifdef LOOM_PLATFORM_WINDOWS
#include <utility>
#include <Windows.h>

namespace Loom
{
    class WindowsWindow final : public Window
    {
    public:
        explicit WindowsWindow(const WindowSpecification& spec);
        ~WindowsWindow() override;

    private:
        static constexpr const wchar_t* s_WindowClassName = L"LoomWindowClass";
        HWND m_WindowHandle = nullptr;
        HINSTANCE m_InstanceHandle = GetModuleHandleW(nullptr);
        EventCallbackFn m_EventCallback;

        void RegisterWindowClass();
        void CreateWindowHandle(const WindowSpecification& spec);

        static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        template<typename EventT>
        void DispatchEvent(const EventT& event)
        {
            if (m_EventCallback)
                m_EventCallback(event);
        }

    public:
        void SetEventCallback(EventCallbackFn callback) override { m_EventCallback = std::move(callback); }

        void PollEvents() override;

        void OnUpdate() override;

    private:
        uint32 m_Width = 0;
        uint32 m_Height = 0;
        bool m_VSync = true;
        bool m_HasLastMousePosition = false;
        float32 m_LastMouseX = 0.0f;
        float32 m_LastMouseY = 0.0f;

    public:
        uint32 GetWidth() const override { return m_Width; };
        uint32 GetHeight() const override { return m_Height; };

        void SetVSync(bool enabled) override { m_VSync = enabled; };
        bool IsVSync() const override { return m_VSync; };
    };
}


#endif
