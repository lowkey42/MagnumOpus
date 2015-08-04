#version auto

in vec2 uvl;

uniform sampler2D texture;
uniform bool horiz;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main() {
    gl_FragColor = texture2D( texture, vec2(uvl) ) * weight[0];
    
    for (int i=1; i<3; i++) {
    	float off = offset[i]/512.0;
    
        gl_FragColor += texture2D( texture, vec2(uvl)+vec2(horiz ? off : 0.0, horiz ? 0.0 : off) ) * weight[i];
        gl_FragColor += texture2D( texture, vec2(uvl)-vec2(horiz ? off : 0.0, horiz ? 0.0 : off) ) * weight[i];
    }
}
