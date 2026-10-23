#include "Core/Paths.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <stdexcept>

namespace Loom
{
    namespace
    {
        std::filesystem::path contentRoot;
        std::filesystem::path userRoot;
        void CreateDirectory(const std::filesystem::path& path)
        {
            std::error_code error;
            std::filesystem::create_directories(path, error);
            if (error || !std::filesystem::is_directory(path))
                throw std::runtime_error("Loom: cannot create writable directory: " + path.string() + ": " + error.message());
        }
    }
    void Paths::Initialize(const std::string& applicationName, const std::filesystem::path& developmentContent,
                           const std::filesystem::path& packagedContent)
    {
        const char* base = SDL_GetBasePath();
        if (!base) throw std::runtime_error(std::string("Loom: cannot resolve application base: ") + SDL_GetError());
        contentRoot = std::filesystem::weakly_canonical(developmentContent.empty()
            ? std::filesystem::path(base) / packagedContent : developmentContent);
        char* preferences = SDL_GetPrefPath("Loom", applicationName.c_str());
        if (!preferences) throw std::runtime_error(std::string("Loom: cannot resolve user data: ") + SDL_GetError());
        userRoot = std::filesystem::path(preferences);
        SDL_free(preferences);
        CreateDirectory(userRoot);
        CreateDirectory(Logs());
        CreateDirectory(Saves());
        CreateDirectory(Settings());
    }
    const std::filesystem::path& Paths::Content() { return contentRoot; }
    const std::filesystem::path& Paths::UserData() { return userRoot; }
    std::filesystem::path Paths::Logs() { return userRoot / "Logs"; }
    std::filesystem::path Paths::Saves() { return userRoot / "Saves"; }
    std::filesystem::path Paths::Settings() { return userRoot / "Settings"; }
    std::string Paths::ReadContentText(const std::filesystem::path& relativePath)
    {
        if (relativePath.is_absolute()) throw std::runtime_error("Loom: content path must be relative");
        for (const auto& part : relativePath)
            if (part == "..") throw std::runtime_error("Loom: content path cannot contain '..'");
        const auto path = Content() / relativePath;
        std::ifstream stream(path, std::ios::binary);
        if (!stream) throw std::runtime_error("Loom: cannot read content file: " + path.string());
        return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    }
}
