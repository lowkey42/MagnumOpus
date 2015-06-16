#version auto

in vec2 uvl;

uniform sampler2D texture;

uniform float time;

uniform float fill_level;

vec4 get(float offset, float speedX, float speedY) {
	vec2 p = vec2(speedX * time, speedY * time);
	vec2 uv = uvl+p;
	uv -= floor(uv);

	return texture2D(texture, vec2(uv.x/2.0 + offset/2.0, uv.y));
}

vec4 bg1() {
	return get(0, -0.2, -0.1);
}
vec4 bg2() {
	return get(0, -0.6, -0.5);
}
vec4 bg3() {
	return get(0, -0.1, -0.5);
}

vec4 line(float offset) {
	float x = uvl.x + 0.5*time*offset;

	vec2 uv = vec2(x-floor(x), uvl.y + 1-fill_level -0.12);

	return texture2D(texture, vec2(uv.x/2.f + 1.0/2.0, uv.y));
}

void main() {

	vec4 line = line(1) * line(-1);

	vec4 c = (bg1() * bg2() *2)* bg3() *3;
	c+=line*0.6;

	if(0.98-uvl.y<=1-fill_level)
		c.a = line.r;

	if(c.a<0.1)
		discard;
	else
		gl_FragColor = c;
}
