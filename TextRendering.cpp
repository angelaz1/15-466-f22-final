#include "TextRendering.hpp"



TextRenderer::TextRenderer(std::string fontFile) {
    // initialize freetype
    error = FT_Init_FreeType( &ft_library );
    if (error) {
        throw std::runtime_error("FT_Init_FreeType failed");
    }
    error = FT_New_Face (ft_library, fontFile.c_str(), 0, &ft_face);
    if (error) {
        throw std::runtime_error("FT_New_Face failed");
    }
    error = FT_Set_Char_Size (ft_face, FONT_SIZE * 64, FONT_SIZE * 64, 0, 0);
    if (error) {
        throw std::runtime_error("FT_Set_Char_Size failed");
    }
    // create hb-ft font
    hb_font = hb_ft_font_create(ft_face, NULL);
}

TextRenderer::~TextRenderer() {
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);
}

void TextRenderer::parseText(std::string text) {
    

    // create hb buffer
    hb_buffer = hb_buffer_create();

    hb_buffer_add_utf8(hb_buffer, text.c_str(), -1, 0, -1);
    hb_buffer_guess_segment_properties(hb_buffer);
    hb_shape(hb_font, hb_buffer, NULL, 0);

    /* Get glyph information and positions out of the buffer. */
	unsigned int len = hb_buffer_get_length (hb_buffer);
	info = hb_buffer_get_glyph_infos (hb_buffer, NULL);
	pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);

    for (unsigned int i = 0; i < len; i++)
	{
		hb_codepoint_t gid   = info[i].codepoint;
		unsigned int cluster = info[i].cluster;
		double x_advance = pos[i].x_advance / 64.;
		double y_advance = pos[i].y_advance / 64.;
		double x_offset  = pos[i].x_offset / 64.;
		double y_offset  = pos[i].y_offset / 64.;

		char glyphname[32];
		hb_font_get_glyph_name (hb_font, gid, glyphname, sizeof (glyphname));

		printf  ("glyph='%s'	cluster=%d	advance=(%g,%g)	offset=(%g,%g)\n",
				glyphname, cluster, x_advance, y_advance, x_offset, y_offset);
    }
    printf ("Converted to absolute positions:\n");
    /* And converted to absolute positions. */
    {
        double current_x = 0;
        double current_y = 0;
        for (unsigned int i = 0; i < len; i++)
        {
        hb_codepoint_t gid   = info[i].codepoint;
        unsigned int cluster = info[i].cluster;
        double x_position = current_x + pos[i].x_offset / 64.;
        double y_position = current_y + pos[i].y_offset / 64.;


        char glyphname[32];
        hb_font_get_glyph_name (hb_font, gid, glyphname, sizeof (glyphname));

        printf ("glyph='%s'	cluster=%d	position=(%g,%g)\n",
            glyphname, cluster, x_position, y_position);

        current_x += pos[i].x_advance / 64.;
        current_y += pos[i].y_advance / 64.;
        }
    }
}

// render text using OpenGL
void TextRenderer::renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
    // parse text using Harfbuzz
    parseText(text);

    // set up shader
    glUseProgram(text_texture_program->program);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1280), 0.0f, static_cast<float>(720));
    glUniformMatrix4fv(text_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(projection));
    
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(ft_face, 0, 48);

    // add glyphs to character map, if not present
    uint32_t glyph_len = hb_buffer_get_length (hb_buffer);
    for ( uint32_t i = 0; i < glyph_len; i++)
    {
        // get glyph
        hb_codepoint_t glyph_cp = info[i].codepoint;
        // find glyph in characters map, if not found, load it
        if (Characters.find(glyph_cp) == Characters.end()) {
            // load glyph
            if (FT_Load_Glyph(ft_face, glyph_cp, FT_LOAD_RENDER)) {
                throw std::runtime_error("FREETYTPE: Failed to load Glyph");
            }
            
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                ft_face->glyph->bitmap.width,
                ft_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                ft_face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows),
                glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top),
                static_cast<unsigned int>(ft_face->glyph->advance.x)
            };
            Characters.insert(std::pair<hb_codepoint_t, Character>(glyph_cp, character));
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // -------------------------

    glUniform3f(text_texture_program->TextColor_vec3, color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for ( uint32_t i = 0; i < glyph_len; i++)
    {
        // get glyph and character buffer entry
        hb_codepoint_t glyph_cp = info[i].codepoint;
        Character ch = Characters[glyph_cp];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    hb_buffer_destroy(hb_buffer);

}

