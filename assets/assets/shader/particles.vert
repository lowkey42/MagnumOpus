#version auto

in vec2 xy;
in vec2 uv;
in vec2 position;
in vec4 color;
in vec2 size;
in float frame;

uniform mat4 vp;
uniform float layer;
uniform float frames;

out vec2 tex_coords;
out vec4 fcolor;

void main() {
	vec2 epos = position + xy*size;

    gl_Position = vp * vec4(epos.x, epos.y, layer, 1.0);

	tex_coords = uv/frames + frame;
	fcolor = color;
}
