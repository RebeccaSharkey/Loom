// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Input/InputContext.h"

#include <utility>

namespace Loom
{
    InputContext::InputContext(std::string name)
        : m_ID(MakeInputContextID(name)), m_Name(std::move(name))
    {
    }

    InputContextID InputContext::GetID() const
    {
        return m_ID;
    }

    const std::string& InputContext::GetName() const
    {
        return m_Name;
    }

    const std::vector<InputAction>& InputContext::GetActions() const
    {
        return m_Actions;
    }

    bool InputContext::IsEnabled() const
    {
        return m_Enabled;
    }

    bool InputContext::IsValid() const
    {
        return m_ID != InvalidInputContextID;
    }

    InputContext& InputContext::AddAction(const InputAction& action)
    {
        if (IsValid() && action.IsValid())
            m_Actions.push_back(action);

        return *this;
    }

    InputContext& InputContext::SetEnabled(bool enabled)
    {
        m_Enabled = enabled;
        return *this;
    }

    void InputContext::ClearActions()
    {
        m_Actions.clear();
    }
}
