#pragma once

#include "GL.hpp"
#include "Load.hpp"

struct BlurTextureProgram {
    BlurTextureProgram();
    ~BlurTextureProgram();

    GLuint program = 0;
    //Attribute (per-vertex variable) locations:
    //Uniform (per-invocation variable) locations:
    //Textures:
    //TEXTURE0 - texture that is accessed by TexCoord
};

extern Load< BlurTextureProgram > blur_texture_program;