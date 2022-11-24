#include "BlurTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load< BlurTextureProgram > blur_texture_program(LoadTagEarly);

BlurTextureProgram::BlurTextureProgram() {
    program = gl_compile_program(
        // vertex shader
        "#version 330\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoords;\n"

        "out vec2 TexCoords;\n"

        "void main()\n"
        "{\n"
        "    TexCoords = aTexCoords;\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "}\n"
    ,
        // fragment shader
        "#version 330\n"

        "out vec4 FragColor;\n"
        "in vec2 TexCoords;\n"

        "uniform sampler2D image;\n"

        "uniform bool horizontal;\n"
        "uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);\n"

        "void main()\n"
        "{             \n"
        "    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel\n"
        "    vec3 result = texture(image, TexCoords).rgb * weight[0];\n"
        "    if(horizontal)\n"
        "    {\n"
        "        for(int i = 1; i < 5; ++i)\n"
        "        {\n"
        "            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
        "            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];\n"
        "        }\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        for(int i = 1; i < 5; ++i)\n"
        "        {\n"
        "            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
        "            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];\n"
        "        }\n"
        "    }\n"
        "    FragColor = vec4(result, 1.0);\n"
        "}\n"
    );
}

