// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "WindowsWindow.h"

#ifdef LOOM_PLATFORM_WINDOWS

#include "LoomEngine.h"
#include "Events/Events/KeyEvents.h"
#include "Events/Events/MouseEvents.h"
#include "Events/Events/WindowEvents.h"
#include <windows.h>

namespace Loom {
    namespace
    {
        KeyCode TranslateWindowsKey(WPARAM key)
        {
            switch (key)
            {
                case VK_SPACE: return KeyCode::Space;
                case VK_OEM_7: return KeyCode::Apostrophe;
                case VK_OEM_COMMA: return KeyCode::Comma;
                case VK_OEM_MINUS: return KeyCode::Minus;
                case VK_OEM_PERIOD: return KeyCode::Period;
                case VK_OEM_2: return KeyCode::Slash;

                case '0': return KeyCode::D0;
                case '1': return KeyCode::D1;
                case '2': return KeyCode::D2;
                case '3': return KeyCode::D3;
                case '4': return KeyCode::D4;
                case '5': return KeyCode::D5;
                case '6': return KeyCode::D6;
                case '7': return KeyCode::D7;
                case '8': return KeyCode::D8;
                case '9': return KeyCode::D9;

                case VK_OEM_1: return KeyCode::Semicolon;
                case VK_OEM_PLUS: return KeyCode::Equal;

                case 'A': return KeyCode::A;
                case 'B': return KeyCode::B;
                case 'C': return KeyCode::C;
                case 'D': return KeyCode::D;
                case 'E': return KeyCode::E;
                case 'F': return KeyCode::F;
                case 'G': return KeyCode::G;
                case 'H': return KeyCode::H;
                case 'I': return KeyCode::I;
                case 'J': return KeyCode::J;
                case 'K': return KeyCode::K;
                case 'L': return KeyCode::L;
                case 'M': return KeyCode::M;
                case 'N': return KeyCode::N;
                case 'O': return KeyCode::O;
                case 'P': return KeyCode::P;
                case 'Q': return KeyCode::Q;
                case 'R': return KeyCode::R;
                case 'S': return KeyCode::S;
                case 'T': return KeyCode::T;
                case 'U': return KeyCode::U;
                case 'V': return KeyCode::V;
                case 'W': return KeyCode::W;
                case 'X': return KeyCode::X;
                case 'Y': return KeyCode::Y;
                case 'Z': return KeyCode::Z;

                case VK_OEM_4: return KeyCode::LeftBracket;
                case VK_OEM_5: return KeyCode::Backslash;
                case VK_OEM_6: return KeyCode::RightBracket;
                case VK_OEM_3: return KeyCode::GraveAccent;

                case VK_ESCAPE: return KeyCode::Escape;
                case VK_RETURN: return KeyCode::Enter;
                case VK_TAB: return KeyCode::Tab;
                case VK_BACK: return KeyCode::Backspace;
                case VK_INSERT: return KeyCode::Insert;
                case VK_DELETE: return KeyCode::Delete;
                case VK_RIGHT: return KeyCode::Right;
                case VK_LEFT: return KeyCode::Left;
                case VK_DOWN: return KeyCode::Down;
                case VK_UP: return KeyCode::Up;
                case VK_PRIOR: return KeyCode::PageUp;
                case VK_NEXT: return KeyCode::PageDown;
                case VK_HOME: return KeyCode::Home;
                case VK_END: return KeyCode::End;
                case VK_CAPITAL: return KeyCode::CapsLock;
                case VK_SCROLL: return KeyCode::ScrollLock;
                case VK_NUMLOCK: return KeyCode::NumLock;
                case VK_SNAPSHOT: return KeyCode::PrintScreen;
                case VK_PAUSE: return KeyCode::Pause;

                case VK_F1: return KeyCode::F1;
                case VK_F2: return KeyCode::F2;
                case VK_F3: return KeyCode::F3;
                case VK_F4: return KeyCode::F4;
                case VK_F5: return KeyCode::F5;
                case VK_F6: return KeyCode::F6;
                case VK_F7: return KeyCode::F7;
                case VK_F8: return KeyCode::F8;
                case VK_F9: return KeyCode::F9;
                case VK_F10: return KeyCode::F10;
                case VK_F11: return KeyCode::F11;
                case VK_F12: return KeyCode::F12;

                case VK_LSHIFT: return KeyCode::LeftShift;
                case VK_LCONTROL: return KeyCode::LeftControl;
                case VK_LMENU: return KeyCode::LeftAlt;
                case VK_LWIN: return KeyCode::LeftSuper;
                case VK_RSHIFT: return KeyCode::RightShift;
                case VK_RCONTROL: return KeyCode::RightControl;
                case VK_RMENU: return KeyCode::RightAlt;
                case VK_RWIN: return KeyCode::RightSuper;
                case VK_APPS: return KeyCode::Menu;

                default: return KeyCode::Unknown;
            }
        }

        KeyCode TranslateExtendedWindowsKey(WPARAM key, LPARAM flags)
        {
            if (key == VK_SHIFT)
            {
                const UINT scanCode = (static_cast<UINT>(flags) >> 16) & 0xff;
                key = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX);
            }
            else if (key == VK_CONTROL)
            {
                key = (flags & (1 << 24)) ? VK_RCONTROL : VK_LCONTROL;
            }
            else if (key == VK_MENU)
            {
                key = (flags & (1 << 24)) ? VK_RMENU : VK_LMENU;
            }

            return TranslateWindowsKey(key);
        }

        MouseButton TranslateWindowsMouseButton(UINT message, WPARAM wParam)
        {
            switch (message)
            {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    return MouseButton::Left;

                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    return MouseButton::Right;

                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    return MouseButton::Middle;

                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                    return HIWORD(wParam) == XBUTTON1 ? MouseButton::X1 : MouseButton::X2;

                default:
                    return MouseButton::Unknown;
            }
        }

        float32 GetSignedLowWord(LPARAM value)
        {
            return static_cast<float32>(static_cast<int16>(LOWORD(value)));
        }

        float32 GetSignedHighWord(LPARAM value)
        {
            return static_cast<float32>(static_cast<int16>(HIWORD(value)));
        }
    }

    Window *Window::Create(const WindowSpecification &spec)
    {
        return new WindowsWindow(spec);
    }

    WindowsWindow::WindowsWindow(const WindowSpecification &spec)
        : m_Width(spec.Width), m_Height(spec.Height), m_VSync(spec.VSync)
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
        auto* wideTitle = new wchar_t[wideLength];
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
            const auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
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
            case WM_CREATE:
            {
                const WindowCreatedEvent event;
                DispatchEvent(event);
                return 0;
            }

            case WM_CLOSE:
            {
                const WindowCloseEvent event;
                DispatchEvent(event);
                return 0;
            }

            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }

            case WM_SETFOCUS:
            {
                const WindowFocusEvent event;
                DispatchEvent(event);
                return 0;
            }

            case WM_KILLFOCUS:
            {
                const WindowLostFocusEvent event;
                DispatchEvent(event);
                return 0;
            }

            case WM_SIZE:
            {
                m_Width = LOWORD(lParam);
                m_Height = HIWORD(lParam);

                switch (wParam)
                {
                    case SIZE_MINIMIZED:
                    {
                        const WindowMinimizedEvent event;
                        DispatchEvent(event);
                        break;
                    }

                    case SIZE_MAXIMIZED:
                    {
                        const WindowMaximizedEvent event;
                        DispatchEvent(event);
                        break;
                    }

                    case SIZE_RESTORED:
                    {
                        const WindowRestoredEvent event;
                        DispatchEvent(event);
                        break;
                    }
                }

                const WindowResizeEvent event(m_Width, m_Height);
                DispatchEvent(event);
                return 0;
            }

            case WM_MOVE:
            {
                const WindowMoveEvent event(LOWORD(lParam), HIWORD(lParam));
                DispatchEvent(event);
                return 0;
            }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                const KeyCode key = TranslateExtendedWindowsKey(wParam, lParam);
                const bool isRepeat = (lParam & (1 << 30)) != 0;
                const KeyPressedEvent event(key, isRepeat);
                DispatchEvent(event);
                return 0;
            }

            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                const KeyCode key = TranslateExtendedWindowsKey(wParam, lParam);
                const KeyReleasedEvent event(key);
                DispatchEvent(event);
                return 0;
            }

            case WM_CHAR:
            {
                const TextInputEvent event(static_cast<uint32>(wParam));
                DispatchEvent(event);
                return 0;
            }

            case WM_MOUSEMOVE:
            {
                const float32 x = GetSignedLowWord(lParam);
                const float32 y = GetSignedHighWord(lParam);
                const float32 deltaX = m_HasLastMousePosition ? x - m_LastMouseX : 0.0f;
                const float32 deltaY = m_HasLastMousePosition ? y - m_LastMouseY : 0.0f;

                m_LastMouseX = x;
                m_LastMouseY = y;
                m_HasLastMousePosition = true;

                const MouseMovedEvent event(x, y, deltaX, deltaY);
                DispatchEvent(event);
                return 0;
            }

            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
            {
                SetCapture(hWnd);

                const MouseButtonPressedEvent event(TranslateWindowsMouseButton(message, wParam));
                DispatchEvent(event);
                return 0;
            }

            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
            {
                if ((wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
                    ReleaseCapture();

                const MouseButtonReleasedEvent event(TranslateWindowsMouseButton(message, wParam));
                DispatchEvent(event);
                return 0;
            }

            case WM_MOUSEWHEEL:
            {
                const float32 yOffset = static_cast<float32>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float32>(WHEEL_DELTA);
                const MouseScrolledEvent event(0.0f, yOffset);
                DispatchEvent(event);
                return 0;
            }

            case WM_MOUSEHWHEEL:
            {
                const float32 xOffset = static_cast<float32>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float32>(WHEEL_DELTA);
                const MouseScrolledEvent event(xOffset, 0.0f);
                DispatchEvent(event);
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

