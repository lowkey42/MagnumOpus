#version 100
precision mediump float;

varying vec2 uvl;

uniform sampler2D tex;
uniform float health;
uniform float health_anim;

vec4 calc(vec4 hb, float h) {
	if(hb.a>0.0 && hb.a>=(1.0-h)) {
		vec3 corrHb =hb.rgb/(hb.a*1.1);

		float r = 1.0-health;
		r*=r;
		corrHb += vec3(r, -r, 0.0);

		return vec4(corrHb, 1.0);

	} else
		return vec4(0.0, 0.0, 0.0, 0.0);
}

void main() {
	vec4 hb = texture2D(tex, uvl);
	
	vec4 color = calc(hb, health);

	if(color.a<=0.0) {
		color = calc(hb, health_anim) * vec4(2.0,0.1,0.1,1);
	}

	if(color.a>0.0)
		gl_FragColor = color;
	else
		discard;
}
