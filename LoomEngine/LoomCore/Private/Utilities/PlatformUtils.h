// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <memory>

struct SDL_Window;

namespace Loom
{
    struct PlatformContext
    {
        PlatformContext();
        ~PlatformContext();

        PlatformContext(const PlatformContext&) = delete;
        PlatformContext& operator=(const PlatformContext&) = delete;
    };

    struct PlatformDestroyer
    {
        void operator()(SDL_Window* window) const;
    };

    typedef std::unique_ptr<SDL_Window, PlatformDestroyer> SDLWindowPtr;
}
