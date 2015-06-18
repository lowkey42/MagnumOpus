#version auto

uniform vec4 color;

in float rp;

void main() {
	gl_FragColor = color * (1.0-abs(rp));
}
