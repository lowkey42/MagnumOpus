#version auto

in vec2 UV;

uniform sampler2D myTextureSampler;

void main() {
	
	vec4 tex = texture2D(myTextureSampler, UV);
	if(tex.a < 0.1){
		discard;
	}
	
	gl_FragColor = vec4(tex.r, tex.g, tex.b, tex.a);

	if(tex.a<=0.1)
		discard;
	
	//gl_FragColor.rgb = texture2D(myTextureSampler, UV).rgb;
	//gl_FragColor.a = 0.1;
}

