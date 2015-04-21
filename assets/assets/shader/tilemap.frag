#version auto

in vec2 UV;

uniform sampler2D myTextureSampler;

void main() {
	gl_FragColor.rgb = texture(myTextureSampler, UV).rgb;
	gl_FragColor.a = 1.0;
}

