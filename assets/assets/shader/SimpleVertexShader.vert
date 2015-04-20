#version auto

in vec2 position;

in vec3 color;
out vec3 c;

uniform float time;

void main(){
    gl_Position.xy = vec2(position.x*sin(time), position.y)/1.5;
	gl_Position.z = 0.9;
    gl_Position.w = 1.0;

	c = color;
 }
