// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Input/InputTypes.h"

#include <cmath>

namespace Loom
{
    namespace
    {
        constexpr uint32 FNV1aOffset = 2166136261u;
        constexpr uint32 FNV1aPrime = 16777619u;

        uint32 HashName(const std::string& name)
        {
            uint32 hash = FNV1aOffset;
            for (const char character : name)
            {
                hash ^= static_cast<uint8>(character);
                hash *= FNV1aPrime;
            }

            return hash == 0 ? 1 : hash;
        }
    }

    InputActionID MakeInputActionID(const std::string& name)
    {
        return name.empty() ? InvalidInputActionID : HashName(name);
    }

    InputContextID MakeInputContextID(const std::string& name)
    {
        return name.empty() ? InvalidInputContextID : HashName(name);
    }

    InputBinding InputBinding::FromKey(KeyCode key, float32 scale, InputValueAxis axis)
    {
        InputBinding binding;
        binding.Type = InputBindingType::Key;
        binding.Device = InputDeviceType::Keyboard;
        binding.Axis = axis;
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
        binding.Axis = InputValueAxis::X;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseY(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseY;
        binding.Device = InputDeviceType::Mouse;
        binding.Axis = InputValueAxis::Y;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseWheelX(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseWheelX;
        binding.Device = InputDeviceType::Mouse;
        binding.Axis = InputValueAxis::X;
        binding.Scale = scale;
        return binding;
    }

    InputBinding InputBinding::FromMouseWheelY(float32 scale)
    {
        InputBinding binding;
        binding.Type = InputBindingType::MouseWheelY;
        binding.Device = InputDeviceType::Mouse;
        binding.Axis = InputValueAxis::Y;
        binding.Scale = scale;
        return binding;
    }

    bool InputActionValue::GetBool() const
    {
        return std::fabs(X) > 0.0001f || std::fabs(Y) > 0.0001f;
    }

    float32 InputActionValue::GetFloat() const
    {
        return X;
    }

    Vector2 InputActionValue::GetVector2() const
    {
        return {X, Y};
    }

    bool InputActionEvent::GetBool() const
    {
        return Value.GetBool();
    }

    float32 InputActionEvent::GetFloat() const
    {
        return Value.GetFloat();
    }

    Vector2 InputActionEvent::GetVector2() const
    {
        return Value.GetVector2();
    }
}
