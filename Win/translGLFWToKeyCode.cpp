/*
 * W - simple cross-platform OpenGL windows
 *
 * ===========================
 *  translGLFWToKeyCode.cpp
 * ===========================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Event.h"
#include "GLFW/glfw3.h"
#include <map>

static std::map<int, W::KeyCode::T> _translMap;

 W::KeyCode::T _translGLFWToKeyCode(int glfwKey) {

	W::KeyCode::T k = W::KeyCode::UNKNOWN;
	 
	if (glfwKey == GLFW_KEY_0) k = W::KeyCode::_0;
	else if (glfwKey == GLFW_KEY_1) k = W::KeyCode::_1;
	else if (glfwKey == GLFW_KEY_2) k = W::KeyCode::_2;
	else if (glfwKey == GLFW_KEY_3) k = W::KeyCode::_3;
	else if (glfwKey == GLFW_KEY_4) k = W::KeyCode::_4;
	else if (glfwKey == GLFW_KEY_5) k = W::KeyCode::_5;
	else if (glfwKey == GLFW_KEY_6) k = W::KeyCode::_6;
	else if (glfwKey == GLFW_KEY_7) k = W::KeyCode::_7;
	else if (glfwKey == GLFW_KEY_8) k = W::KeyCode::_8;
	else if (glfwKey == GLFW_KEY_9) k = W::KeyCode::_9;

	else if (glfwKey == GLFW_KEY_Q) k = W::KeyCode::_Q;
	else if (glfwKey == GLFW_KEY_W) k = W::KeyCode::_W;
	else if (glfwKey == GLFW_KEY_E) k = W::KeyCode::_E;
	else if (glfwKey == GLFW_KEY_R) k = W::KeyCode::_R;
	else if (glfwKey == GLFW_KEY_T) k = W::KeyCode::_T;
	else if (glfwKey == GLFW_KEY_Y) k = W::KeyCode::_Y;
	else if (glfwKey == GLFW_KEY_U) k = W::KeyCode::_U;
	else if (glfwKey == GLFW_KEY_I) k = W::KeyCode::_I;
	else if (glfwKey == GLFW_KEY_O) k = W::KeyCode::_O;
	else if (glfwKey == GLFW_KEY_P) k = W::KeyCode::_P;
	else if (glfwKey == GLFW_KEY_A) k = W::KeyCode::_A;
	else if (glfwKey == GLFW_KEY_S) k = W::KeyCode::_S;
	else if (glfwKey == GLFW_KEY_D) k = W::KeyCode::_D;
	else if (glfwKey == GLFW_KEY_F) k = W::KeyCode::_F;
	else if (glfwKey == GLFW_KEY_G) k = W::KeyCode::_G;
	else if (glfwKey == GLFW_KEY_H) k = W::KeyCode::_H;
	else if (glfwKey == GLFW_KEY_J) k = W::KeyCode::_J;
	else if (glfwKey == GLFW_KEY_K) k = W::KeyCode::_K;
	else if (glfwKey == GLFW_KEY_L) k = W::KeyCode::_L;
	else if (glfwKey == GLFW_KEY_Z) k = W::KeyCode::_Z;
	else if (glfwKey == GLFW_KEY_X) k = W::KeyCode::_X;
	else if (glfwKey == GLFW_KEY_C) k = W::KeyCode::_C;
	else if (glfwKey == GLFW_KEY_V) k = W::KeyCode::_V;
	else if (glfwKey == GLFW_KEY_B) k = W::KeyCode::_B;
	else if (glfwKey == GLFW_KEY_N) k = W::KeyCode::_N;
	else if (glfwKey == GLFW_KEY_M) k = W::KeyCode::_M;
	
	else if (glfwKey == GLFW_KEY_SPACE) k = W::KeyCode::SPACE;
	else if (glfwKey == GLFW_KEY_LEFT)  k = W::KeyCode::LEFT_ARROW;
	else if (glfwKey == GLFW_KEY_RIGHT) k = W::KeyCode::RIGHT_ARROW;
	else if (glfwKey == GLFW_KEY_UP)    k = W::KeyCode::UP_ARROW;
	else if (glfwKey == GLFW_KEY_DOWN)  k = W::KeyCode::DOWN_ARROW;

	else if (glfwKey == GLFW_KEY_ESCAPE) k = W::KeyCode::ESC;
	else if (glfwKey == GLFW_KEY_ENTER)  k = W::KeyCode::RETURN;
	else if (glfwKey == GLFW_KEY_BACKSPACE) k = W::KeyCode::BACKSPACE;
	else if (glfwKey == GLFW_KEY_DELETE) k = W::KeyCode::_DELETE;
	else if (glfwKey == GLFW_KEY_TAB)    k = W::KeyCode::TAB;
	else if (glfwKey == GLFW_KEY_HOME)   k = W::KeyCode::HOME;
	else if (glfwKey == GLFW_KEY_END)    k = W::KeyCode::END;

	return k;
}
