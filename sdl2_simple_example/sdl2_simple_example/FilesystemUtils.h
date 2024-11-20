#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class FileSystemUtils {
public:
    //crear directorios necesarios
    static void GenerateRequiredDirectories() {
        const std::vector<std::string> dirs = { "Assets", "Library/Meshes", "Library/Materials", "Library/Models", "Library/Textures"};
        for (const auto& dir : dirs) {
            if (!fs::exists(dir)) {
                fs::create_directories(dir);
            }
        }
    }

    //Obtener nombre del archivo a partir de su ruta
    static std::string getFileName(const std::string& filePath) {
        return fs::path(filePath).filename().string();
    }

    // Listar todos los archivos y carpetas dentro de un directorio
    static std::vector<std::string> ListDirectory(const std::string& directoryPath) {
        std::vector<std::string> entries;
        if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
            for (const auto& entry : fs::directory_iterator(directoryPath)) {
                entries.push_back(entry.path().string());
            }
        }
        return entries;
    }

    // Verificar si un archivo existe
    static bool FileExists(const std::string& filePath) {
        return fs::exists(filePath);
    }
};
