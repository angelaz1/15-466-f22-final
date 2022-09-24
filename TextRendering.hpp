#pragma once
#include "Mode.hpp"
#include "Scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <deque>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <iostream>
#include <map>
#include <algorithm>

#include "TextTextureProgram.hpp"


#define FONT_SIZE 36
class TextRenderer {
    public: 
        FT_Library ft_library;
        FT_Face ft_face;
        FT_Error error;

        FT_GlyphSlot  slot;
        FT_Matrix     matrix;                 /* transformation matrix */
        FT_Vector     pen;                    /* untransformed origin  */

        hb_font_t *hb_font;
        hb_buffer_t *hb_buffer;

        hb_glyph_info_t *info;
        hb_glyph_position_t *pos;

        /// Holds all state information relevant to a character as loaded using FreeType
        struct Character {
            unsigned int TextureID; // ID handle of the glyph texture
            glm::ivec2   Size;      // Size of glyph
            glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
            unsigned int Advance;   // Horizontal offset to advance to next glyph
        };

        std::map<hb_codepoint_t, Character> Characters;
        unsigned int VAO, VBO;



        TextRenderer(std::string fontFile);
        ~TextRenderer();

        void parseText(std::string text);
        void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

};




