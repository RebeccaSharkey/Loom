// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Events/Event.h"

namespace Loom
{


    bool Event::GetHandled() const
    {
        return m_Handled;
    }
    void Event::SetHandled(const bool handled)
    {
        m_Handled = handled;
    }
}

