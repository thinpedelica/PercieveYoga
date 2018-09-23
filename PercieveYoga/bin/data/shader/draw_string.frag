#version 330

uniform vec4 color;

in  float v_progress;
out vec4  fragColor;

void main() {
    if (v_progress < 0.0) discard;
    fragColor = color;
}
