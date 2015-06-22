#version auto

in vec2 position;
in vec2 uv;

out vec2 uvl;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(position.x, position.y, 0.9, 1);

	uvl = uv;
}
