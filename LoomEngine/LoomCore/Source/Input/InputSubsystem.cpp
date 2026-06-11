// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Input/InputSubsystem.h"

#include "InputRuntime.h"

#include <algorithm>
#include <utility>

namespace Loom
{
    InputSubsystem::~InputSubsystem()
    {
        ClearBindings();
        ClearContexts();
    }

    InputSubsystem::InputSubsystem(InputSubsystem&& other) noexcept
        : m_Contexts(std::move(other.m_Contexts)), m_Bindings(std::move(other.m_Bindings))
    {
        other.m_Contexts.clear();
        other.m_Bindings.clear();
    }

    InputSubsystem& InputSubsystem::operator=(InputSubsystem&& other) noexcept
    {
        if (this == &other)
            return *this;

        ClearBindings();
        ClearContexts();

        m_Contexts = std::move(other.m_Contexts);
        m_Bindings = std::move(other.m_Bindings);

        other.m_Contexts.clear();
        other.m_Bindings.clear();
        return *this;
    }

    void InputSubsystem::AddContext(const InputContext& context, uint32 priority)
    {
        if (!context.IsValid())
            return;

        if (std::find(m_Contexts.begin(), m_Contexts.end(), context.GetName()) != m_Contexts.end())
            return;

        InputRuntime::PushContext(context, priority);
        m_Contexts.push_back(context.GetName());
    }

    void InputSubsystem::RemoveContext(const InputContext& context)
    {
        if (!context.IsValid())
            return;

        InputRuntime::RemoveContext(context.GetName());
        m_Contexts.erase(std::remove(m_Contexts.begin(), m_Contexts.end(), context.GetName()), m_Contexts.end());
    }

    void InputSubsystem::ClearContexts()
    {
        for (const std::string& context : m_Contexts)
            InputRuntime::RemoveContext(context);

        m_Contexts.clear();
    }

    InputBindingHandle InputSubsystem::BindAction(const InputAction& action, InputTriggerEvent trigger, InputActionCallback callback)
    {
        const InputBindingHandle handle = InputRuntime::BindAction(action, trigger, std::move(callback));
        if (handle != InvalidInputBindingHandle)
            m_Bindings.push_back(handle);

        return handle;
    }

    void InputSubsystem::UnbindAction(InputBindingHandle handle)
    {
        InputRuntime::UnbindAction(handle);
        m_Bindings.erase(std::remove(m_Bindings.begin(), m_Bindings.end(), handle), m_Bindings.end());
    }

    void InputSubsystem::ClearBindings()
    {
        for (const InputBindingHandle handle : m_Bindings)
            InputRuntime::UnbindAction(handle);

        m_Bindings.clear();
    }
}
