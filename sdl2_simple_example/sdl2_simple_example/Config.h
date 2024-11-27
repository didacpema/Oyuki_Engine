#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <imgui.h>
#include "MyWindow.h"
#include "MemoryUsage.h"


class Config {
public:
	Config(MyWindow* window);
	~Config();

	void draw(SDL_Window* _window, Renderer renderer, float fps);
private:
	MyWindow* _myWindow;
	glm::ivec2 selectedResolution;
	bool isFullscreen = false;
};


#endif // CONFIG_H