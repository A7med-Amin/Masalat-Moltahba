// DONE TODO: (Phase 2) DONE
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

// Model matrix to transform object from object space to world space
uniform mat4 M;
// Inverse Modal transpose to scale normals with object scaling to ensure it perpendicular to object surface
uniform mat4 M_IT;
// View projection matrix to fix normal on object and not to rotate with the object rotation
// To achieve this we seperate M from VP 
uniform mat4 VP;
// Eye position in world space to know the object space with respect to it
uniform vec3 camera_position;

out Varyings {
   vec4 color;
   vec2 tex_coord;
   vec3 world;
   vec3 view;
   vec3 normal;
} vsout;

void main() {
   vsout.world = (M * vec4(position, 1.0f)).xyz;
   gl_Position = VP * vec4(vsout.world, 1.0);
   vsout.color = color;
   vsout.tex_coord = tex_coord;
   vsout.normal = normalize((M_IT * vec4(normal, 0.0f)).xyz);
   vsout.view = camera_position - vsout.world;
}

