// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Types.h"
#include "Input/InputSubsystem.h"
#include <string>

namespace Loom
{
    class Application;

    class InputRuntime
    {
        friend class Application;
        friend class InputSubsystem;

    private:
        static void Initialize();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();

        static InputContextHandle PushContext(const InputContext& context, uint32 priority);
        static void RemoveContext(InputContextHandle handle);

        static InputBindingHandle BindAction(const InputAction& action, InputTriggerEvent trigger, OwnerID owner, InputActionCallback callback);
        static void UnbindAction(InputBindingHandle handle);
        static void UnbindAllForOwner(OwnerID owner);
    };
}
