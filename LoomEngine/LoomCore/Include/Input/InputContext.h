// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Input/InputAction.h"
#include <string>
#include <vector>

namespace Loom
{
    class LOOM_API InputContext
    {
    public:
        InputContext() = default;
        explicit InputContext(std::string name);

        InputContextID GetID() const;
        const std::string& GetName() const;
        const std::vector<InputAction>& GetActions() const;
        bool IsEnabled() const;
        bool IsValid() const;

        InputContext& AddAction(const InputAction& action);
        InputContext& SetEnabled(bool enabled);
        void ClearActions();

    private:
        InputContextID m_ID = InvalidInputContextID;
        std::string m_Name;
        bool m_Enabled = true;
        std::vector<InputAction> m_Actions;
    };

    using InputMappingContext = InputContext;
}
