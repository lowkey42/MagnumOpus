#version 100
precision mediump float;

attribute vec3 position;
attribute vec2 vertexUV;

varying vec2 UV;

uniform mat4 MVP;

void main(){
	vec4 v = vec4(position.xyz, 1);
	gl_Position = MVP * v;

	UV = vertexUV;
}

