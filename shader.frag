#version 440

// must have the same name as its corresponding "out" item in the vert shader
smooth in vec2 textureCoordinate;
uniform sampler2D tex;

// because gl_FragColor was apparently deprecated as of version 120 (currently using 440)
// Note: If I set gl_FragColor to an intermediate vec4, then the "glFragColor is deprecated"
// compiler error goes away.  I'd rather not rely on this though, so I will follow good practice
// and define my own.
out vec4 finalFragColor;

void main()
{
    // retrieve the texture values from the sampler (??you sure??), which was formatted as RGBA
    // Note: Texture2D(...) can be explicitly called, or the shader compilation can figure
    // out from the sampler type and the texture position type that this is a 2D texture.
    // Also Note: Cannot use an integer for the sampler.  Even though the sampler is understood
    // as sampler 0, 1, 2, 3, etc., it is not an integer.  Attempting to make it one will cause
    // the shader to fail compilation.
    finalFragColor = texture2D(tex, textureCoordinate).rgba;
}
