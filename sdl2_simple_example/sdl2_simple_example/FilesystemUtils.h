#pragma once

#include <filesystem>
#include <string>
#include <vector>

using namespace std;    
namespace fs = filesystem;

class FileSystemUtils {
public:
    static void createRequiredDirectories() {
        const vector<string> dirs = { "Assets", "Library/Meshes", "Library/Materials", "Library/Models" };
        for (const auto& dir : dirs) {
            if (!fs::exists(dir)) {
                fs::create_directories(dir);
            }
        }
    }

    static string getFileName(const string& filePath) {
        return fs::path(filePath).filename().string();
    }
};