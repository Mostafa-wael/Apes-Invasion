#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the radius

// Create two uniforms, 1 to get the circle radius and 
// The other to get the center
uniform float center;
uniform vec2 radius;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the circle
    
    // Using the euclidian distance, check if the distance of the current pixel (gl_FragCoord)
    // from the circle center is larger than the radius, if so, use outside_color the pixel
    // else, use inside_color to color the pixel

    if(sqrt(pow(gl_FragCoord.x - circleCenter.x, 2) + pow(center.y - center.y, 2)) <= radius){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}