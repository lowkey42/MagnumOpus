#version auto

in vec2 uvl;

uniform sampler2D texture;
uniform bool horiz;

vec4 my_read_px(float o, float w) {
	float off = o/512.0;

	return texture2D( texture, vec2(uvl)+vec2(horiz ? off : 0.0, horiz ? 0.0 : off) ) * w
	     + texture2D( texture, vec2(uvl)-vec2(horiz ? off : 0.0, horiz ? 0.0 : off) ) * w;
}

void main() {
	gl_FragColor = texture2D( texture, vec2(uvl) ) * 0.2270270270
	             + my_read_px(1.3846153846, 0.3162162162)
	             + my_read_px(3.2307692308, 0.0702702703);
}

