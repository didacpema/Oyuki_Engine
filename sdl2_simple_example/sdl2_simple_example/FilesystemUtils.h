#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class FileSystemUtils {
public:
    static void GenerateRequiredDirectories() {
        const std::vector<std::string> dirs = { "Assets", "Library/Meshes", "Library/Materials", "Library/Models" };
        for (const auto& dir : dirs) {
            if (!fs::exists(dir)) {
                fs::create_directories(dir);
            }
        }
    }

    static std::string getFileName(const std::string& filePath) {
        return fs::path(filePath).filename().string();
    }
};
