#version auto

in vec2 position;
in vec2 uv;

out vec2 uvl;

uniform mat4 vp;
uniform mat4 model;

void main() {
    gl_Position = (vp*model) * vec4(position.x, position.y, 0.5, 1);

	uvl = uv;
}
