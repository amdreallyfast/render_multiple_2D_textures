#include "Texture.h"

#include "glload/include/glload/gl_4_4.h"
#include "glm/vec3.hpp"
#include "RandomColor.h"
#include <vector>

// I have decided that I will create a 2D texture of RGBA values.  The fragment shader's idea 
// of color is 0.0-1.0, so I will make each color channel (R, G, B, or alpha) as a float.  Other 
// format options are available if someone wants to get very specific about how they store 
// their texture data (ex: GL_RGBA32F is RGBA (RGB + alpha channel) with 32bits per channel,
// which might come in handy if the programmer was concerned that a user's "float" might not
// be 32bits), but for this demo, I will keep things relatively simple.
struct texel
{
    // do NOT define any methods or else the texel construction loop will be unable to use 
    // array-construction syntax (using curly braces) 
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;  // alpha
};

/*-----------------------------------------------------------------------------------------------
Description:
    Encapsulates the generation of texture values so that the CreateTexture() function can focus 
    on the OpenGL texture info.

    Creates three horizontal beams of random color (think of a three-stripe flag).  These are
    sufficient for a barebones demo.
Parameters: 
    putDataHere     Self-explanatory.  Is a pointer because that is my style of indicating 
                    "will be changed".
    maxTexelRows    Self-explanatory.
    texelsPerRow    A texture is a rectangle, and I thought this was more descriptive than 
                    "columns".
Returns:    None
Exception:  Safe
Creator:    John Cox (6-19-2016)
-----------------------------------------------------------------------------------------------*/
static void GenerateThreeBeamTexture(std::vector<texel> *putDataHere,
    const unsigned int maxTexelRows, const unsigned int texelsPerRow)
{
    // hard-code full opacity (alpha = 1.0f)
    glm::vec3 color = RandomColor();
    texel topThird = { color.r, color.g, color.b, 1.0f };

    color = RandomColor();
    texel middleThird = { color.r, color.g, color.b, 1.0f };

    color = RandomColor();
    texel bottomThird = { color.r, color.g, color.b, 1.0f };

    // cache coherency is higher if I work in a local variable and then assign it to the argument
    // all at once at the end (I think)
    std::vector<texel> localTexelData(maxTexelRows * texelsPerRow);

    // Note: The documentation for glTexImage2D(...) says this about the order of the contents:
    // "The first element corresponds to the lower left corner of the texture image. Subsequent 
    // elements progress left-to-right through the remaining texels in the lowest row of the 
    // texture image, and then in successively higher rows of the texture image. The final 
    // element corresponds to the upper right corner of the texture image."  In other words,
    // OpenGL's idea of a rectangle is one that starts in the lower left and proceeds to the 
    // upper right, unlike most other programming ideas of a rectangle, which start at the upper
    // left and proceed to the lower right.  Woohoo for different rectangle standards.
    for (size_t rowCounter = 0; rowCounter < maxTexelRows; rowCounter++)
    {
        for (size_t colCounter = 0; colCounter < texelsPerRow; colCounter++)
        {
            // this array-style assignment is only possible when the struct has no methods 
            // Note: Even a constructor that takes nothing and does nothing will prevent this.
            texel t = { 0.0f, 0.0f, 0.0f, 0.0f };
            if (rowCounter < (maxTexelRows / 3))
            {
                t = bottomThird;
            }
            else if (rowCounter < ((2 * maxTexelRows) / 3))
            {
                t = middleThird;
            }
            else
            {
                t = topThird;
            }

            // jam the data into the data structure
            localTexelData[(rowCounter * texelsPerRow) + colCounter] = t;
        }
    }

    // all done, so shove the data all at once into the argument
    *putDataHere = localTexelData;
}

/*-----------------------------------------------------------------------------------------------
Description:
    Encapsulates the creation of a texture (not the texture parameters; 
    see CreateGenericSampler()).  
    
    These are the basics:
    - Generate a texture ID
    - Binds it (the active texture unit is irrelevant here; whatever texture unit is current set 
        gets run over by this binding, butthe association of a texture unit with a texture is a 
        render time thing, so don't worry about the run over)
    - Generate the data for a 2D texture (in this case, three random color horizontal bars)
    - Load the data into the texture object buffer for the texture ID
    - Returns that texture ID.

    Note: There are two options for telling OpenGL the texture parameters:
    (1) Bind a texture with glBindTexture(...) and using glTexParameteri(...).  This tells OpenGL
    to use those texture parameters for whatever texture is in use.
    (2) Create a texture sampler, use glSamplerParameteri(...), and then associate a texture and 
    texture sampler with the same texture unit at draw time.  

    This tutorial uses the latter approach.  Approach (1) needs to be set whenever the currently
    bound texture needs a different setup than the texture that came before it.  Approach (2) is
    decoupled from the texture itself, hence there is no need to associate it syntactically with
    the texture or textures that it refers to.  
    
    In this way, texture samplers are unlike VAOs, whose vertex array attributes are associated 
    with whatever array buffer ID was bound at the time that those attributes were set up.  While
    maybe seeming like a bit of a hassle, this is the only way that multiple textures with 
    different parameters can be used at the same time.
    
Parameters: None
Returns:
    The OpenGL ID of the texture that was created.
Exception:  Safe
Creator:    John Cox (6-24-2016)
-----------------------------------------------------------------------------------------------*/
GLuint CreateRandom3BeamTexture()
{
    // create a 2D texture buffer
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // glTexImage2D(...) will take a pointer to the data, but not a pointer to pointer, so 2D
    // arrays are not an option and the 2D texture data must be crammed into a 1D array
    const unsigned int MAX_TEXEL_ROWS = 64;
    const unsigned int TEXELS_PER_ROW = 64;
    std::vector<texel> crude2DTextureIn1D;
    GenerateThreeBeamTexture(&crude2DTextureIn1D, MAX_TEXEL_ROWS, TEXELS_PER_ROW);

    // upload the texture to the GPU
    // Note: The function glTexImage2D(...) is how texture data is sent to the GPU.  The 
    // documentation (http://docs.gl/gl4/glTexImage2D) specifies what the individual arguments 
    // mean.  
    // void glTexImage2D(GLenum target, // making a 2D texture, so use GL_TEXTURE_2D
    //     GLint level,                 // somekind of "level of detail" thing; leave at 0
    //     GLint internalFormat,        // how we want OpenGL to store the data we provide
    //     GLsizei width,               // how many texels wide
    //     GLsizei height,              // texels tall (for GL_TEXTURE_2D; GL_TEXTURE_1D different)
    //     GLint border,                // documentation says to only use 0 (probably legacy)
    //     GLenum format,               // the format of the data that we provide
    //     GLenum type,                 // integer, unsigned byte, float, etc.
    //     const GLvoid * data);        // pointer to data
    // Also Note: The type and format will tell OpenGL how many bytes the texture requires.
    GLint level = 0;
    GLenum type = GL_FLOAT;
    GLenum format = GL_RGBA;            // texel data provided as 4-float sets
    GLint internalFormat = GL_RGBA;     // store the texel data in the same way
    GLsizei width = TEXELS_PER_ROW;
    GLsizei height = MAX_TEXEL_ROWS;
    GLint border = 0;                   // documentation says 0 (must be legacy)
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, crude2DTextureIn1D.data());

    // clean up bindings
    glBindTexture(GL_TEXTURE_2D, 0);

    // give back a handle to what was created
    return textureId;
}

/*-----------------------------------------------------------------------------------------------
Description:
    Encapsulates the creation of a texture sampler (texture parameters).  All the textures in 
    this barebones project are the same, so only a single sampler is required, hence the small
    function and the "generic" in its name.

    A sampler is an encapsulation of texture parameters such that glTexParameteri(...), which 
    only binds to the currently bound texture, doesn't have to be called every time the currently
    bound texture unit changes.
Parameters: None
Returns:
    The OpenGL ID of the sampler that was created.
Exception:  Safe
Creator:    John Cox (6-24-2016)
-----------------------------------------------------------------------------------------------*/
GLuint CreateGenericSampler()
{
    GLuint samplerId;
    glGenSamplers(1, &samplerId);

    // these are some kind of standard texture settings for how to magnify it (detail when 
    // zooming in), "minify" it (detail when zooming out), and set tiling.
    // Note: I don't know how these work or what they do in detail, but they seem to be common
    // in a few texture tutorials.
    //??why wouldn't the program work at all without the "min filter"? it'll work without the
    // other glTexParameteri(...) settings, but not without the "min filter"??
    glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);   // "S" is texture X axis
    glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);   // "T" is texture Y axis
    glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // "zoom in"
    glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // "zoom out"

    return samplerId;
}
