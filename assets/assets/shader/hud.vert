#version 100
precision mediump float;

attribute vec2 position;
attribute vec2 uv;

varying vec2 uvl;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(position.x, position.y, 0.9, 1);

	uvl = uv;
}
