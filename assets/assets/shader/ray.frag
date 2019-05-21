#version 100
precision mediump float;

uniform vec4 color;

varying float rp;

void main() {
	gl_FragColor = color * (1.0-abs(rp));
}
