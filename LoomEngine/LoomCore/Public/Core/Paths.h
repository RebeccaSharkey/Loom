#pragma once
#include <filesystem>
#include <string>

namespace Loom
{
    // Does not change the process working directory.
    class Paths
    {
    public:
        static void Initialize(const std::string& applicationName,
                               const std::filesystem::path& developmentContent = {},
                               const std::filesystem::path& packagedContent = "Content");
        static const std::filesystem::path& Content();
        static const std::filesystem::path& UserData();
        static std::filesystem::path Logs();
        static std::filesystem::path Saves();
        static std::filesystem::path Settings();
        static std::string ReadContentText(const std::filesystem::path& relativePath);
    };
}
