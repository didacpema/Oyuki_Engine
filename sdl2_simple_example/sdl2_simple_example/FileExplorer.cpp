#include "FileExplorer.h"
#include "DropHandler.h"
#include <GLFW/glfw3.h>
#include <filesystem>

Explorer::Explorer() {
    if (!fs::exists(currentDirectory)) {
        fs::create_directories(currentDirectory);
    }
}

Explorer::~Explorer() {
    // Destructor limpio, sin implementación incorrecta
}

void Explorer::handleFileDrop(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        try {
            DropHandler::HandleDroppedFile(path, currentDirectory);
        }
        catch (const std::exception& e) {
            std::cerr << "Error handling dropped file: " << e.what() << std::endl;
        }
    }
    UpdateDirectoryContents();
}

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
                selectedFileIndex = -1;
                UpdateDirectoryContents();
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error accessing parent directory: " << e.what() << std::endl;
        }
    }

    // Área de drop
    ImGui::BeginChild("Files", ImVec2(0, -30), true);

    // Manejar drag & drop
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EXTERNAL_FILE")) {
            std::vector<std::string> droppedPaths;
            const char* paths = static_cast<const char*>(payload->Data);
            droppedPaths.push_back(paths);
            handleFileDrop(droppedPaths);
        }
        ImGui::EndDragDropTarget();
    }

    // Lista de archivos y directorios
    for (size_t i = 0; i < directoryContents.size(); ++i) {
        try {
            fs::path entry(directoryContents[i]);
            if (!fs::exists(entry)) continue;

            std::string displayName = FileSystemUtils::getFileName(entry.string());
            bool isSelected = (i == selectedFileIndex);

            if (fs::is_directory(entry)) {
                if (ImGui::Button(displayName.c_str())) {
                    currentDirectory = entry.string();
                    selectedFileIndex = -1;
                    UpdateDirectoryContents();
                }
            }
            else {
                if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                    selectedFileIndex = i;
                }

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("FILE_PATH", entry.string().c_str(),
                        entry.string().length() + 1);
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