// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "Core/Application.h"
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string_view>

extern Loom::Application* Loom::CreateApplication();

int main(int argc, char* argv[])
{
    try
    {
        for (int i = 1; i < argc; ++i)
        {
            const std::string_view argument(argv[i]);
            if (argument == "--loom-smoke-test") Loom::Application::SmokeTest = true;
            if (argument == "--loom-packaged") Loom::Application::PackagedContent = true;
        }
        std::unique_ptr<Loom::Application> app(Loom::CreateApplication());
        if (!app) throw std::runtime_error("Loom: CreateApplication returned null");
        app->Run();
        return 0;
    }
    catch (const std::exception& error)
    {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
