#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length

// Create two uniforms, 1 to get the diamond center and 
// The other to get the shape side length 
uniform vec2 center;
uniform float side_length;
uniform vec4 inside_color=vec4(1.,0.,0.,1.);
uniform vec4 outside_color=vec4(0.,0.,0.,1.);

void main(){
    //TODO: Write code that will draw the diamond

    //  Using the euclidian distance, check if the distance of the current pixel (gl_FragCoord)
    //  from the diamond center is larger than half of the shape side length, if so, use outside_color the pixel
    //  else, use inside_color to color the pixel
    //  to make calculations more easier we compare the absolute values to work in one quartre in the grid 

    vec4 euclidean_distance=abs(gl_FragCoord-vec4(center,0,0));
    float indicator=euclidean_distance.x+euclidean_distance.y;
    if(indicator<=side_length/2){
        frag_color=inside_color;
    }else{
        frag_color=outside_color;
    }
}