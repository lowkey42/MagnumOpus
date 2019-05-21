#version 100
precision mediump float;

attribute vec2 position;
attribute vec2 in_uv;

varying vec2 uv;

uniform mat4 MVP;

void main(){

	vec4 v = vec4(position.xy, 0, 1);
	gl_Position = MVP * v;

	uv = in_uv;

}
