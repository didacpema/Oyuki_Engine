#pragma once
#ifndef JERARQUIA_H
#define JERARQUIA_H

#include <imgui.h>
#include "MyWindow.h"
#include "GameObject.h"

class Jerarquia {
public:
	Jerarquia();
	~Jerarquia();
	static void draw();

private:
	// M�todo auxiliar para dibujar un objeto y manejar la jerarqu�a
	static void drawGameObject(GameObject* gameObject, size_t index);

};

#endif // JERARQUIA_H
