// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Input/InputContext.h"
#include <functional>
#include <string>
#include <vector>

namespace Loom
{
    using InputActionCallback = std::function<void(const InputActionEvent&)>;

    class LOOM_API InputSubsystem
    {
    public:
        InputSubsystem() = default;
        ~InputSubsystem();

        InputSubsystem(const InputSubsystem&) = delete;
        InputSubsystem& operator=(const InputSubsystem&) = delete;

        InputSubsystem(InputSubsystem&& other) noexcept;
        InputSubsystem& operator=(InputSubsystem&& other) noexcept;

        InputContextHandle AddContext(const InputContext& context, uint32 priority = 0);
        void RemoveContext(InputContextHandle handle);
        void ClearContexts();

        InputBindingHandle BindAction(const InputAction& action, InputTriggerEvent trigger, InputActionCallback callback);
        InputBindingHandle BindAction(const InputAction& action, InputTriggerEvent trigger, OwnerID owner, InputActionCallback callback);

        template<typename InstanceT>
        InputBindingHandle BindAction(const InputAction& action, InputTriggerEvent trigger, OwnerID owner, InstanceT* instance, void (InstanceT::*function)(const InputActionEvent&))
        {
            return BindAction(action, trigger, owner, [instance, function](const InputActionEvent& event)
            {
                (instance->*function)(event);
            });
        }

        void UnbindAction(InputBindingHandle handle);
        void UnbindAllForOwner(OwnerID owner);
        void ClearBindings();

    private:
        std::vector<InputContextHandle> m_Contexts;
        std::vector<InputBindingHandle> m_Bindings;
    };
}
