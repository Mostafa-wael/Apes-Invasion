#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length
uniform vec2 center;
uniform float side_length;
uniform vec4 inside_color=vec4(1.,0.,0.,1.);
uniform vec4 outside_color=vec4(0.,0.,0.,1.);

void main(){
    //TODO: Write code that will draw the diamond
    vec4 euclidean_distance=abs(gl_FragCoord-vec4(center,0,0));
    float indicator=euclidean_distance.x+euclidean_distance.y;
    if(indicator<=side_length/2){
        frag_color=inside_color;
    }else{
        frag_color=outside_color;
    }
}