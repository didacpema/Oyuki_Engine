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

    ImGui::Text("Current Directory: %s", currentDirectory.c_str());

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

    for (const auto& entryPath : directoryContents) {
        try {
            fs::path entry(entryPath);

            if (fs::is_directory(entry)) {
                if (ImGui::Button(FileSystemUtils::getFileName(entry.string()).c_str())) {
                    currentDirectory = entry.string();
                    UpdateDirectoryContents();
                }
            }
            else {
                ImGui::Text("%s", FileSystemUtils::getFileName(entry.string()).c_str());

                // Drag & Drop: iniciar un drag si el archivo es válido
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    ImGui::SetDragDropPayload("FILE_PATH", entry.string().c_str(), entry.string().size() + 1);
                    ImGui::Text("Dragging %s", FileSystemUtils::getFileName(entry.string()).c_str());
                    ImGui::EndDragDropSource();
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            ImGui::Text("Error: %s", e.what());
        }
    }

    ImGui::End();
}

