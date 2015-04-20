#version auto

in vec2 position;
in vec2 uv;

out vec2 uvl;

uniform mat4 VP;
uniform mat4 model;
uniform float layer;

void main() {
    gl_Position = (VP*model) * vec4(position.x, position.y, layer, 1);

	uvl = uv;
}
