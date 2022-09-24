#pragma once
#include "Mode.hpp"
#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <iostream>
#include <map>
#include <algorithm>

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

        TextRenderer(std::string fontFile);
        ~TextRenderer();

        void parseText(std::string text);

};




