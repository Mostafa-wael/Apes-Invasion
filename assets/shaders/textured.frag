#version 330 core

// #include "light_common.glsl" 

// This will be used to compute the diffuse factor.
    float calculate_lambert(vec3 normal, vec3 light_direction){
        return max(0.0f, dot(normal, -light_direction));
    }

    // This will be used to compute the phong specular.
    float calculate_phong(vec3 normal, vec3 light_direction, vec3 view, float shininess){
        vec3 reflected = reflect(light_direction, normal);
        return pow(max(0.0f, dot(view, reflected)), shininess);
    }


    // This contains all the material properties for a single pixel.
    // We have an extra property "emissive" which is used when the pixel itself emits light.
    struct Material {
        vec3 diffuse;
        vec3 specular;
        vec3 ambient;
        vec3 emissive;
        float shininess;
    };

    // This contains all the material properties and texture maps for the object.
    struct TexturedMaterial {
        sampler2D albedo_map;
        vec3 albedo_tint;
        sampler2D specular_map;
        vec3 specular_tint;
        sampler2D ambient_occlusion_map;
        sampler2D roughness_map;
        vec2 roughness_range;
        sampler2D emissive_map;
        vec3 emissive_tint;
    };

    // This function samples the texture maps from the textured material and calculates the equivalent material at the given texture coordinates.
    Material sample_material(TexturedMaterial tex_mat, vec2 tex_coord){
        Material mat;
        // Albedo is used to sample the diffuse
        mat.diffuse = tex_mat.albedo_tint * texture(tex_mat.albedo_map, tex_coord).rgb;
        // Specular is used to sample the specular... obviously
        mat.specular = tex_mat.specular_tint * texture(tex_mat.specular_map, tex_coord).rgb;
        // Emissive is used to sample the Emissive... once again "obviously"
        mat.emissive = tex_mat.emissive_tint * texture(tex_mat.emissive_map, tex_coord).rgb;
        // Ambient is computed by multiplying the diffuse by the ambient occlusion factor. This allows occluded crevices to look darker.
        mat.ambient = mat.diffuse * texture(tex_mat.ambient_occlusion_map, tex_coord).r;

        // Roughness is used to compute the shininess (specular power).
        float roughness = mix(tex_mat.roughness_range.x, tex_mat.roughness_range.y,
            texture(tex_mat.roughness_map, tex_coord).r);
        // We are using a formula designed the Blinn-Phong model which is a popular approximation of the Phong model.
        // The source of the formula is http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
        // It is noteworthy that we clamp the roughness to prevent its value from ever becoming 0 or 1 to prevent lighting artifacts.
        mat.shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;

        return mat;
    }

in Varyings {
    vec4 color;
    vec2 tex_coord;
} fs_in;

out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;

struct Light {
    // This will hold the light type.
    int type;
    // This defines the color and intensity of the light.
    // Note that we no longer define different values for the diffuse and the specular because it is unrealistic.
    // Also, we skipped the ambient and we will use a sky light instead.
    vec3 color;

    // Position is used for point and spot lights. Direction is used for directional and spot lights.
    vec3 position, direction;
    // Attentuation factors are used for point and spot lights.
    float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic;
    // Cone angles are used for spot lights.
    float inner_angle, outer_angle;
    vec3 top_color, middle_color, bottom_color;
};

#define MAX_LIGHT_COUNT 16
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;

void main(){
    //TODO: (Req 6) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color

    // std::cout << "light_count: " << light_count << std::endl;

    frag_color = tint * fs_in.color * texture(tex, fs_in.tex_coord) * vec4(lights[0].color, 1.0f);
}