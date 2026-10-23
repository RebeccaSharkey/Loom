#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <filesystem>
#include <string>

// Quote one argument for CreateProcess/CommandLineToArgvW; no shell is involved.
static std::wstring Quote(const std::wstring& value)
{
    std::wstring quoted = L"\"";
    size_t slashes = 0;
    for (wchar_t character : value)
    {
        if (character == L'\\') { ++slashes; continue; }
        quoted.append(character == L'\"' ? slashes * 2 + 1 : slashes, L'\\');
        quoted += character;
        slashes = 0;
    }
    quoted.append(slashes * 2, L'\\');
    return quoted + L"\"";
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    wchar_t executable[32768]{};
    if (!GetModuleFileNameW(nullptr, executable, 32768)) return 1;
    const auto base = std::filesystem::path(executable).parent_path();
    auto script = base / L"tools" / L"ProjectWorkflow" / L"New-LoomProjectGui.ps1";
    // Also support executables launched directly from a repository build tree.
    if (!std::filesystem::exists(script))
    {
        auto ancestor = base;
        for (int depth = 0; depth < 4; ++depth)
        {
            script = ancestor / L"Tools" / L"ProjectWorkflow" / L"New-LoomProjectGui.ps1";
            if (std::filesystem::exists(script)) break;
            ancestor = ancestor.parent_path();
        }
    }
    if (!std::filesystem::exists(script))
    {
        MessageBoxW(nullptr, L"The launcher files are missing. Reinstall the Loom SDK.", L"Loom Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }

    wchar_t windows[32768]{};
    if (!GetWindowsDirectoryW(windows, 32768)) return 1;
    const auto powershell = std::filesystem::path(windows) / L"System32/WindowsPowerShell/v1.0/powershell.exe";
    std::wstring command = Quote(powershell.wstring()) + L" -NoProfile -STA -ExecutionPolicy Bypass -File " + Quote(script.wstring());
    int count = 0;
    LPWSTR* arguments = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!arguments) return 1;
    for (int index = 1; index < count; ++index) command += L" " + Quote(arguments[index]);
    LocalFree(arguments);

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(powershell.c_str(), command.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, base.c_str(), &startup, &process))
    {
        MessageBoxW(nullptr, L"Windows could not start the Loom launcher.", L"Loom Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }
    CloseHandle(process.hThread);
    WaitForSingleObject(process.hProcess, INFINITE);
    DWORD result = 1;
    GetExitCodeProcess(process.hProcess, &result);
    CloseHandle(process.hProcess);
    return static_cast<int>(result);
}
