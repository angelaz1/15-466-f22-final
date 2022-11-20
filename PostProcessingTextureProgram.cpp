#include "PostProcessingTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load< PostProcessingTextureProgram > post_processing_texture_program(LoadTagEarly);

PostProcessingTextureProgram::PostProcessingTextureProgram() {
    // Texture program taken from https://learnopengl.com/In-Practice/2D-Game/Postprocessing
	program = gl_compile_program(
		//vertex shader:
		"#version 330 core\n"
        "layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>\n"
        "out vec2 TexCoords;\n"
        "uniform bool  chaos;\n"
        "uniform bool  confuse;\n"
        "uniform bool  shake;\n"
        "uniform float time;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(vertex.xy, 0.0f, 1.0f);\n"
        "    vec2 texture = vertex.zw;\n"
        "    if (chaos)\n"
        "    {\n"
        "        float strength = 0.3;\n"
        "        vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength);\n"        
        "        TexCoords = pos;\n"
        "    }\n"
        "    else if (confuse)\n"
        "    {\n"
        "        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        TexCoords = texture;\n"
        "    }\n"
        "    if (shake)\n"
        "    {\n"
        "        float strength = 0.01;\n"
        "        gl_Position.x += cos(time * 10) * strength;\n"     
        "        gl_Position.y += cos(time * 15) * strength;\n"     
        "    }\n"
        "}\n"
	,
		//fragment shader:
		"#version 330 core\n"
        "in  vec2  TexCoords;\n"
        "out vec4  color;\n"
        "uniform sampler2D scene;\n"
        "uniform vec2      offsets[9];\n"
        "uniform int       edge_kernel[9];\n"
        "uniform float     blur_kernel[9];\n"
        "uniform bool chaos;\n"
        "uniform bool confuse;\n"
        "uniform bool shake;\n"
        "void main()\n"
        "{\n"
        "    color = vec4(0.0f);\n"
        "    vec3 sample[9];\n"
        "    // sample from texture offsets if using convolution matrix\n"
        "    if(chaos || shake)\n"
        "        for(int i = 0; i < 9; i++)\n"
        "            sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));\n"
        "    // process effects\n"
        "    if (chaos)\n"
        "    {           \n"
        "        for(int i = 0; i < 9; i++)\n"
        "            color += vec4(sample[i] * edge_kernel[i], 0.0f);\n"
        "        color.a = 1.0f;\n"
        "    }\n"
        "    else if (confuse)\n"
        "    {\n"
        "        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);\n"
        "    }\n"
        "    else if (shake)\n"
        "    {\n"
        "        for(int i = 0; i < 9; i++)\n"
        "            color += vec4(sample[i] * blur_kernel[i], 0.0f);\n"
        "        color.a = 1.0f;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        color =  texture(scene, TexCoords);\n"
        "    }\n"
        "}\n"
	);
}

PostProcessingTextureProgram::~PostProcessingTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}

