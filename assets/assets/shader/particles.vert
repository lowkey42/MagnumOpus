#version 100
precision mediump float;

attribute vec2 xy;
attribute vec2 uv;
attribute vec2 position;
attribute vec4 color;
attribute vec2 size;
attribute float frame;
attribute float rotation;

uniform mat4 vp;
uniform float layer;
uniform float frames;

varying vec2 tex_coords;
varying vec4 fcolor;

vec2 rotate(vec2 p, float a) {
	vec2 r = mat2(cos(a), -sin(a), sin(a), cos(a)) * p;

	return vec2(r.x, -r.y);
}

void main() {
	vec2 epos = position + rotate(xy*size,rotation);

	gl_Position = vp * vec4(epos.x, epos.y, layer, 1.0);

	tex_coords = vec2(uv.x*((frame+1.0)/frames), uv.y);
	fcolor = color;
}
