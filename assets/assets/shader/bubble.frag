#version auto

in vec2 uvl;

uniform sampler2D texture;

void main() {
	vec4 c = texture2D(texture, uvl);

	gl_FragColor = c*2;
}
