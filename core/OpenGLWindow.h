// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#ifdef WITH_SHADERS

#pragma once
#include "Window.h"
#include <string>
#include <vector>
#include <GL/glew.h> // or glad, depending on your setup

namespace agp
{
	class OpenGLWindow;
}

class agp::OpenGLWindow : public agp::Window
{
	protected:
		SDL_GLContext _glContext;
		SDL_Texture* _targetTexture; // render-to-texture target

		// OpenGL shader program and quad resources
		GLuint _program;
		GLuint _vao;
		GLuint _vbo;

		virtual Uint32 windowFlags() override;
		virtual void preWindowCreation() override;
		virtual void initWindow() override;
		virtual void initRenderer() override;
		virtual void initOpenGL();
		virtual void createShaderProgram();
		virtual void createFullScreenQuad();

	public:
		OpenGLWindow(const std::string& title, int width, int height);
		virtual ~OpenGLWindow();

		// Override render to apply the shader after scene rendering
		virtual void render(const std::vector<Scene*>& scenes) override;
};

#endif