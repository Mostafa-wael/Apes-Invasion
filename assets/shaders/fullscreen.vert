#version 330 core

out vec4 shape_color;

void main(){
    const vec3 TRIANGLE[3] = vec3[3](
        vec3(-1.0, -1.0, 0.0),
        vec3( 3.0, -1.0, 0.0),
        vec3(-1.0,  3.0, 0.0)
    );

    gl_Position = vec4(TRIANGLE[gl_VertexID], 1.0);



    
    const vec4 colors[3] = vec4[3] (
        vec4(1.0, 0.0, 0.0, 1.0),
        vec4(0.0, 1.0, 0.0, 1.0),
        vec4(0.0, 0.0, 1.0, 1.0)
    );
    shape_color = colors[gl_VertexID];
}