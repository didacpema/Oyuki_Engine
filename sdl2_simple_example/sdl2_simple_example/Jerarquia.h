#pragma once
#ifndef JERARQUIA_H
#define JERARQUIA_H

#include <imgui.h>
#include "MyWindow.h"
#include "Scene.h"
class Jerarquia {
public:
	Jerarquia();
	~Jerarquia();
	static void draw();
private:
	static void drawGameObjectNode(GameObject* obj, const std::string& name, int index);
};

#endif // JERARQUIA_H
