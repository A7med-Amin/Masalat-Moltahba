#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.


uniform int size = 32;
uniform vec3 colors[2];

void main(){
    // Calculate the normalized coordinates of the fragment
    vec2 fragCoordNorm = gl_FragCoord.xy / size;

    // Calculate the checkboard pattern
    int xCheck = int(mod(fragCoordNorm.x, 2.0));
    int yCheck = int(mod(fragCoordNorm.y, 2.0));

    // Determine the color based on the checkboard pattern
    // they should be (in order): colors[0], colors[1] which means adjacent tiles should be different in colors
    vec3 finalColor = colors[xCheck ^ yCheck];

    // Set the output color
    frag_color = vec4(finalColor, 1.0);
}
