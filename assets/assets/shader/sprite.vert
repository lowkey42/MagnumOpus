#version auto

in vec2 position;
in vec2 vertexUV;

out vec2 UV;

uniform mat4 MVP;

void main(){
    vec4 v = vec4(position.x, position.y, 0.5, 1);
	gl_Position = MVP * v;

	UV = vertexUV;
}

