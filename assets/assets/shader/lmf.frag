#version auto

in vec2 uvl;

uniform sampler2D texture;

void main() {
	vec4 c = texture2D(texture, uvl);

	float lum = 0.299*c.r + 0.587*c.g + 0.114*c.b;
	
	float maxc = max(max(c.r, c.g), c.b);

	if(c.a>0 && maxc>0.85 && lum>0.3) {
		gl_FragColor = c * maxc;
	}else
		discard;
}
