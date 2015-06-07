#version auto

in vec2 xy;
in vec2 uv;
in vec2 position;
in vec4 color;
in vec2 size;
in float frame;
in float rotation;

uniform mat4 vp;
uniform float layer;
uniform float frames;

out vec2 tex_coords;
out vec4 fcolor;

vec2 rotate(vec2 p, float a) {
	vec2 r = mat2(cos(a), -sin(a), sin(a), cos(a)) * p;

	return vec2(r.x, -r.y);
}

void main() {
	vec2 epos = position + rotate(xy*size,rotation);

    gl_Position = vp * vec4(epos.x, epos.y, layer, 1.0);

	tex_coords = vec2(uv.x*((frame+1.0)/frames), uv.y);
	fcolor = color;
}
