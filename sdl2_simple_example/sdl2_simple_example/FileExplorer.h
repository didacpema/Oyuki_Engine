#pragma once
#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <imgui.h>
#include "MyWindow.h"
#include "FilesystemUtils.h"
#include <string>
#include <vector>

class Explorer {
public:
	//MyWindow* window
	Explorer();
	~Explorer();

	void draw();
	void UpdateDirectoryContents();
	void handleFileDrop(const std::vector<std::string>& paths);

	int getSelectedFileIndex() const { return selectedFileIndex; }
	std::string getSelectedFilePath() const {
		return selectedFileIndex >= 0 && selectedFileIndex < directoryContents.size()
			? directoryContents[selectedFileIndex]
			: "";
	}
private:
	//MyWindow* _myWindow;
	glm::ivec2 selectedResolution;
	bool isFullscreen = false;

	std::string currentDirectory = "Library/Meshes";
	std::vector<std::string> directoryContents;
	int selectedFileIndex = -1;  

	void deleteSelectedFile(); 
};


#endif // FILEEXPLORER_H