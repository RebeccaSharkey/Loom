// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Application.h"

extern Loom::Application* Loom::CreateApplication();

int main(int argc, char* argv[])
{
    auto app = Loom::CreateApplication();

    app->Run();

    delete app;
}