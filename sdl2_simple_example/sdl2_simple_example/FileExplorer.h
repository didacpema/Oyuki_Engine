#pragma once
#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <imgui.h>
#include "MyWindow.h"
#include "FilesystemUtils.h"


class Explorer {
public:
	//MyWindow* window
	Explorer();
	~Explorer();

	void draw();
	void UpdateDirectoryContents();
private:
	//MyWindow* _myWindow;
	glm::ivec2 selectedResolution;
	bool isFullscreen = false;
	std::string currentDirectory = "Library/Meshes";
	std::vector<std::string> directoryContents;
};


#endif // FILEEXPLORER_H