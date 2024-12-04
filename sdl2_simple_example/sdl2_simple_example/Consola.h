#pragma once
#ifndef CONSOLA_H
#define CONSOLA_H

#include <string>
#include <vector>
#include <imgui.h>

class Consola {
public:
	Consola();
	~Consola();

	static void draw(std::vector<std::string> logMessage);
private:

};

#endif // CONSOLA_H