#version 330 core

layout(location=0)in vec3 position;
layout(location=1)in vec4 color;
layout(location=2)in vec2 tex_coord;
layout(location=3)in vec3 normal;

out Varyings{
    vec3 position;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
}vs_out;

uniform vec3 dynamic_position;
uniform int height;
uniform int width;

void main(){
    if(gl_VertexID == 0) {
        gl_Position = vec4(vec3(
            ((dynamic_position.x/(width/2))-1) - 0.08,
            -1*((dynamic_position.y/(height/2))-1) - 0.08,
            0),1.00);

        vs_out.position= vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    } else if(gl_VertexID == 1) {
        gl_Position = vec4(vec3(
            ((dynamic_position.x/(width/2))-1) + 0.08,
            -1*((dynamic_position.y/(height/2))-1) - 0.08,
            0),1.00);

        vs_out.position= vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    } else if(gl_VertexID == 2) {
        gl_Position = vec4(vec3(
            ((dynamic_position.x/(width/2))-1) + 0.08,
            -1*((dynamic_position.y/(height/2))-1) + 0.08,
            0),1.00);

        vs_out.position= vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    } else if(gl_VertexID == 3) {
        gl_Position = vec4(vec3(
            ((dynamic_position.x/(width/2))-1) - 0.08,
            -1*((dynamic_position.y/(height/2))-1) + 0.08,
            0),1.00);

        vs_out.position= vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    }

        if(color == vec4(1.0, 1.0, 1.0, 1.0))
            vs_out.color=color;
        vs_out.tex_coord=tex_coord;
        vs_out.normal=normal;
    }