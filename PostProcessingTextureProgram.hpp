#pragma once

#include "GL.hpp"
#include "Load.hpp"

//Shader program that draws transformed, vertices tinted with vertex colors:
struct PostProcessingTextureProgram {
	PostProcessingTextureProgram();
	~PostProcessingTextureProgram();

	GLuint program = 0;
	//Attribute (per-vertex variable) locations:
	//Uniform (per-invocation variable) locations:
	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord
};

extern Load< PostProcessingTextureProgram > post_processing_texture_program;
