/*******************************************************************
** This code is part of Breakout.
** Taken from https://learnopengl.com/In-Practice/2D-Game/Postprocessing
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_errors.hpp"
#include "PostProcessingTextureProgram.hpp"

// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor
{
public:
    // state
    unsigned int Width, Height;
    unsigned int TextureID;
    // options
    bool Shake = false;
    // constructor
    PostProcessor(unsigned int width, unsigned int height);
    // prepares the postprocessor's framebuffer operations before rendering the game
    void BeginRender();
    // should be called after rendering the game, so it stores all the rendered data into a texture object
    void EndRender();
    // renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void Render(float time);
private:
    // render state
    unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    unsigned int RBO; // RBO is used for multisampled color buffer
    unsigned int VAO;
    // initialize quad for rendering postprocessing texture
    void initRenderData();
};