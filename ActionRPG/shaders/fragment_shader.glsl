#version 330 core

// Legacy OpenGL circular mask; it is not loaded by the current SDL_GPU pipeline.

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sceneTexture;
uniform vec2 playerPosition;
uniform float radius;
uniform vec2 resolution;

void main()
{
    // Convert normalized UVs to pixels before comparing with the player's radius.
    vec2 fragCoord = TexCoord * resolution;
    float distance = length(fragCoord - playerPosition);

    if (distance <= radius)
    {
        FragColor = texture(sceneTexture, TexCoord);
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black color
    }
}