#pragma once
#ifndef ESCENA_H
#define ESCENA_H

#include <imgui.h>
#include "MyWindow.h"


class Escena {
public:
	Escena();
	~Escena();
	static void draw(GLuint texturecolorbuffer);
private:

};

#endif // ESCENA_H