#version 330 core

// Legacy OpenGL pass-through shader; current demos use the SDL_GPU shaders in postprocessing/.

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
    // Keep UV coordinates unchanged for the fragment shader.
    TexCoord = aTexCoord;
}