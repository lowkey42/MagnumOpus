#version auto

in vec2 uvl;

uniform sampler2D texture;

uniform float time;

uniform float fill_level;

uniform float activity;

vec4 get(vec2 rp,float r, float offset, float speedX, float speedY) {
	vec2 p = vec2(speedX * time, speedY * time);

	vec2 uv=rp*r + p;
	uv-=floor(uv);

	return texture2D(texture, vec2(uv.x/2.0 + offset/2.0, uv.y));
}

vec4 bg1(vec2 p,float r) {
	return get(p,r, 0.0, -0.1, -0.05);
}
vec4 bg2(vec2 p,float r) {
	return get(p,r, 0.0, -0.4, -0.3);
}
vec4 bg3(vec2 p,float r) {
	return get(p,r, 0.0, -0.1, -0.3);
}

vec4 line(float offset) {
	float x = uvl.x + 0.5*time*offset;

	vec2 uv = vec2(x-floor(x), uvl.y + 1.0-fill_level -0.12);

	return texture2D(texture, vec2(uv.x/2.0 + 1.0/2.0, uv.y));
}

void main() {
	vec2 p = -1.0 + 2.0 * uvl;
	float r = clamp(sqrt(dot(p,p)), 0.0,0.9999);
	float f = (1.0-sqrt(1.0-r))/(r) / 2.0;

	vec4 c = (bg1(p,f) * bg2(p,f) *2.0)* bg3(p,f) *3.0;

	c.rgb /= pow(r*20.0, 1.0-activity);

	if(fill_level<1.0) {
		vec4 line = line(1.0) * line(-1.0);

		c+=line*0.7;

		if(0.98-uvl.y<=1.0-fill_level)
			c.a = line.r;
	}

	if(c.a<0.1)
		discard;
	else
		gl_FragColor = c;
}
