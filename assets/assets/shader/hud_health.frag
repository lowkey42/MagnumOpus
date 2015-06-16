#version auto

in vec2 uvl;

uniform sampler2D tex;

uniform float health;

void main() {
	vec4 hb = texture2D(tex, uvl);
	
	if(hb.a>0.0 && hb.a>=(1.0-health)) {
		vec3 corrHb =hb.rgb/(hb.a*1.1);

		float r = 1.0-health;
		r*=r*1.5;
		corrHb += vec3(r, -r, 0.0);

		gl_FragColor = vec4(corrHb, 1.0);

	} else
		discard;
}
