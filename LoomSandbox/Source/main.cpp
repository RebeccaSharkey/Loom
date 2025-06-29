// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "EntryPoint.h"

class LoomSandbox : public Loom::Application
{
public:
    using Application::Application;
    LoomSandbox() {};
    ~LoomSandbox() override = default;
};

Loom::Application* Loom::CreateApplication()
{
    return new LoomSandbox();
}