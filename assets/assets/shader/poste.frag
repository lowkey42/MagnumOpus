#version auto

in vec2 uvl;

uniform sampler2D texture;
uniform sampler2D lightmap;
uniform float fade;


void main() {
	vec4 c = texture2D(texture, uvl);
	vec4 lc = texture2D(lightmap, uvl);

	if(c.a>0.0) {
		gl_FragColor = c*1.0 + lc*4.0;
		
		if(fade>0.0) {
			float x = (uvl.x - 0.5) * 2.0;
			float y = (uvl.y - 0.5) * 2.0;
			gl_FragColor -= (x*x + y*y + 0.2 + lc) * fade;
		}

		float x = (uvl.x - 0.5) * 2.0;
		float y = (uvl.y - 0.5) * 2.0;
		gl_FragColor -= sqrt(x*x + y*y) * 0.4 - 0.04;

	}else
		discard;
}
