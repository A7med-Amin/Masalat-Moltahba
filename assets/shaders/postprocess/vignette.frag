#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;

out vec4 frag_color;

// Vignette is a postprocessing effect that darkens the corners of the screen
// to grab the attention of the viewer towards the center of the screen

void main(){
    //TODO: Modify this shader to apply vignette
    // To apply vignette, divide the scene color
    // by 1 + the squared length of the 2D pixel location the NDC space
    // Hint: remember that the NDC space ranges from -1 to 1
    // while the texture coordinate space ranges from 0 to 1
    // We have the pixel's texture coordinate, how can we compute its location in the NDC space?
    frag_color = texture(tex, tex_coord);   
    // Conversion of text_coordinates range (0 , 1) to NDC range(-1 , 1): 
    // here i make a scale to texture coordinates to cover the whole screen in NDC so, the range will be from
    // 0 -> 2 and after that i make -1 to make it (-1 , 1) the rangle of NDC space and the center will be (0 , 0)
    frag_color = frag_color / (1 + length(2 * tex_coord - 1) * length(2 * tex_coord - 1));
}