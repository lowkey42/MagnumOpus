#version auto

in vec2 position;
in vec2 in_uv;

out vec2 uv;

uniform mat4 MVP;

void main(){

	vec4 v = vec4(position.xy, 0, 1);
	gl_Position = MVP * v;

	uv = in_uv;

}