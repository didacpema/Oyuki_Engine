#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

class DropHandler {
public:
    static void HandleDroppedFile(const std::string& sourcePath, const std::string& targetDirectory);
    static std::string GetSafeFileName(const std::string& originalName, const std::string& targetDirectory);
    static bool CopyFile(const std::string& sourcePath, const std::string& targetPath);

private:
    static std::string IncrementFileName(const std::string& baseName, const std::string& extension, int counter);
};