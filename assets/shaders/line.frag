#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the slope and the intercept

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

uniform float slope = 1.0;
uniform float intercept = 0.0;

void main(){
    float y = slope * gl_FragCoord.x + intercept;

    // Aspect ratio correction, needs resolution to be passed.
    // y = y*(iResolution.y/iResolution.x);  
    
    float above = float(gl_FragCoord.y <= y);
    frag_color=mix(outside_color,inside_color,above);
}