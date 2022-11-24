#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_errors.hpp"
#include "BlurTextureProgram.hpp"

class Blur {

    public:
        unsigned int Width, Height;

        unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
        unsigned int RBO; // RBO is used for multisampled color buffer
        unsigned int TextureID;

        unsigned int pingpongFBO[2];
        unsigned int pingpongColorbuffers[2];

        unsigned int VAO;

        bool horizontal = true;
        bool first_iteration = true;


        Blur(unsigned int width, unsigned int height);

        void BeginRender();

        void EndRender();

        void Render(unsigned int iters);

        void initRenderData();
};