#include "PrimitiveGeneration.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glload/include/glload/gl_4_4.h"
#include "RandomColor.h"
#include <cmath>

/*-----------------------------------------------------------------------------------------------
Description:
    Creates a (in window coordinates) 0.5 wide by 0.5 tall triangle with texture coordinates
    that hit the bottem left, bottom right, and top center of whatever texture will be used with
    it.
Parameters:
    putDataHere     Self-explanatory. 
Returns:    None
Exception:  Safe
Creator:    John Cox (6-12-2016)
-----------------------------------------------------------------------------------------------*/
void GenerateTriangle(GeometryData *putDataHere)
{
    // all points generated in window space
    MyVertex localVerts[] =
    {
        glm::vec2(-0.25f, -0.25f),      // left bottom corner
        glm::vec2(+0.0f, +0.0f),        // texel at bottom left of texture

        glm::vec2(+0.25f, -0.25f),      // right bottom corner
        glm::vec2(+1.0f, +0.0f),        // texel at bottom right of texture

        glm::vec2(+0.0f, +0.25f),       // center top
        glm::vec2(+0.5f, +1.0f),        // texel at top center of texture
    };

    unsigned int numVerts = sizeof(localVerts) / sizeof(MyVertex);
    for (size_t vertCounter = 0; vertCounter < numVerts; vertCounter++)
    {
        MyVertex &v = localVerts[vertCounter];
        putDataHere->_verts.push_back(v);
    }

    // // only three points, so indices are straightforward, going counterclockwise
    GLushort localIndices[] =
    {
        0, 1, 2,
    };

    unsigned int numIndices = sizeof(localIndices) / sizeof(unsigned short);
    for (size_t indicesCounter = 0; indicesCounter < numIndices; indicesCounter++)
    {
        putDataHere->_indices.push_back(localIndices[indicesCounter]);
    }

    putDataHere->_drawStyle = GL_TRIANGLES;
}

/*-----------------------------------------------------------------------------------------------
Description:
    Creates a (in window coordinates) 0.5 wide by 0.5 tall square with texture coordinates that 
    hit the 4 corners of whatever texture will be used with it.
Parameters:
    putDataHere     Self-explanatory.
Returns:    None
Exception:  Safe
Creator:    John Cox (6-12-2016)
-----------------------------------------------------------------------------------------------*/
void GenerateBox(GeometryData *putDataHere)
{
    // all points generated in window space
    MyVertex localVerts[] =
    {
        glm::vec2(-0.25f, -0.25f),      // left bottom corner
        glm::vec2(+0.0f, +0.0f),        // texel at bottom left of texture

        glm::vec2(+0.25f, -0.25f),      // right bottom corner
        glm::vec2(+1.0f, +0.0f),        // texel at bottom right of texture

        glm::vec2(+0.25f, +0.25f),      // right top
        glm::vec2(+1.0f, +1.0f),        // texel at top right of texture

        glm::vec2(-0.25f, +0.25f),      // left top
        glm::vec2(+0.0f, +1.0f),        // texel at top left of texture
    };

    unsigned int numVerts = sizeof(localVerts) / sizeof(MyVertex);
    for (size_t vertCounter = 0; vertCounter < numVerts; vertCounter++)
    {
        MyVertex &v = localVerts[vertCounter];
        putDataHere->_verts.push_back(v);
    }

    // indices going counterclockwise
    GLushort localIndices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };

    unsigned int numIndices = sizeof(localIndices) / sizeof(unsigned short);
    for (size_t indicesCounter = 0; indicesCounter < numIndices; indicesCounter++)
    {
        putDataHere->_indices.push_back(localIndices[indicesCounter]);
    }

    putDataHere->_drawStyle = GL_TRIANGLES;
}

/*-----------------------------------------------------------------------------------------------
Description:
    Creates a 32-point, 0.25 radius (in window coordinates) circle with texture coordinates that
    will, on the 90-degree parts, hit the right center, top center, left center, and bottom 
    center of whatever texture is used with it.

    Note: I could have used sinf(...) and cosf(...) to create the points, but where's the fun in
    that if I have a faster and obtuse algorithm :) ?  Algorithm courtesy of 
    http://slabode.exofire.net/circle_draw.shtml .
Parameters:
    putDataHere     Self-explanatory.
Returns:    None
Exception:  Safe
Creator:    John Cox (6-12-2016)
-----------------------------------------------------------------------------------------------*/
void GenerateCircle(GeometryData *putDataHere)
{
    // a 32-point, 0.25 radius (window dimensions) circle will suffice for this demo
    unsigned int arcSegments = 32;
    float x = 0.25f;
    float y = 0.0f;
    float textureS = 0.5f;  // texture's "x"
    float textureT = 0.0f;  // texture's "y"
    glm::vec2 textureCenter(0.5f, 0.5f); 
    float theta = 2 * 3.1415926f / float(arcSegments);
    float tangetialFactor = tanf(theta);
    float radialFactor = cosf(theta);
    for (unsigned int segmentCount = 0; segmentCount < 32; segmentCount++)
    {
        MyVertex v;
        v._position = glm::vec2(x, y);

        // without adding the texture's center, the texture coordinates will center on 0, which 
        // will run over into another copy of the texture (not bad, but I want to demonstrate 
        // the use of a texture without unintential duplication)
        v._texturePosition = textureCenter + glm::vec2(textureS, textureT);
        putDataHere->_verts.push_back(v);

        float tx = (-y) * tangetialFactor;
        float ty = x * tangetialFactor;
        float textureTs = (-textureT) * tangetialFactor;    // textureTs = texture tangential S
        float textureTt = textureS * tangetialFactor;

        // add the tangential factor
        x += tx;
        y += ty;
        textureS += textureTs;
        textureT += textureTt;

        // correct using the radial factor
        x *= radialFactor;
        y *= radialFactor;
        textureS *= radialFactor;
        textureT *= radialFactor;
    }

    // make triangles out of the first vertex from the first arc segment, and then the two 
    // vertices from each successive arc segment (draw it out on paper; it makes sense)
    for (unsigned short segmentCount = 1; segmentCount < 32; segmentCount++)
    {
        putDataHere->_indices.push_back(0);
        putDataHere->_indices.push_back(segmentCount);
        putDataHere->_indices.push_back(segmentCount + 1);
    }

    putDataHere->_drawStyle = GL_TRIANGLES;
}

/*-----------------------------------------------------------------------------------------------
Description:
    Generates a vertex buffer, index buffer, and vertex array object (contains vertex array
    attributes) for the provided geometry data.
Parameters:
    programId   Program binding is required for vertex attributes.
    initThis    Self-explanatory.
Returns:    None
Exception:  Safe
Creator:    John Cox (6-12-2016)
-----------------------------------------------------------------------------------------------*/
void InitializeGeometry(GLuint programId, GeometryData *initThis)
{
    // must bind program or else the vertex arrays will either blow up or refer to a 
    // non-existent program
    glUseProgram(programId);

    // vertex array buffer
    GLuint arrayBufferId = 0;
    glGenBuffers(1, &arrayBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, arrayBufferId);
    initThis->_arrayBufferId = arrayBufferId;

    unsigned int vertBufferSizeBytes = 
        initThis->_verts.size() * sizeof(initThis->_verts[0]);
    glBufferData(GL_ARRAY_BUFFER, vertBufferSizeBytes, initThis->_verts.data(), GL_STATIC_DRAW);

    // vertex attribute arrays
    // Note: I really don't like these, but this is about as clear as I can make it.
    GLuint vaoId = 0;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    initThis->_vaoId = vaoId;

    unsigned int vertexAttribArrayIndex = 0;
    unsigned int vertexBufferStartOffset = 0;
    unsigned int strideSizeBytes = sizeof(initThis->_verts[0]);

    // position
    // Note: Just hard code a 2 for the number of floats in a vec2.
    glEnableVertexAttribArray(vertexAttribArrayIndex);
    glVertexAttribPointer(vertexAttribArrayIndex, 2, GL_FLOAT, GL_FALSE, 
        strideSizeBytes, (void *)vertexBufferStartOffset);

    // color
    // Note: Like for position, hard code the number of floats (2 for vec2).
    vertexAttribArrayIndex++;
    vertexBufferStartOffset += sizeof(initThis->_verts[0]._position);
    glEnableVertexAttribArray(vertexAttribArrayIndex);
    glVertexAttribPointer(vertexAttribArrayIndex, 2, GL_FLOAT, GL_FALSE, 
        strideSizeBytes, (void *)vertexBufferStartOffset);

    // indices buffer
    GLuint elementBufferId = 0;
    glGenBuffers(1, &elementBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
    initThis->_elementBufferId = elementBufferId;

    unsigned int elementBufferSizeBytes = 
        initThis->_indices.size() * sizeof(initThis->_indices[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSizeBytes, initThis->_indices.data(), GL_STATIC_DRAW);

    // must unbind array object BEFORE unbinding the buffer or else the array object will think 
    // that its vertex attribute pointers will believe that they should refer to buffer 0
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(0);
}