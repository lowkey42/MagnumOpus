#version 100
precision mediump float;

attribute vec2 position;
attribute vec2 uv;

varying vec2 uvl;

uniform mat4 VP;

void main() {
	gl_Position = VP * vec4(position.x, position.y, 1, 1);

	uvl = uv;
}
