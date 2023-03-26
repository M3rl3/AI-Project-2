#pragma once

#include <GLFW/glfw3.h>

class cRenderReticle {
public:

	cRenderReticle();
	~cRenderReticle();

	void Initialize(GLFWwindow* window, const char* glsl_version);
	void Update();
	void Shutdown();
};