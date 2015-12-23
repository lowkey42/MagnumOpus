#version 100
precision mediump float;

varying vec2 uvl;

uniform sampler2D tex;

void main() {
	vec4 c = texture2D(tex, uvl);

	if(c.a>0.0) {
		gl_FragColor = c;
	} else
		discard;
}
