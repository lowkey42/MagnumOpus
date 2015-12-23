#version 100
precision mediump float;

attribute vec2 position;
attribute float radius_pos;

varying float rp;

uniform mat4 vp;
uniform mat4 model;

void main() {
	gl_Position = (vp*model) * vec4(position.x, position.y, 0, 1);
	rp = radius_pos;
}
