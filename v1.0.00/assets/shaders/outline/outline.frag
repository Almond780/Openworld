#version 330 core
in vec3 gBarycentric;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 edgeColor = vec3(0.0, 0.0, 0.0);
uniform float edgeWidth = 0.05;

void main() {
    float minDist = min(min(gBarycentric.x, gBarycentric.y), gBarycentric.z);
    if (minDist < edgeWidth) {
        FragColor = vec4(edgeColor, 1.0);
    } else {
        FragColor = vec4(objectColor, 1.0);
    }
}