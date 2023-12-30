#version 330 core
#define MAX_LIGHT_COUNT 16

in Varyings
{
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
}
fsin;
#define TYPE_DIRECTIONAL 0
#define TYPE_POINT 1
#define TYPE_SPOT 2
float lambert(vec3 normal, vec3 light_direction)
{
    return max(0.0f, dot(normal, light_direction));
}
float phong(vec3 reflected, vec3 view, float shininess)
{
    return pow(max(0.0f, dot(reflected, view)), shininess);
}
struct Light
{
    int type; // 0 = directional, 1 = point, 2 = spot
    vec3 color;
    vec3 position, direction;
    vec3 attenuation;
    float inner_angle, outer_angle;
};
struct Material
{
    vec3 diffuse, specular, ambient, emissive;
    float shininess;
};
struct MaterialProperties
{
    sampler2D albedo;
    sampler2D specular;
    sampler2D ambient_occlusion;
    sampler2D roughness;
    sampler2D emissive;
};
Material Fragment_material(MaterialProperties tex_mat, vec2 tex_coord)
{
    Material mat;
    mat.diffuse = texture(tex_mat.albedo, tex_coord).rgb;
    mat.specular = texture(tex_mat.specular, tex_coord).rgb;
    mat.emissive = texture(tex_mat.emissive, tex_coord).rgb;
    mat.ambient = mat.diffuse * texture(tex_mat.ambient_occlusion, tex_coord).r;
    float roughness = texture(tex_mat.roughness, tex_coord).r;
    mat.shininess = 2.0f / pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
    return mat;
}
// vec3 compute_sky_light(vec3 normal)
// {
//     vec3 extreme = normal.y > 0 ? sky.top : sky.bottom;
//     return mix(sky.horizon, extreme, normal.y * normal.y);
// }
uniform MaterialProperties material;
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;

out vec4 frag_color;
void main()
{
    vec3 normal = normalize(fsin.normal);
    vec3 view = normalize(fsin.view);
    Material material = Fragment_material(material, fsin.tex_coord);
    int count = min(light_count, MAX_LIGHT_COUNT);
    vec3 whole_light =  material.emissive + material.ambient; // Emissive light and ambient light may add sky later
    for (int index = 0; index < count; index++)
    {
        Light light = lights[index];
        vec3 light_direction;
        float attenuation = 1;
        if (light.type == TYPE_DIRECTIONAL)
            light_direction = -light.direction;
        else
        {
            light_direction = light.position-fsin.world;
            float d = length(light_direction);
            light_direction /= d;
            attenuation *= 1.0f / dot(light.attenuation, vec3(1,d,d*d));
            if (light.type == TYPE_SPOT)
            {
                float angle = acos(dot(light.direction, -light_direction));
                attenuation *= smoothstep(light.outer_angle, light.inner_angle, angle); // Decrease the attenuation as the angle increases
            }
        }   
        vec3 diffuse = material.diffuse * light.color * lambert(normal,light_direction); // Lamberitan diffuse
        vec3 reflected = reflect(-light_direction, normal);
        vec3 specular = material.specular * light.color * phong(reflected,view,material.shininess); //Phong specular
        vec3 ambient = material.ambient * light.color;
        whole_light += attenuation * (diffuse + specular);
    }
    frag_color = fsin.color * vec4(whole_light, 1.0f);
}