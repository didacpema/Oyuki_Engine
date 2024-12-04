#include "FileExplorer.h"

Explorer::Explorer(){}
Explorer::~Explorer(){}


void Explorer::UpdateDirectoryContents() {
    try {
        directoryContents.clear(); // Limpiar contenido previo

        if (fs::exists(currentDirectory) && fs::is_directory(currentDirectory)) {
            for (const auto& entry : fs::directory_iterator(currentDirectory)) {
                directoryContents.push_back(entry.path().string());
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing directory: " << e.what() << std::endl;
    }
}

void Explorer::draw() {
    ImGui::Begin("File Explorer");

    // Mostrar directorio actual
    ImGui::Text("Current Directory: %s", currentDirectory.c_str());

    // Botón para navegar al directorio padre
    if (ImGui::Button(".. (Go Up)")) {
        try {
            fs::path parent = fs::path(currentDirectory).parent_path();
            if (fs::exists(parent)) {
                currentDirectory = parent.string();
                UpdateDirectoryContents();
            }
            else {
                ImGui::Text("Error: Parent directory does not exist.");
            }
        }
        catch (const fs::filesystem_error& e) {
            ImGui::Text("Error accessing parent directory: %s", e.what());
        }
    }

    // Listar contenido del directorio
    for (const auto& entryPath : directoryContents) {
        try {
            fs::path entry(entryPath); // Convertir a fs::path

            // Si es un directorio, mostrarlo como botón
            if (fs::is_directory(entry)) {
                if (ImGui::Button(FileSystemUtils::getFileName(entry.string()).c_str())) {
                    currentDirectory = entry.string();
                    UpdateDirectoryContents();
                }
            }
            // Si es un archivo, permitir interacción
            else {
                ImGui::Text("%s", FileSystemUtils::getFileName(entry.string()).c_str());

                // Permitir arrastrar archivos hacia la escena
                if (ImGui::IsItemClicked()) {
                    handleFileDrop(entry.string().c_str());
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            ImGui::Text("Error: %s", e.what()); // Mostrar errores específicos por entrada
        }
    }

    ImGui::End();
}