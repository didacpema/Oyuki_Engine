#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <imgui.h>

class Config {
public:
	Config();
	~Config();

	void draw(SDL_Window* _window);
	glm::ivec2 selectedResolution;
private:
};


#endif // CONFIG_H