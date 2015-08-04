#version auto

in vec2 position;
in vec2 uv;

out vec2 uvl;

uniform mat4 VP;

void main() {
    gl_Position = VP * vec4(position.x, position.y, 1, 1);

	uvl = uv;
}
