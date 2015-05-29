#version auto

in vec2 uvl;

uniform sampler2D bg_tex;
uniform sampler2D fg_tex;
uniform sampler2D health_tex;

uniform float health;

void main() {
	vec4 bg = texture2D(bg_tex, uvl);
	vec4 fg = texture2D(fg_tex, uvl);
	vec4 hb = texture2D(health_tex, uvl);

	vec4 c = bg;
	
	if(hb.a>0 && hb.a>=(1-health)) {
		vec3 corrHb =hb.rgb/(hb.a*1.1);

		float r = 1-health;
		r*=r*1.5;
		corrHb += vec3(r, -r, 0);

		c = vec4(corrHb, 1);

	}

	c = vec4(c.rgb*(1-fg.a) + fg.rgb*fg.a, c.a+fg.a);

	if(c.a>0) {
		gl_FragColor = c;
	}else
		discard;
}
