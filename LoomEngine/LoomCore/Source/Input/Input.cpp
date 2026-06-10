// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Input/Input.h"

#include "Events/EventDispatcher.h"
#include "Events/Events/KeyEvents.h"
#include "Events/Events/MouseEvents.h"
#include "Events/Events/WindowEvents.h"

#include <array>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Loom
{
    namespace
    {
        constexpr size_t KeyCount = static_cast<size_t>(KeyCode::Count);
        constexpr size_t MouseButtonCount = static_cast<size_t>(MouseButton::Count);

        struct InputState
        {
            bool Initialized = false;
            OwnerID Owner = InvalidOwnerID;

            std::array<bool, KeyCount> CurrentKeys{};
            std::array<bool, KeyCount> PreviousKeys{};

            std::array<bool, MouseButtonCount> CurrentMouseButtons{};
            std::array<bool, MouseButtonCount> PreviousMouseButtons{};

            float32 MouseX = 0.0f;
            float32 MouseY = 0.0f;
            float32 MouseDeltaX = 0.0f;
            float32 MouseDeltaY = 0.0f;
            float32 MouseScrollX = 0.0f;
            float32 MouseScrollY = 0.0f;
            InputDeviceType LastDevice = InputDeviceType::Unknown;
        };

        struct InputContext
        {
            bool Enabled = true;
            std::unordered_map<std::string, std::vector<InputBinding>> Actions;
            std::unordered_map<std::string, std::vector<InputBinding>> Axes;
        };

        InputState s_Input;
        std::unordered_map<std::string, InputContext> s_Contexts;
        std::vector<std::string> s_ContextStack;

        size_t ToIndex(KeyCode key)
        {
            return static_cast<size_t>(key);
        }

        size_t ToIndex(MouseButton button)
        {
            return static_cast<size_t>(button);
        }

        bool IsValid(KeyCode key)
        {
            const size_t index = ToIndex(key);
            return key != KeyCode::Unknown && index < KeyCount;
        }

        bool IsValid(MouseButton button)
        {
            const size_t index = ToIndex(button);
            return button != MouseButton::Unknown && index < MouseButtonCount;
        }

        InputContext& GetOrCreateContext(const std::string& context)
        {
            return s_Contexts[context];
        }

        bool IsBindingDown(const InputBinding& binding)
        {
            switch (binding.Type)
            {
                case InputBindingType::Key:
                    return Input::IsKeyDown(binding.Key);

                case InputBindingType::MouseButton:
                    return Input::IsMouseButtonDown(binding.Button);

                case InputBindingType::MouseX:
                    return Input::GetMouseDeltaX() != 0.0f;

                case InputBindingType::MouseY:
                    return Input::GetMouseDeltaY() != 0.0f;

                case InputBindingType::MouseWheelX:
                    return Input::GetMouseScrollX() != 0.0f;

                case InputBindingType::MouseWheelY:
                    return Input::GetMouseScrollY() != 0.0f;
            }

            return false;
        }

        bool WasBindingPressed(const InputBinding& binding)
        {
            switch (binding.Type)
            {
                case InputBindingType::Key:
                    return Input::WasKeyPressed(binding.Key);

                case InputBindingType::MouseButton:
                    return Input::WasMouseButtonPressed(binding.Button);

                case InputBindingType::MouseX:
                case InputBindingType::MouseY:
                case InputBindingType::MouseWheelX:
                case InputBindingType::MouseWheelY:
                    return IsBindingDown(binding);
            }

            return false;
        }

        bool WasBindingReleased(const InputBinding& binding)
        {
            switch (binding.Type)
            {
                case InputBindingType::Key:
                    return Input::WasKeyReleased(binding.Key);

                case InputBindingType::MouseButton:
                    return Input::WasMouseButtonReleased(binding.Button);

                case InputBindingType::MouseX:
                case InputBindingType::MouseY:
                case InputBindingType::MouseWheelX:
                case InputBindingType::MouseWheelY:
                    return false;
            }

            return false;
        }

        float32 GetBindingValue(const InputBinding& binding)
        {
            switch (binding.Type)
            {
                case InputBindingType::Key:
                    return Input::IsKeyDown(binding.Key) ? binding.Scale : 0.0f;

                case InputBindingType::MouseButton:
                    return Input::IsMouseButtonDown(binding.Button) ? binding.Scale : 0.0f;

                case InputBindingType::MouseX:
                    return Input::GetMouseDeltaX() * binding.Scale;

                case InputBindingType::MouseY:
                    return Input::GetMouseDeltaY() * binding.Scale;

                case InputBindingType::MouseWheelX:
                    return Input::GetMouseScrollX() * binding.Scale;

                case InputBindingType::MouseWheelY:
                    return Input::GetMouseScrollY() * binding.Scale;
            }

            return 0.0f;
        }

        template<typename CallbackT>
        bool ForEachActiveBinding(const std::string& name, bool axis, CallbackT callback)
        {
            for (auto it = s_ContextStack.rbegin(); it != s_ContextStack.rend(); ++it)
            {
                const auto contextIt = s_Contexts.find(*it);
                if (contextIt == s_Contexts.end() || !contextIt->second.Enabled)
                    continue;

                const auto& map = axis ? contextIt->second.Axes : contextIt->second.Actions;
                const auto bindingIt = map.find(name);
                if (bindingIt == map.end())
                    continue;

                bool matched = false;
                for (const InputBinding& binding : bindingIt->second)
                {
                    if (callback(binding))
                        matched = true;
                }

                return matched;
            }

            return false;
        }

        const char* ToString(InputBindingType type)
        {
            switch (type)
            {
                case InputBindingType::Key: return "Key";
                case InputBindingType::MouseButton: return "MouseButton";
                case InputBindingType::MouseX: return "MouseX";
                case InputBindingType::MouseY: return "MouseY";
                case InputBindingType::MouseWheelX: return "MouseWheelX";
                case InputBindingType::MouseWheelY: return "MouseWheelY";
            }

            return "Key";
        }

        bool TryParseBindingType(const std::string& value, InputBindingType& outType)
        {
            if (value == "Key") outType = InputBindingType::Key;
            else if (value == "MouseButton") outType = InputBindingType::MouseButton;
            else if (value == "MouseX") outType = InputBindingType::MouseX;
            else if (value == "MouseY") outType = InputBindingType::MouseY;
            else if (value == "MouseWheelX") outType = InputBindingType::MouseWheelX;
            else if (value == "MouseWheelY") outType = InputBindingType::MouseWheelY;
            else return false;

            return true;
        }
    }

    InputBinding InputBinding::FromKey(KeyCode key, float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::Key;
        binding.Device = InputDeviceType::Keyboard;
        binding.Key = key;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseButton(MouseButton button, float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseButton;
        binding.Device = InputDeviceType::Mouse;
        binding.Button = button;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseX(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseX;
        binding.Device = InputDeviceType::Mouse;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseY(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseY;
        binding.Device = InputDeviceType::Mouse;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseWheelX(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseWheelX;
        binding.Device = InputDeviceType::Mouse;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseWheelY(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseWheelY;
        binding.Device = InputDeviceType::Mouse;
        binding.Scale = scale;
        return binding;
    }

    void Input::Initialize()
    {
        if (s_Input.Initialized)
            return;

        s_Input.Owner = GenerateOwnerID();
        s_Input.Initialized = true;
        PushContext("Default");

        EventDispatcher::Subscribe<KeyPressedEvent>([](const KeyPressedEvent& event)
        {
            s_Input.LastDevice = InputDeviceType::Keyboard;
            if (IsValid(event.Key))
                s_Input.CurrentKeys[ToIndex(event.Key)] = true;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<KeyReleasedEvent>([](const KeyReleasedEvent& event)
        {
            s_Input.LastDevice = InputDeviceType::Keyboard;
            if (IsValid(event.Key))
                s_Input.CurrentKeys[ToIndex(event.Key)] = false;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseMovedEvent>([](const MouseMovedEvent& event)
        {
            s_Input.LastDevice = InputDeviceType::Mouse;
            s_Input.MouseX = event.X;
            s_Input.MouseY = event.Y;
            s_Input.MouseDeltaX += event.DeltaX;
            s_Input.MouseDeltaY += event.DeltaY;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseButtonPressedEvent>([](const MouseButtonPressedEvent& event)
        {
            s_Input.LastDevice = InputDeviceType::Mouse;
            if (IsValid(event.Button))
                s_Input.CurrentMouseButtons[ToIndex(event.Button)] = true;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseButtonReleasedEvent>([](const MouseButtonReleasedEvent& event)
        {
            s_Input.LastDevice = InputDeviceType::Mouse;
            if (IsValid(event.Button))
                s_Input.CurrentMouseButtons[ToIndex(event.Button)] = false;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<MouseScrolledEvent>([](const MouseScrolledEvent& event)
        {
            s_Input.LastDevice = InputDeviceType::Mouse;
            s_Input.MouseScrollX += event.XOffset;
            s_Input.MouseScrollY += event.YOffset;
        }, s_Input.Owner);

        EventDispatcher::Subscribe<WindowLostFocusEvent>([](const WindowLostFocusEvent&)
        {
            s_Input.CurrentKeys.fill(false);
            s_Input.CurrentMouseButtons.fill(false);
        }, s_Input.Owner);
    }

    void Input::Shutdown()
    {
        if (!s_Input.Initialized)
            return;

        EventDispatcher::UnsubscribeAllForOwner(s_Input.Owner);
        s_Input = InputState{};
        s_Contexts.clear();
        s_ContextStack.clear();
    }

    void Input::BeginFrame()
    {
        s_Input.PreviousKeys = s_Input.CurrentKeys;
        s_Input.PreviousMouseButtons = s_Input.CurrentMouseButtons;
        s_Input.MouseDeltaX = 0.0f;
        s_Input.MouseDeltaY = 0.0f;
        s_Input.MouseScrollX = 0.0f;
        s_Input.MouseScrollY = 0.0f;
    }

    void Input::EndFrame()
    {
    }

    bool Input::IsKeyDown(KeyCode key)
    {
        return IsValid(key) && s_Input.CurrentKeys[ToIndex(key)];
    }

    bool Input::WasKeyPressed(KeyCode key)
    {
        return IsValid(key) && s_Input.CurrentKeys[ToIndex(key)] && !s_Input.PreviousKeys[ToIndex(key)];
    }

    bool Input::WasKeyReleased(KeyCode key)
    {
        return IsValid(key) && !s_Input.CurrentKeys[ToIndex(key)] && s_Input.PreviousKeys[ToIndex(key)];
    }

    bool Input::IsMouseButtonDown(MouseButton button)
    {
        return IsValid(button) && s_Input.CurrentMouseButtons[ToIndex(button)];
    }

    bool Input::WasMouseButtonPressed(MouseButton button)
    {
        return IsValid(button) && s_Input.CurrentMouseButtons[ToIndex(button)] && !s_Input.PreviousMouseButtons[ToIndex(button)];
    }

    bool Input::WasMouseButtonReleased(MouseButton button)
    {
        return IsValid(button) && !s_Input.CurrentMouseButtons[ToIndex(button)] && s_Input.PreviousMouseButtons[ToIndex(button)];
    }

    float32 Input::GetMouseX()
    {
        return s_Input.MouseX;
    }

    float32 Input::GetMouseY()
    {
        return s_Input.MouseY;
    }

    float32 Input::GetMouseDeltaX()
    {
        return s_Input.MouseDeltaX;
    }

    float32 Input::GetMouseDeltaY()
    {
        return s_Input.MouseDeltaY;
    }

    float32 Input::GetMouseScrollX()
    {
        return s_Input.MouseScrollX;
    }

    float32 Input::GetMouseScrollY()
    {
        return s_Input.MouseScrollY;
    }

    InputDeviceType Input::GetLastInputDevice()
    {
        return s_Input.LastDevice;
    }

    void Input::CreateContext(const std::string& context)
    {
        GetOrCreateContext(context);
    }

    void Input::ClearContext(const std::string& context)
    {
        s_Contexts.erase(context);
        for (auto it = s_ContextStack.begin(); it != s_ContextStack.end();)
        {
            if (*it == context)
                it = s_ContextStack.erase(it);
            else
                ++it;
        }
    }

    void Input::PushContext(const std::string& context)
    {
        GetOrCreateContext(context);
        s_ContextStack.push_back(context);
    }

    void Input::PopContext()
    {
        if (!s_ContextStack.empty())
            s_ContextStack.pop_back();
    }

    void Input::SetContextEnabled(const std::string& context, bool enabled)
    {
        GetOrCreateContext(context).Enabled = enabled;
    }

    bool Input::IsContextEnabled(const std::string& context)
    {
        const auto it = s_Contexts.find(context);
        return it != s_Contexts.end() && it->second.Enabled;
    }

    void Input::BindAction(const std::string& context, const std::string& action, const InputBinding& binding)
    {
        GetOrCreateContext(context).Actions[action].push_back(binding);
    }

    void Input::BindAction(const std::string& context, const std::string& action, KeyCode key)
    {
        BindAction(context, action, InputBinding::FromKey(key));
    }

    void Input::BindAction(const std::string& context, const std::string& action, MouseButton button)
    {
        BindAction(context, action, InputBinding::FromMouseButton(button));
    }

    void Input::BindAxis(const std::string& context, const std::string& axis, const InputBinding& binding, float32 scale)
    {
        InputBinding scaledBinding = binding;
        scaledBinding.Scale *= scale;
        GetOrCreateContext(context).Axes[axis].push_back(scaledBinding);
    }

    void Input::BindAxis(const std::string& context, const std::string& axis, KeyCode key, float32 scale)
    {
        BindAxis(context, axis, InputBinding::FromKey(key, scale));
    }

    void Input::ClearAction(const std::string& context, const std::string& action)
    {
        const auto it = s_Contexts.find(context);
        if (it != s_Contexts.end())
            it->second.Actions.erase(action);
    }

    void Input::ClearAxis(const std::string& context, const std::string& axis)
    {
        const auto it = s_Contexts.find(context);
        if (it != s_Contexts.end())
            it->second.Axes.erase(axis);
    }

    bool Input::IsActionDown(const std::string& action)
    {
        return ForEachActiveBinding(action, false, [](const InputBinding& binding)
        {
            return IsBindingDown(binding);
        });
    }

    bool Input::WasActionPressed(const std::string& action)
    {
        return ForEachActiveBinding(action, false, [](const InputBinding& binding)
        {
            return WasBindingPressed(binding);
        });
    }

    bool Input::WasActionReleased(const std::string& action)
    {
        return ForEachActiveBinding(action, false, [](const InputBinding& binding)
        {
            return WasBindingReleased(binding);
        });
    }

    float32 Input::GetAxis(const std::string& axis)
    {
        float32 value = 0.0f;
        ForEachActiveBinding(axis, true, [&value](const InputBinding& binding)
        {
            value += GetBindingValue(binding);
            return false;
        });

        return value;
    }
}
