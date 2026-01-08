// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "../../../Include/Window/PlatformCreation/WindowsWindow.h"

#ifdef LOOM_PLATFORM_WINDOWS

#include "LoomEngine.h"
#include "Events/EventDispatcher.h"
#include <windowsx.h>

namespace Loom
{
    Window* Window::Create(const WindowSpecification& spec)
    {
        return new WindowsWindow(spec);
    }

    WindowsWindow::WindowsWindow(const WindowSpecification& spec)
        : m_Width(spec.Width)
        , m_Height(spec.Height)
        , m_VSync(spec.VSync)
        , m_Instance(GetModuleHandle(nullptr))
    {
        LOOM_LOG_TRACE("Window", "Creating Windows window: %s (%ux%u)", spec.Title.c_str(), spec.Width, spec.Height);

        RegisterWindowClass();
        CreateWindowHandle(spec);

        LOOM_LOG_INFO("Window", "Window created successfully");
    }

    WindowsWindow::~WindowsWindow()
    {
        LOOM_LOG_TRACE("Window", "Destroying Windows window");

        if (m_WindowHandle)
        {
            DestroyWindow(m_WindowHandle);
            m_WindowHandle = nullptr;
        }

        UnregisterClassW(s_WindowClassName, m_Instance);
    }

    void WindowsWindow::RegisterWindowClass()
    {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = StaticWindowProc;
        wc.hInstance = m_Instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
        wc.lpszClassName = s_WindowClassName;

        ATOM result = RegisterClassExW(&wc);
        LOOM_ASSERT(result != 0, "Failed to register window class");
    }

    void WindowsWindow::CreateWindowHandle(const WindowSpecification& spec)
    {
        // Calculate window size with borders to achieve the desired client area
        DWORD style = WS_OVERLAPPEDWINDOW;
        DWORD exStyle = WS_EX_APPWINDOW;

        RECT rect = { 0, 0, static_cast<LONG>(spec.Width), static_cast<LONG>(spec.Height) };
        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        // Convert UTF-8 title to wide string
        int wideLength = MultiByteToWideChar(CP_UTF8, 0, spec.Title.c_str(), -1, nullptr, 0);
        wchar_t* wideTitle = new wchar_t[wideLength];
        MultiByteToWideChar(CP_UTF8, 0, spec.Title.c_str(), -1, wideTitle, wideLength);

        m_WindowHandle = CreateWindowExW(
            exStyle,
            s_WindowClassName,
            wideTitle,
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            m_Instance,
            this
        );

        delete[] wideTitle;

        LOOM_ASSERT(m_WindowHandle != nullptr, "Failed to create window");
        ShowWindow(m_WindowHandle, SW_SHOW);
        UpdateWindow(m_WindowHandle);
    }

    void WindowsWindow::ProcessEvents()
    {
        MSG msg = {};
        while (PeekMessageW(&msg, m_WindowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    LRESULT CALLBACK WindowsWindow::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        WindowsWindow* window = nullptr;

        if (msg == WM_CREATE)
        {
            CREATESTRUCTW* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
            window = static_cast<WindowsWindow*>(createStruct->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        }
        else
        {
            window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (window)
        {
            return window->WindowProc(hwnd, msg, wParam, lParam);
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    LRESULT WindowsWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
            case WM_CLOSE:
            {
                m_ShouldClose = true;
                return 0;
            }

            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }

            case WM_SIZE:
            {
                m_Width = LOWORD(lParam);
                m_Height = HIWORD(lParam);
                return 0;
            }

            case WM_SETFOCUS:
            {
                return 0;
            }

            case WM_KILLFOCUS:
            {
                return 0;
            }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                return 0;
            }

            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                return 0;
            }

            case WM_CHAR:
            {
                return 0;
            }

            case WM_MOUSEMOVE:
            {
                return 0;
            }

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            {
                return 0;
            }

            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            {
                return 0;
            }

            case WM_MOUSEWHEEL:
            {
                return 0;
            }
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

#endif // LOOM_PLATFORM_WINDOWS
