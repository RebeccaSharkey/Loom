// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Input/InputAction.h"

#include <utility>

namespace Loom
{
    InputAction::InputAction(std::string name, InputValueType valueType)
        : m_Name(std::move(name)), m_ValueType(valueType)
    {
    }

    InputAction::InputAction(std::string name, KeyCode key, InputValueType valueType)
        : InputAction(std::move(name), valueType)
    {
        BindKey(key);
    }

    InputAction::InputAction(std::string name, MouseButton button, InputValueType valueType)
        : InputAction(std::move(name), valueType)
    {
        BindMouseButton(button);
    }

    const std::string& InputAction::GetName() const
    {
        return m_Name;
    }

    InputValueType InputAction::GetValueType() const
    {
        return m_ValueType;
    }

    const std::vector<InputBinding>& InputAction::GetBindings() const
    {
        return m_Bindings;
    }

    bool InputAction::IsValid() const
    {
        return !m_Name.empty();
    }

    InputAction& InputAction::Bind(const InputBinding& binding)
    {
        if (IsValid())
            m_Bindings.push_back(binding);

        return *this;
    }

    InputAction& InputAction::BindKey(KeyCode key, float32 scale, InputValueAxis axis)
    {
        return Bind(InputBinding::FromKey(key, scale, axis));
    }

    InputAction& InputAction::BindMouseButton(MouseButton button, float32 scale)
    {
        return Bind(InputBinding::FromMouseButton(button, scale));
    }

    InputAction& InputAction::BindMouseX(float32 scale)
    {
        return Bind(InputBinding::FromMouseX(scale));
    }

    InputAction& InputAction::BindMouseY(float32 scale)
    {
        return Bind(InputBinding::FromMouseY(scale));
    }

    InputAction& InputAction::BindMouseWheelX(float32 scale)
    {
        return Bind(InputBinding::FromMouseWheelX(scale));
    }

    InputAction& InputAction::BindMouseWheelY(float32 scale)
    {
        return Bind(InputBinding::FromMouseWheelY(scale));
    }

    void InputAction::ClearBindings()
    {
        m_Bindings.clear();
    }
}
