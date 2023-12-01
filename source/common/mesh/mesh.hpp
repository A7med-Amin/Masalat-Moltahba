#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: (Req 2) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            // ------------------------------------------------------------------------------------------------------------
            // Vertex buffer implementation
            // generate the vertex buffer object
            glGenBuffers(1, &VBO);
            // bind the vertex buffer object
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // 2nd attr: size of vertices vector in bytes
            // 3rd attr: data of vertices vector
            // 4th attr: static as rendered data won't change
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
            // ------------------------------------------------------------------------------------------------------------
            // Vertex array implementation
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            
            // 1st attr: position location in shader's program
            // 2nd attr: data type of position enteries
            // 3rd attr: normalize where true is to normalize
            // 4th attr: stride in bytes between each position
            // 5th attr: offset of position in bytes from the start of the buffer
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);

            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, color));
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);

            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);

            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            // ------------------------------------------------------------------------------------------------------------
            // Element buffer implementation
            elementCount = elements.size();
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);

            // unbind the VAO for more safety
            glBindVertexArray(0);
        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 2) Write this function
            // binf the vertex array object to retrive data and send it to shader
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void*)0);
            // inbind the vertex array object
            glBindVertexArray(0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 2) Write this function
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            elementCount = 0;
            // VAO = 0;
            // VBO = 0;
            // EBO = 0;
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}