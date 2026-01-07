// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Core/Assert.h"

namespace Loom
{
    AssertPolicy AssertConfig::RuntimePolicy = AssertPolicy::DebugBreak;

    void HandleAssertFailure(const char *expr, const char *file, int line, const char *func,
        const std::string &message)
    {
        switch (AssertConfig::RuntimePolicy)
        {
            case AssertPolicy::LogOnly:
                // Already logged, just continue
                break;
            case AssertPolicy::DebugBreak:
            case AssertPolicy::EditorPrompt:
                LOOM_DEBUG_BREAK();
                break;
            case AssertPolicy::Abort:
                std::abort();
                break;
            case AssertPolicy::Throw:
                throw std::runtime_error("Assert failed: " + message);
                break;
        }
    }
}
