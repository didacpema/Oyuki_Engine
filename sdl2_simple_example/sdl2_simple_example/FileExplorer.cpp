#include "FileExplorer.h"

Explorer::Explorer(){}
Explorer::~Explorer(){}


void Explorer::UpdateDirectoryContents() {
    try {
        directoryContents.clear();

        if (!fs::exists(currentDirectory)) {
            std::cerr << "Directory does not exist: " << currentDirectory << std::endl;
            return;
        }

        if (!fs::is_directory(currentDirectory)) {
            std::cerr << "Path is not a directory: " << currentDirectory << std::endl;
            return;
        }

        for (const auto& entry : fs::directory_iterator(currentDirectory)) {
            directoryContents.push_back(entry.path().string());
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing directory: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error in UpdateDirectoryContents: " << e.what() << std::endl;
    }
}
bool Explorer::tryDeleteFile(const std::string& filePath) {
    try {
        if (filePath.empty()) {
            return false;
        }

        fs::path path(filePath);
        if (!fs::exists(path)) {
            return false;
        }

        // Verificar permisos de escritura
        auto perms = fs::status(path).permissions();
        if ((perms & fs::perms::owner_write) == fs::perms::none) {
            std::cerr << "No write permissions for: " << filePath << std::endl;
            return false;
        }

        return fs::remove_all(path) > 0;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error while deleting: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error while deleting: " << e.what() << std::endl;
        return false;
    }
}
void Explorer::draw() {
    if (ImGui::Begin("File Explorer")) {
        ImGui::Text("Current Directory: %s", currentDirectory.c_str());

        if (ImGui::Button(".. (Go Up)")) {
            try {
                fs::path parent = fs::path(currentDirectory).parent_path();
                if (!parent.empty() && fs::exists(parent)) {
                    currentDirectory = parent.string();
                    UpdateDirectoryContents();
                }
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Error accessing parent directory: " << e.what() << std::endl;
            }
        }

        for (const auto& entryPath : directoryContents) {
            try {
                if (entryPath.empty()) continue;

                fs::path entry(entryPath);
                if (!fs::exists(entry)) continue;

                std::string fileName = FileSystemUtils::getFileName(entry.string());
                if (fileName.empty()) continue;

                bool isDir = fs::is_directory(entry);

                if (isDir) {
                    if (ImGui::Button(fileName.c_str())) {
                        currentDirectory = entry.string();
                        UpdateDirectoryContents();
                    }
                }
                else {
                    bool isSelected = (selectedFile == entry.string());
                    if (ImGui::Selectable(fileName.c_str(), isSelected)) {
                        selectedFile = entry.string();
                    }

                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                        ImGui::SetDragDropPayload("FILE_PATH", entry.string().c_str(), entry.string().size() + 1);
                        ImGui::Text("Dragging %s", fileName.c_str());
                        ImGui::EndDragDropSource();
                    }
                }

                // Manejar la eliminación
                if (!selectedFile.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                    if (tryDeleteFile(selectedFile)) {
                        selectedFile = "";
                        UpdateDirectoryContents();
                    }
                }
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Error processing entry: " << e.what() << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Unexpected error: " << e.what() << std::endl;
            }
        }
    }
    ImGui::End();
}

