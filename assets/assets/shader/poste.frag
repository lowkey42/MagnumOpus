#version 100
precision mediump float;

varying vec2 uvl;

uniform sampler2D texture;
uniform sampler2D lightmap;
uniform float fade;
uniform float brightness;
uniform float saturate;


vec3 saturation(vec3 c, float change) {
	vec3 f = vec3(0.299,0.587,0.114);
	float p = sqrt(c.r*c.r*f.r + c.g*c.g*f.g + c.b*c.b*f.b);

	return vec3(p) + (c-vec3(p))*vec3(change);
}

void main() {
	vec4 c = texture2D(texture, uvl);
	vec4 lc = texture2D(lightmap, uvl);

	vec4 final_color = c*brightness*1.1 + lc*4.0;

	if(final_color.r<0.9)
		final_color.rgb = saturation(final_color.rgb, saturate);

	if(fade>0.0) {
		float x = (uvl.x - 0.5) * 2.0;
		float y = (uvl.y - 0.5) * 2.0;
		final_color -= (x*x + y*y + 0.2 + lc) * fade;
	}

	float x = (uvl.x - 0.5) * 2.0;
	float y = (uvl.y - 0.5) * 2.0;
	final_color -= sqrt(x*x + y*y) * 0.5 - 0.1;

	gl_FragColor = final_color;
}
