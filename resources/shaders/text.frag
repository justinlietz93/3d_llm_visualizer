#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    FragColor = textColor * sampled;
} 