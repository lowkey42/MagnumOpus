#version 100
precision mediump float;

attribute vec2 position;
attribute vec2 uv;

varying vec2 uvl;

uniform mat4 VP;
uniform mat4 model;
uniform float layer;

void main() {
	gl_Position = (VP*model) * vec4(position.x, position.y, layer, 1.0);

	uvl = uv;
}
