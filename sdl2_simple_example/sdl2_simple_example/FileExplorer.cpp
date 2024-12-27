#include "FileExplorer.h"

Explorer::Explorer(){}
Explorer::~Explorer(){}


void Explorer::UpdateDirectoryContents() {
    try {
        directoryContents.clear();

        if (fs::exists(currentDirectory) && fs::is_directory(currentDirectory)) {
            for (const auto& entry : fs::directory_iterator(currentDirectory)) {
                directoryContents.push_back(entry.path().string());
            }
            // Asegurarnos de que el índice seleccionado sea válido
            if (selectedFileIndex >= directoryContents.size()) {
                selectedFileIndex = -1;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing directory: " << e.what() << std::endl;
        selectedFileIndex = -1;
    }
}
void Explorer::deleteSelectedFile() {
    if (selectedFileIndex < 0 || selectedFileIndex >= directoryContents.size()) {
        return;
    }

    try {
        std::string filePath = directoryContents[selectedFileIndex];
        if (!fs::is_directory(filePath) && fs::exists(filePath)) {
            if (fs::remove(filePath)) {
                selectedFileIndex = -1;
                UpdateDirectoryContents();
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error deleting file: " << e.what() << std::endl;
    }
}
void Explorer::draw() {
    ImGui::Begin("File Explorer");

    // Mostrar directorio actual
    ImGui::Text("Current Directory: %s", currentDirectory.c_str());

    // Botón para subir un nivel
    if (ImGui::Button(".. (Go Up)")) {
        try {
            fs::path parent = fs::path(currentDirectory).parent_path();
            if (fs::exists(parent)) {
                currentDirectory = parent.string();
                selectedFileIndex = -1;  // Reset selection when changing directory
                UpdateDirectoryContents();
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error accessing parent directory: " << e.what() << std::endl;
        }
    }

    // Lista de archivos y directorios
    ImGui::BeginChild("Files", ImVec2(0, -30), true);
    for (size_t i = 0; i < directoryContents.size(); ++i) {
        try {
            fs::path entry(directoryContents[i]);
            if (!fs::exists(entry)) continue;  // Skip if file no longer exists

            std::string displayName = FileSystemUtils::getFileName(entry.string());
            bool isSelected = (i == selectedFileIndex);

            if (fs::is_directory(entry)) {
                // Directorios como botones
                if (ImGui::Button(displayName.c_str())) {
                    currentDirectory = entry.string();
                    selectedFileIndex = -1;
                    UpdateDirectoryContents();
                }
            }
            else {
                // Archivos como seleccionables
                if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                    selectedFileIndex = i;
                }

                // Drag & Drop solo para archivos
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    const char* filePath = entry.string().c_str();
                    ImGui::SetDragDropPayload("FILE_PATH", filePath, strlen(filePath) + 1);
                    ImGui::Text("Dragging: %s", displayName.c_str());
                    ImGui::EndDragDropSource();
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error processing file: " << e.what() << std::endl;
        }
    }
    ImGui::EndChild();

    // Manejar borrado con tecla Delete
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) && selectedFileIndex >= 0) {
        deleteSelectedFile();
    }

    ImGui::End();
}

