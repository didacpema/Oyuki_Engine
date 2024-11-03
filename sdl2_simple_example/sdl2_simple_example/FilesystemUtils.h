#pragma once
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

void createRequiredDirectories() {
    const std::vector<std::string> dirs = { "Assets", "Library/Meshes", "Library/Materials", "Library/Models" };
    for (const auto& dir : dirs) {
        if (!fs::exists(dir)) fs::create_directories(dir);
    }
}