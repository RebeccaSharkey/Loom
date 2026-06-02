// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "WindowsWindow.h"

#ifdef LOOM_PLATFORM_WINDOWS

#include "LoomEngine.h"
#include <windows.h>

#include "../../../Include/Events/Events/WindowsEvents.h"
#include "Events/EventDispatcher.h"

namespace Loom {
    Window *Window::Create(const WindowSpecification &spec)
    {
        return new WindowsWindow(spec);
    }

    WindowsWindow::WindowsWindow(const WindowSpecification &spec)
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

        UnregisterClassW(s_WindowClassName, m_InstanceHandle);
    }

    void WindowsWindow::RegisterWindowClass()
    {
        WNDCLASSEXW windowClass = {};

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = StaticWindowProc;
        windowClass.hInstance = m_InstanceHandle;
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        windowClass.lpszClassName = s_WindowClassName;

        ATOM result = RegisterClassExW(&windowClass);
        LOOM_ASSERT(result, "Failed to register Window Class");
    }

    void WindowsWindow::CreateWindowHandle(const WindowSpecification& spec)
    {
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
            m_InstanceHandle,
            this
        );

        delete[] wideTitle;

        LOOM_ASSERT(m_WindowHandle != nullptr, "Failed to create window");

        ShowWindow(m_WindowHandle, SW_SHOW);
        UpdateWindow(m_WindowHandle);
    }

    LRESULT WindowsWindow::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

    LRESULT WindowsWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_CLOSE:
            {
                WindowCloseEvent event;
                EventDispatcher::Enqueue<WindowCloseEvent>(event);
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
                WindowResizeEvent event(m_Width, m_Height);
                EventDispatcher::Enqueue<WindowResizeEvent>(event);
                return 0;
            }

            case WM_SETFOCUS:
            {
                WindowFocusEvent event;
                EventDispatcher::Enqueue<WindowFocusEvent>(event);
                return 0;
            }

            case WM_KILLFOCUS:
            {
                WindowLostFocusEvent event;
                EventDispatcher::Enqueue<WindowLostFocusEvent>(event);
                return 0;
            }

            default:
            {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }

        return 0;
    }

    void WindowsWindow::PollEvents()
    {
        MSG msg = {};
        while (PeekMessageW(&msg, m_WindowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    void WindowsWindow::OnUpdate()
    {
    }
}

#endif

