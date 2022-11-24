#include "Blur.hpp"

Blur::Blur(unsigned int width, unsigned int height) {
    // initialize renderbuffer/framebuffer object
//     glGenFramebuffers(1, &this->MSFBO);
//     glGenFramebuffers(1, &this->FBO);
//     glGenRenderbuffers(1, &this->RBO);
//     // initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
//     glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
//     glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
//     glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height); // allocate storage for render buffer object
//     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // attach MS render buffer object to framebuffer
//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//         std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
//     // also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
//     glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

   { // this->Texture.Generate(width, height, NULL);
        glGenTextures(1, &TextureID);
        this->Width = width;
        this->Height = height;
        // create Texture
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        // set Texture wrap and filter modes
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, 0); // attach texture to framebuffer as its color attachment
//     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//         std::cout << "Framebuffer not complete!" << std::endl;
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    this->initRenderData();
}

void Blur::BeginRender() {
    first_iteration = true;
    glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFBO[horizontal]);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Blur::EndRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Blur::Render(unsigned int iters) {

    glEnable(GL_DEPTH_TEST);
    
    glUseProgram(blur_texture_program->program);    
    
    for (unsigned int i = 0; i < iters; i++)
    {
        
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

        glUniform1i(glGetUniformLocation(blur_texture_program->program, "horizontal"), horizontal);

        glBindTexture(GL_TEXTURE_2D, first_iteration ? TextureID : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
        
        // render textured quad
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, TextureID); //this->Texture.Bind();	
        // glBindVertexArray(this->VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render textured quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    
}


void Blur::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    // float vertices[] = {
    //     // pos        // tex
    //     -1.0f, -1.0f, 0.0f, 0.0f,
    //      1.0f,  1.0f, 1.0f, 1.0f,
    //     -1.0f,  1.0f, 0.0f, 1.0f,

    //     -1.0f, -1.0f, 0.0f, 0.0f,
    //      1.0f, -1.0f, 1.0f, 0.0f,
    //      1.0f,  1.0f, 1.0f, 1.0f
    // };
    // glGenVertexArrays(1, &this->VAO);
    // glGenBuffers(1, &VBO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glUseProgram(blur_texture_program->program);
    // glUniform1i(glGetUniformLocation(blur_texture_program->program, "scene"), 0);

    // glBindVertexArray(this->VAO);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}