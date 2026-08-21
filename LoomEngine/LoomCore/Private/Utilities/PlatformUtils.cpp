// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "PlatformUtils.h"

#include "Core/Assert.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

Loom::PlatformContext::PlatformContext()
{
    SDL_SetMainReady();
    LOOM_VERIFY(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS), SDL_GetError());
    LOOM_LOG_QUIET("Platform", "Platform Initialised" );
}

Loom::PlatformContext::~PlatformContext()
{
    SDL_Quit();
    LOOM_LOG_QUIET("Platform", "Platform shut down" );
}

void Loom::PlatformDestroyer::operator()(SDL_Window* window) const
{
    if (window)
    {
        SDL_DestroyWindow(window);
        LOOM_LOG_QUIET("SDL_Destroyer", "SDL Window Destroyed" );
    }
}