#pragma once

// this should be GLuint, but I try to avoid including OpenGL in headers if possible due to its
// sheer size
unsigned int CreateRandom3BeamTexture();
unsigned int CreateGenericSampler();
