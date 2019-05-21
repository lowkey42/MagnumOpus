#version 100
precision mediump float;

varying vec2 uv;

uniform sampler2D myTextureSampler;

void main(){
	gl_FragColor.rgb = texture2D(myTextureSampler, uv).rgb;
	gl_FragColor.a = 1.0;
}
