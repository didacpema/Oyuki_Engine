#pragma once
#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "MyWindow.h"
#include "imgui.h"

class Inspector {
public:
	Inspector();
	~Inspector();
	void draw();
private:
	bool chekerOn = false;
	bool saved = false;

};

#endif // INSPECTOR_H