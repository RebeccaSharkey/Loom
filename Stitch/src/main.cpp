// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Loom/Core/Application.h"
#include "Loom/Core/EntryPoint..h"

class Stitch : public Loom::Application
{
public:
    Stitch() {};
    ~Stitch() {};
};

Loom::Application* Loom::CreateApplication()
{
    return new Stitch();
}