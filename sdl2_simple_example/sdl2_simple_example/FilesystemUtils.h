#pragma once

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

class FileSystemUtils {
public:
    // Crear directorios necesarios
    static void GenerateRequiredDirectories() {
        const std::vector<std::string> dirs = {
            "Assets",
            "Library/Meshes",
            "Library/Materials",
            "Library/Models",
            "Library/Textures"
        };

        for (const auto& dir : dirs) {
            try {
                if (!fs::exists(dir)) {
                    fs::create_directories(dir);
                }
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Error al crear directorio " << dir << ": " << e.what() << std::endl;
            }
        }
    }

    // Obtener nombre del archivo a partir de su ruta
    static std::string getFileName(const std::string& filePath) {
        try {
            return fs::path(filePath).filename().string();
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error al obtener el nombre del archivo: " << e.what() << std::endl;
            return "";
        }
    }

    // Listar todos los archivos y carpetas dentro de un directorio
    static std::vector<std::string> ListDirectory(const std::string& directoryPath) {
        std::vector<std::string> entries;
        try {
            if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
                for (const auto& entry : fs::directory_iterator(directoryPath)) {
                    entries.push_back(entry.path().generic_string());
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error al listar el directorio " << directoryPath << ": " << e.what() << std::endl;
        }
        return entries;
    }

    // Verificar si un archivo existe
    static bool FileExists(const std::string& filePath) {
        try {
            return fs::exists(filePath);
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error al verificar existencia de archivo: " << e.what() << std::endl;
            return false;
        }
    }
};
