#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sceneTexture;
uniform vec2 playerPosition;
uniform float radius;
uniform vec2 resolution;

void main()
{
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