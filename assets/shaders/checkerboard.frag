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
    //This line calculates the normalized coordinates of the fragment. gl_FragCoord is a built-in variable in GLSL
    // that represents the pixel coordinates of the fragment. size is presumably a variable representing the size of the rendering area. 
    //The result, fragCoordNorm, is a 2D vector containing the normalized coordinates.
    vec2 fragCoordNorm = gl_FragCoord.xy / size;

    // Calculate the checkboard pattern
    // This line calculates the checkboard pattern. xCheck and yCheck are integers representing whether the fragment is in an even 
    //or odd position in the x and y directions, respectively.
    int xCheck = int(mod(fragCoordNorm.x, 2.0));
    int yCheck = int(mod(fragCoordNorm.y, 2.0));

    // Determine the color based on the checkboard pattern
    // they should be (in order): colors[0], colors[1] which means adjacent tiles should be different in colors
    // we use the xCheck and yCheck variables to determine the color by Xoring them together
    // i.e. if xCheck and yCheck are 1, then the color should be colors[1] (1 ^ 1 = 0)
    // where top most tile (start tile) is colors[0]
    vec3 finalColor = colors[xCheck ^ yCheck];

    // Set the output color
    frag_color = vec4(finalColor, 1.0);
}
