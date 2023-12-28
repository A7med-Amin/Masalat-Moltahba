// DONE TODO: (Phase 2)

#define DIRECTIONAL 0
#define POINT       1
#define SPOT        2

#define MAX_LIGHT_COUNT 16

// in to shader from other shaders (like vertex shader)
in Varyings {
   vec4 color;
   vec2 tex_coord;
   vec3 world;
   vec3 view;
   vec3 normal;
} fsin;

struct Light {
    // DIRECTIONAL or POINT or SPOT
    int type;
    // Light position in world space  
    vec3 position;
    // Light direction (S vector), where it points
    vec3 direction;
    // Light color (amibent, specular and diffusion )
    vec3 color; 
    // Attenuation represents intensity of light with distance
    // As we move away from the light we reduce its intensity
    vec3 attenuation; 
    // Cone angles (inner and outer) for SPOT light only
    vec2 cone_angles; 
};

struct Sky {
    // Allows us to vary amibent light based on surface normal makes it much realistic compared to constant amibent light
    // intensity of light from sky in all directions
    vec3 top, horizon, bottom;
};

struct Material {
    sampler2D albedo;
    sampler2D specular;
    sampler2D roughness;
    sampler2D ambient_occlusion;
    sampler2D emissive;
};


// Uniforms
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;
uniform Sky sky;
uniform Material material;

// output from fragment shader
out vec4 frag_color;


// functions 

// It interpolates between the horizon color and either the top or bottom of the sky, depending on whether the normal vector 
// is pointing upwards or downwards. The degree of interpolation is determined by the square of the y-component of the normal vector
vec3 compute_sky_light(vec3 normal){
    // Determines whether the normal vector is pointing upwards or downwards based on its y-component. If it's pointing upwards 
    // (normal.y > 0), it uses sky.top as the extreme color; otherwise, it uses sky.bottom.
    vec3 extreme = normal.y > 0 ? sky.top : sky.bottom;

    // Linearly interpolates between sky.horizon and extreme based on the square of the y-component of the normal vector. 
    // The mix function in GLSL performs the linear interpolation. If normal.y * normal.y is close to 0, 
    // the result is close to sky.horizon. If normal.y * normal.y is close to 1, the result is close to extreme.
    return mix(sky.horizon, extreme, normal.y * normal.y);
}

float lambert(vec3 normal, vec3 world_to_light_direction) {
    return max(0.0f, dot(normal, world_to_light_direction));
}

float phong(vec3 reflected, vec3 view, float shininess) {
    // reflected = reflect(-world_to_light_dir, normal)
    return pow(max(0.0f, dot(reflected, view)), shininess);
}


void main() {
    vec3 normal = normalize(fsin.normal);
    vec3 view = normalize(fsin.view);

    vec3 ambient_light = compute_sky_light(normal);

    vec3 diffuse = texture(material.albedo, fsin.tex_coord).rgb;
    vec3 specular = texture(material.specular, fsin.tex_coord).rgb;
    // Used to sample shininess (specular power)
    float roughness = texture(material.roughness, fsin.tex_coord).r;
    vec3 ambient = diffuse * texture(material.ambient_occlusion, fsin.tex_coord).r;
    vec3 emissive = texture(material.emissive, fsin.tex_coord).rgb;

    // Using a formula designed the Blinn-Phong model which is a popular approximation of the Phong model.
    // The source of the formula is http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
    // It is noteworthy to clamp the roughness to prevent its value from ever becoming 0 or 1 to prevent lighting artifacts.
    float shininess = 2.0f / pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
    
    vec3 world_to_light_dir;

    // Initially the color holds ambient light and emissive light (light coming out of the object)
    vec3 color = emissive + ambient_light * ambient;

    for(int index = 0; index < min(MAX_LIGHT_COUNT, light_count); index++){
        Light light = lights[index];
        float attenuation = 1.0f;
        if(light.type == DIRECTIONAL){
            world_to_light_dir = -light.direction;
        } else {
            world_to_light_dir = light.position - fsin.world;
            float d = length(world_to_light_dir);
            world_to_light_dir /= d;
            attenuation = 1.0f / dot(light.attenuation, vec3(d*d, d, 1.0f));
            if(light.type == SPOT){
                float angle = acos(dot(light.direction, -world_to_light_dir));
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
            }
        }

        vec3 computed_diffuse = light.color * diffuse * lambert(normal, world_to_light_dir);

        vec3 reflected = reflect(-world_to_light_dir, normal);
        vec3 computed_specular = light.color * specular * phong(reflected, view, shininess);

        color += (computed_diffuse + computed_specular) * attenuation;
    }

    // TODO: (Phase 2) Make sure
    frag_color = fsin.color * vec4(color, 1.0f);
}

