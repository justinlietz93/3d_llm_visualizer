#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec4 color;
uniform float strength;

void main()
{
    FragColor = vec4(color.rgb, color.a * strength);
} 