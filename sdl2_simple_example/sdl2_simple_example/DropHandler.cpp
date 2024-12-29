#include "DropHandler.h"
#include <iostream>

void DropHandler::HandleDroppedFile(const std::string& sourcePath, const std::string& targetDirectory) {
    try {
        fs::path source(sourcePath);
        if (!fs::exists(source)) {
            std::cerr << "Source file does not exist: " << sourcePath << std::endl;
            return;
        }

        // Obtener un nombre seguro para el archivo
        std::string safeName = GetSafeFileName(source.filename().string(), targetDirectory);
        fs::path targetPath = fs::path(targetDirectory) / safeName;

        // Copiar el archivo
        if (CopyFile(sourcePath, targetPath.string())) {
            std::cout << "File copied successfully to: " << targetPath << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error handling dropped file: " << e.what() << std::endl;
    }
}

std::string DropHandler::GetSafeFileName(const std::string& originalName, const std::string& targetDirectory) {
    fs::path original(originalName);
    std::string baseName = original.stem().string();
    std::string extension = original.extension().string();

    fs::path targetPath = fs::path(targetDirectory) / originalName;
    int counter = 1;

    while (fs::exists(targetPath)) {
        std::string newName = IncrementFileName(baseName, extension, counter);
        targetPath = fs::path(targetDirectory) / newName;
        counter++;
    }

    return targetPath.filename().string();
}

std::string DropHandler::IncrementFileName(const std::string& baseName, const std::string& extension, int counter) {
    return baseName + "(" + std::to_string(counter) + ")" + extension;
}

bool DropHandler::CopyFile(const std::string& sourcePath, const std::string& targetPath) {
    try {
        fs::copy(sourcePath, targetPath, fs::copy_options::overwrite_existing);
        return true;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
        return false;
    }
}