#version auto

in vec3 c;

uniform float time;

void main() {
	gl_FragColor.rgb = c.rgb * abs(sin(time)*float(5.0));
	gl_FragColor.a = 1.0;
}
