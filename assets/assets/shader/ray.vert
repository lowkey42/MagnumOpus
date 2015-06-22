#version auto

in vec2 position;
in float radius_pos;
out float rp;

uniform mat4 vp;
uniform mat4 model;

void main() {
    gl_Position = (vp*model) * vec4(position.x, position.y, 0, 1);
	rp = radius_pos;
}
