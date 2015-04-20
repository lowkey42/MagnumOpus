#version auto

in vec2 uvl;

uniform sampler2D texture;
uniform vec4 color;

void main() {
	vec4 c = texture2D(texture, uvl);
		
	if(c.a>0.1) {
		gl_FragColor = c * color;
	}else
		discard;
}
