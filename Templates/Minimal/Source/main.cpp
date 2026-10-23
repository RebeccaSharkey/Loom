#include "EntryPoint.h"
#include "Core/Paths.h"
#include "LoomProjectConfig.h"

class GameApplication final : public Loom::Application
{
public:
    GameApplication() : Application(LoomProject::Specification()) {}

    void OnStart() override
    {
        LOOM_LOG_NOTICE("__LOOM_NAME__", "Application started.");
        const auto text = Loom::Paths::ReadContentText("welcome.txt");
        LOOM_LOG_NOTICE("Content", "%s", text.c_str());
    }

    void OnShutdown() override
    {
        LOOM_LOG_NOTICE("__LOOM_NAME__", "Application shutdown.");
    }
};

Loom::Application* Loom::CreateApplication() { return new GameApplication(); }
