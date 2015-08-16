#version auto

in vec2 uvl;

uniform sampler2D texture;
uniform vec4 color;
uniform vec4 clip = vec4(0,0,1,1);

void main() {
	vec4 c = texture2D(texture, uvl*clip.zw + clip.xy);

	if(c.a>0.1) {
		gl_FragColor = c * color;
	}else
		discard;
}
