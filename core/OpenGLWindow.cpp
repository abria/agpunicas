// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria
// Released under the BSD License
// ----------------------------------------------------------------

#ifdef WITH_SHADERS

#include "OpenGLWindow.h"
#include "Scene.h"
#include "stringUtils.h"
#include <stdexcept>
#include <iostream>

using namespace agp;

static const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

static const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    // Neutral shader: just sample from the input texture
    FragColor = texture(uTexture, vTexCoord);
}
)glsl";


OpenGLWindow::OpenGLWindow(const std::string& title, int width, int height)
    : Window(title, width, height), _glContext(nullptr), _targetTexture(nullptr),
    _program(0), _vao(0), _vbo(0)
{
    // The Window base constructor calls SDL_Init, etc.
}

OpenGLWindow::~OpenGLWindow()
{
    // Clean up OpenGL resources
    if (_program) glDeleteProgram(_program);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_vao) glDeleteVertexArrays(1, &_vao);

    // Destroy target texture
    if (_targetTexture) SDL_DestroyTexture(_targetTexture);

    if (_glContext)
    {
        SDL_GL_MakeCurrent(_window, NULL);
        SDL_GL_DeleteContext(_glContext);
    }
}

Uint32 OpenGLWindow::windowFlags()
{
    // Enable OpenGL
    return Window::windowFlags() | SDL_WINDOW_OPENGL;
}

void OpenGLWindow::preWindowCreation()
{
    // Set OpenGL attributes before window creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Possibly set double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void OpenGLWindow::initWindow()
{
    // Call base implementation to create the window
    Window::initWindow();

    // Create the OpenGL context
    _glContext = SDL_GL_CreateContext(_window);
    if (!_glContext)
        throw SDL_GetError();

    // Initialize GLEW or GLAD
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw ("Failed to initialize GLEW");

    printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
    printf("GL_SHADING_LANGUAGE_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // V-Sync
    SDL_GL_SetSwapInterval(1);

    initOpenGL();
    createShaderProgram();
    createFullScreenQuad();
}

// Function to find the index of the OpenGL render driver
int findOpenGLRenderDriverIndex()
{
    int numDrivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < numDrivers; ++i)
    {
        SDL_RendererInfo info;
        if (SDL_GetRenderDriverInfo(i, &info) == 0)
        {
            if (strcmp(info.name, "opengl") == 0)
            {
                return i;
            }
        }
    }
    return -1; // OpenGL renderer not found
}

void OpenGLWindow::initRenderer()
{
    // Find OpenGL renderer index
    int openglIndex = findOpenGLRenderDriverIndex();
    if (openglIndex == -1)
        throw("OpenGL renderer not found!");

    // Create renderer with OpenGL backend
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    _renderer = SDL_CreateRenderer(
        _window,
        openglIndex,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );
    if (!_renderer)
        throw(strprintf("Failed to create SDL Renderer: %s)", SDL_GetError()));

    // Verify renderer info
    SDL_RendererInfo rendererInfo;
    if (SDL_GetRendererInfo(_renderer, &rendererInfo) == 0)
        std::cout << "Renderer Name: " << rendererInfo.name << std::endl;
    else
        std::cerr << "Failed to get renderer info: " << SDL_GetError() << std::endl;

    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

    // Create a target texture to render scenes into
    _targetTexture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        _width,
        _height
    );

    if (!_targetTexture)
        throw SDL_GetError();
}

void OpenGLWindow::initOpenGL()
{
    // Basic OpenGL state
    glViewport(0, 0, _width, _height);
    glDisable(GL_DEPTH_TEST);
}

void OpenGLWindow::createShaderProgram()
{
    // Utility function to compile a shader
    auto compileShader = [&](const char* src, GLenum type) 
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::string err = std::string("Shader compilation error: ") + infoLog;
            throw (err);
        }
        return shader;
    };

    GLuint vs = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fs = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    glLinkProgram(_program);

    int success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(_program, 512, nullptr, infoLog);
        std::string err = std::string("Program link error: ") + infoLog;
        throw (err);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void OpenGLWindow::createFullScreenQuad()
{
    // A simple full-screen quad covering the entire screen
    // Positions (x, y) and texture coords (u, v)
    float quadVertices[] = {
        //  Positions    Texcoords
        -1.0f,  1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f,

        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 0.0f
    };

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGLWindow::render(const std::vector<Scene*>& scenes)
{
    // Render scenes to the target texture
    SDL_SetRenderTarget(_renderer, _targetTexture);
    SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, 255);
    SDL_RenderClear(_renderer);

    //// Test: draw a red rect in the target texture
    //SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
    //SDL_Rect testRect = { 50, 50, 100, 100 };
    //SDL_RenderFillRect(_renderer, &testRect);
    for (auto scene : scenes)
        scene->render();

    // Make sure all rendering commands are done
    SDL_RenderFlush(_renderer);

    // Switch back to default render target
    SDL_SetRenderTarget(_renderer, nullptr);

    // Now use OpenGL to draw a fullscreen quad with the texture
    // Bind the SDL texture as an OpenGL texture
    float w, h;
    GLuint texID;
    if (SDL_GL_BindTexture(_targetTexture, &w, &h) != 0)
        throw ("Failed to bind SDL texture to OpenGL texture");

    // Render the quad
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_MakeCurrent(_window, _glContext);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "Clearing previous error: " << err << "\n";
    }
    glUseProgram(_program);
    err = glGetError();
    if (err != GL_NO_ERROR)
        throw(strprintf("OpenGL error: %d", err));

    // The texture is bound by SDL_GL_BindTexture(), which should give us an active texture
    // Typically it binds to GL_TEXTURE_2D at texture unit 0.
    glActiveTexture(GL_TEXTURE0);
    // SDL_GL_BindTexture doesn't return the texture ID directly, but it sets the bound texture.
    // We assume here that the texture is now bound to GL_TEXTURE_2D.
    // If needed, we can query the texture handle through other means, but generally it's handled internally.

    // Set our uniform
    GLint loc = glGetUniformLocation(_program, "uTexture");
    if (loc == -1) {
        throw ("Could not find uniform uTexture in shader");
    }
    glUniform1i(loc, 0);

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind the texture from OpenGL
    SDL_GL_UnbindTexture(_targetTexture);

    // Swap the window buffers to present the final image
    SDL_GL_SwapWindow(_window);
}

#endif