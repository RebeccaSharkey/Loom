// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Input/InputTypes.h"
#include <string>
#include <vector>

namespace Loom
{
    class LOOM_API InputAction
    {
    public:
        InputAction() = default;
        explicit InputAction(std::string name, InputValueType valueType = InputValueType::Bool);
        InputAction(std::string name, KeyCode key, InputValueType valueType = InputValueType::Bool);
        InputAction(std::string name, MouseButton button, InputValueType valueType = InputValueType::Bool);

        const std::string& GetName() const;
        InputValueType GetValueType() const;
        const std::vector<InputBinding>& GetBindings() const;
        bool IsValid() const;

        InputAction& Bind(const InputBinding& binding);
        InputAction& BindKey(KeyCode key, float32 scale = 1.0f, InputValueAxis axis = InputValueAxis::X);
        InputAction& BindMouseButton(MouseButton button, float32 scale = 1.0f);
        InputAction& BindMouseX(float32 scale = 1.0f);
        InputAction& BindMouseY(float32 scale = 1.0f);
        InputAction& BindMouseWheelX(float32 scale = 1.0f);
        InputAction& BindMouseWheelY(float32 scale = 1.0f);
        void ClearBindings();

    private:
        std::string m_Name;
        InputValueType m_ValueType = InputValueType::Bool;
        std::vector<InputBinding> m_Bindings;
    };
}
