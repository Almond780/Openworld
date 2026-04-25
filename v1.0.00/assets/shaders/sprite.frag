#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D spriteTexture;  // unused for now, but ready

void main() {
    FragColor = vec4(ourColor, 1.0);
}