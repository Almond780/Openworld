#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Place the 2D quad in 3D space (it will be a flat plane facing the camera if billboarded,
    // but for a sign we'll rotate it explicitly)
    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
}