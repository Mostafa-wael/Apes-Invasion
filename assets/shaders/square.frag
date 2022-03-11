#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length

uniform vec2 center;
uniform float side_length;
uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the square
    vec4 euclidean_distance=abs(gl_FragCoord-vec4(center,0,0));
    float threshold = max(euclidean_distance.x, euclidean_distance.y);

    // this line is equivalent to the following code snippet.
    // However, using "mix" is more convenient when dealing with GPUs
    frag_color = mix(inside_color, outside_color, float(threshold > side_length/2));

    // if(threshold <= side_length/2){
    //     frag_color = inside_color;
    // } else {
    //     frag_color = outside_color;
    // }
}
