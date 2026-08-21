// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "InputRuntime.h"

#include "Events/EventDispatcher.h"
#include "Events/Events/KeyEvents.h"
#include "Events/Events/MouseEvents.h"
#include "Events/Events/WindowEvents.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Loom
{
    namespace
    {
        constexpr size_t KeyCount = static_cast<size_t>(KeyCode::Count);
        constexpr size_t MouseButtonCount = static_cast<size_t>(MouseButton::Count);
        constexpr float32 InputEpsilon = 0.0001f;

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

        struct ActiveInputContext
        {
            InputContextHandle Handle = InvalidInputContextHandle;
            InputContext Context;
            uint32 Priority = 0;
            uint64 Order = 0;
            bool PendingRemove = false;
        };

        struct RegisteredInputAction
        {
            InputBindingHandle Handle = InvalidInputBindingHandle;
            InputActionID ActionID = InvalidInputActionID;
            InputTriggerEvent Trigger = InputTriggerEvent::Triggered;
            OwnerID Owner = InvalidOwnerID;
            InputActionCallback Callback;
            bool PendingRemove = false;
        };

        struct EvaluatedInputAction
        {
            const InputAction* Action = nullptr;
            InputActionValue Current;
            InputActionValue Previous;
            bool Started = false;
            bool Triggered = false;
            bool Completed = false;
        };

        InputState s_Input;
        std::vector<ActiveInputContext> s_ActiveContexts;
        std::vector<ActiveInputContext> s_PendingContextAdds;
        std::vector<RegisteredInputAction> s_ActionCallbacks;
        std::vector<RegisteredInputAction> s_PendingActionCallbackAdds;
        std::unordered_map<InputActionID, EvaluatedInputAction> s_EvaluatedActions;
        bool s_IsDispatching = false;
        InputBindingHandle s_NextBindingHandle = 1;
        InputContextHandle s_NextContextHandle = 1;
        uint64 s_NextContextOrder = 1;

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

        bool IsValueActive(const InputActionValue& value)
        {
            return std::fabs(value.X) > InputEpsilon || std::fabs(value.Y) > InputEpsilon;
        }

        bool IsKeyDown(KeyCode key, bool previous)
        {
            if (!IsValid(key))
                return false;

            return previous ? s_Input.PreviousKeys[ToIndex(key)] : s_Input.CurrentKeys[ToIndex(key)];
        }

        bool IsMouseButtonDown(MouseButton button, bool previous)
        {
            if (!IsValid(button))
                return false;

            return previous ? s_Input.PreviousMouseButtons[ToIndex(button)] : s_Input.CurrentMouseButtons[ToIndex(button)];
        }

        float32 GetBindingScalar(const InputBinding& binding, bool previous)
        {
            switch (binding.Type)
            {
                case InputBindingType::Key:
                    return IsKeyDown(binding.Key, previous) ? binding.Scale : 0.0f;

                case InputBindingType::MouseButton:
                    return IsMouseButtonDown(binding.Button, previous) ? binding.Scale : 0.0f;

                case InputBindingType::MouseX:
                    return previous ? 0.0f : s_Input.MouseDeltaX * binding.Scale;

                case InputBindingType::MouseY:
                    return previous ? 0.0f : s_Input.MouseDeltaY * binding.Scale;

                case InputBindingType::MouseWheelX:
                    return previous ? 0.0f : s_Input.MouseScrollX * binding.Scale;

                case InputBindingType::MouseWheelY:
                    return previous ? 0.0f : s_Input.MouseScrollY * binding.Scale;
            }

            return 0.0f;
        }

        InputActionValue EvaluateActionValue(const InputAction& action, bool previous)
        {
            InputActionValue value;
            value.Type = action.GetValueType();

            for (const InputBinding& binding : action.GetBindings())
            {
                const float32 scalar = GetBindingScalar(binding, previous);

                switch (action.GetValueType())
                {
                    case InputValueType::Bool:
                        if (std::fabs(scalar) > InputEpsilon)
                            value.X = 1.0f;
                        break;

                    case InputValueType::Axis1D:
                        value.X += scalar;
                        break;

                    case InputValueType::Axis2D:
                        if (binding.Axis == InputValueAxis::X)
                            value.X += scalar;
                        else
                            value.Y += scalar;
                        break;
                }
            }

            return value;
        }

        void SortActiveContexts()
        {
            std::sort(s_ActiveContexts.begin(), s_ActiveContexts.end(), [](const ActiveInputContext& left, const ActiveInputContext& right)
            {
                if (left.Priority != right.Priority)
                    return left.Priority < right.Priority;

                return left.Order < right.Order;
            });
        }

        bool ShouldDispatch(InputTriggerEvent trigger, const EvaluatedInputAction& action)
        {
            switch (trigger)
            {
                case InputTriggerEvent::Started:
                    return action.Started;

                case InputTriggerEvent::Triggered:
                    return action.Triggered;

                case InputTriggerEvent::Completed:
                    return action.Completed;
            }

            return false;
        }

        void EvaluateActiveActions()
        {
            s_EvaluatedActions.clear();

            for (auto activeIt = s_ActiveContexts.rbegin(); activeIt != s_ActiveContexts.rend(); ++activeIt)
            {
                if (activeIt->PendingRemove || !activeIt->Context.IsEnabled())
                    continue;

                for (const InputAction& action : activeIt->Context.GetActions())
                {
                    if (!action.IsValid() || s_EvaluatedActions.find(action.GetID()) != s_EvaluatedActions.end())
                        continue;

                    EvaluatedInputAction evaluated;
                    evaluated.Action = &action;
                    evaluated.Current = EvaluateActionValue(action, false);
                    evaluated.Previous = EvaluateActionValue(action, true);

                    const bool active = IsValueActive(evaluated.Current);
                    const bool previousActive = IsValueActive(evaluated.Previous);

                    evaluated.Started = active && !previousActive;
                    evaluated.Triggered = active;
                    evaluated.Completed = !active && previousActive;

                    s_EvaluatedActions[action.GetID()] = evaluated;
                }
            }
        }

        void ApplyPendingInputMutations()
        {
            s_ActiveContexts.erase(std::remove_if(s_ActiveContexts.begin(), s_ActiveContexts.end(), [](const ActiveInputContext& context)
            {
                return context.PendingRemove;
            }), s_ActiveContexts.end());

            s_PendingContextAdds.erase(std::remove_if(s_PendingContextAdds.begin(), s_PendingContextAdds.end(), [](const ActiveInputContext& context)
            {
                return context.PendingRemove;
            }), s_PendingContextAdds.end());

            if (!s_PendingContextAdds.empty())
            {
                s_ActiveContexts.insert(s_ActiveContexts.end(), s_PendingContextAdds.begin(), s_PendingContextAdds.end());
                s_PendingContextAdds.clear();
                SortActiveContexts();
            }

            s_ActionCallbacks.erase(std::remove_if(s_ActionCallbacks.begin(), s_ActionCallbacks.end(), [](const RegisteredInputAction& binding)
            {
                return binding.PendingRemove;
            }), s_ActionCallbacks.end());

            if (!s_PendingActionCallbackAdds.empty())
            {
                s_ActionCallbacks.insert(s_ActionCallbacks.end(), s_PendingActionCallbackAdds.begin(), s_PendingActionCallbackAdds.end());
                s_PendingActionCallbackAdds.clear();
            }
        }

        void DispatchActionCallbacks()
        {
            EvaluateActiveActions();

            s_IsDispatching = true;
            for (const RegisteredInputAction& binding : s_ActionCallbacks)
            {
                if (binding.PendingRemove || !binding.Callback)
                    continue;

                const auto evaluatedIt = s_EvaluatedActions.find(binding.ActionID);
                if (evaluatedIt == s_EvaluatedActions.end() || !evaluatedIt->second.Action)
                    continue;

                const EvaluatedInputAction& evaluated = evaluatedIt->second;
                if (!ShouldDispatch(binding.Trigger, evaluated))
                    continue;

                InputActionEvent event;
                event.Action = evaluated.Action;
                event.Trigger = binding.Trigger;
                event.Value = evaluated.Current;
                event.Device = s_Input.LastDevice;
                binding.Callback(event);
            }

            s_IsDispatching = false;
            ApplyPendingInputMutations();
        }
    }

    void InputRuntime::Initialize()
    {
        if (s_Input.Initialized)
            return;

        s_Input.Owner = GenerateOwnerID();
        s_Input.Initialized = true;

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

    void InputRuntime::Shutdown()
    {
        if (!s_Input.Initialized)
            return;

        EventDispatcher::UnsubscribeAllForOwner(s_Input.Owner);
        s_Input = InputState{};
        s_ActiveContexts.clear();
        s_PendingContextAdds.clear();
        s_ActionCallbacks.clear();
        s_PendingActionCallbackAdds.clear();
        s_EvaluatedActions.clear();
        s_IsDispatching = false;
        s_NextBindingHandle = 1;
        s_NextContextHandle = 1;
        s_NextContextOrder = 1;
    }

    void InputRuntime::BeginFrame()
    {
        s_Input.PreviousKeys = s_Input.CurrentKeys;
        s_Input.PreviousMouseButtons = s_Input.CurrentMouseButtons;
        s_Input.MouseDeltaX = 0.0f;
        s_Input.MouseDeltaY = 0.0f;
        s_Input.MouseScrollX = 0.0f;
        s_Input.MouseScrollY = 0.0f;
    }

    void InputRuntime::EndFrame()
    {
        DispatchActionCallbacks();
    }

    InputContextHandle InputRuntime::PushContext(const InputContext& context, uint32 priority)
    {
        if (!context.IsValid())
            return InvalidInputContextHandle;

        ActiveInputContext activeContext;
        activeContext.Handle = s_NextContextHandle++;
        activeContext.Context = context;
        activeContext.Priority = priority;
        activeContext.Order = s_NextContextOrder++;

        if (s_IsDispatching)
            s_PendingContextAdds.push_back(activeContext);
        else
        {
            s_ActiveContexts.push_back(activeContext);
            SortActiveContexts();
        }

        return activeContext.Handle;
    }

    void InputRuntime::RemoveContext(InputContextHandle handle)
    {
        if (handle == InvalidInputContextHandle)
            return;

        auto markPendingRemove = [handle](ActiveInputContext& activeContext)
        {
            if (activeContext.Handle == handle)
                activeContext.PendingRemove = true;
        };

        for (ActiveInputContext& activeContext : s_ActiveContexts)
            markPendingRemove(activeContext);

        for (ActiveInputContext& activeContext : s_PendingContextAdds)
            markPendingRemove(activeContext);

        if (s_IsDispatching)
            return;

        s_ActiveContexts.erase(std::remove_if(s_ActiveContexts.begin(), s_ActiveContexts.end(), [](const ActiveInputContext& activeContext)
        {
            return activeContext.PendingRemove;
        }), s_ActiveContexts.end());

        s_PendingContextAdds.erase(std::remove_if(s_PendingContextAdds.begin(), s_PendingContextAdds.end(), [](const ActiveInputContext& activeContext)
        {
            return activeContext.PendingRemove;
        }), s_PendingContextAdds.end());
    }

    InputBindingHandle InputRuntime::BindAction(const InputAction& action, InputTriggerEvent trigger, OwnerID owner, InputActionCallback callback)
    {
        if (!action.IsValid() || !callback)
            return InvalidInputBindingHandle;

        RegisteredInputAction binding;
        binding.Handle = s_NextBindingHandle++;
        binding.ActionID = action.GetID();
        binding.Trigger = trigger;
        binding.Owner = owner;
        binding.Callback = std::move(callback);

        const InputBindingHandle handle = binding.Handle;
        if (s_IsDispatching)
            s_PendingActionCallbackAdds.push_back(std::move(binding));
        else
            s_ActionCallbacks.push_back(std::move(binding));

        return handle;
    }

    void InputRuntime::UnbindAction(InputBindingHandle handle)
    {
        if (handle == InvalidInputBindingHandle)
            return;

        for (RegisteredInputAction& binding : s_ActionCallbacks)
        {
            if (binding.Handle == handle)
                binding.PendingRemove = true;
        }

        s_PendingActionCallbackAdds.erase(std::remove_if(s_PendingActionCallbackAdds.begin(), s_PendingActionCallbackAdds.end(), [handle](const RegisteredInputAction& binding)
        {
            return binding.Handle == handle;
        }), s_PendingActionCallbackAdds.end());

        if (s_IsDispatching)
            return;

        s_ActionCallbacks.erase(std::remove_if(s_ActionCallbacks.begin(), s_ActionCallbacks.end(), [](const RegisteredInputAction& binding)
        {
            return binding.PendingRemove;
        }), s_ActionCallbacks.end());
    }

    void InputRuntime::UnbindAllForOwner(OwnerID owner)
    {
        if (owner == InvalidOwnerID)
            return;

        for (RegisteredInputAction& binding : s_ActionCallbacks)
        {
            if (binding.Owner == owner)
                binding.PendingRemove = true;
        }

        s_PendingActionCallbackAdds.erase(std::remove_if(s_PendingActionCallbackAdds.begin(), s_PendingActionCallbackAdds.end(), [owner](const RegisteredInputAction& binding)
        {
            return binding.Owner == owner;
        }), s_PendingActionCallbackAdds.end());

        if (s_IsDispatching)
            return;

        s_ActionCallbacks.erase(std::remove_if(s_ActionCallbacks.begin(), s_ActionCallbacks.end(), [](const RegisteredInputAction& binding)
        {
            return binding.PendingRemove;
        }), s_ActionCallbacks.end());
    }
}
