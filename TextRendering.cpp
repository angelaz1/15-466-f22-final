#include "TextRendering.hpp"



TextRenderer::TextRenderer(std::string font_file, uint32_t font_size) {
    // initialize freetype
    error = FT_Init_FreeType( &ft_library );
    if (error) {
        throw std::runtime_error("FT_Init_FreeType failed");
    }
    error = FT_New_Face (ft_library, font_file.c_str(), 0, &ft_face);
    if (error) {
        throw std::runtime_error("FT_New_Face failed");
    }
    error = FT_Set_Char_Size (ft_face, font_size * 64, font_size * 64, 0, 0);
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

// render text using OpenGL
// heavily based on following tutorials:
// https://learnopengl.com/In-Practice/Text-Rendering
// https://www.freetype.org/freetype2/docs/tutorial/step1.html
// https://github.com/harfbuzz/harfbuzz-tutorial/blob/master/hello-harfbuzz-freetype.c
void TextRenderer::renderLine(std::string line, float x, float y, float scale, glm::vec3 color) {
    // OpenGL state
	// ------------
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); 
	// disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // parse text using Harfbuzz
    // create hb buffer
    hb_buffer = hb_buffer_create();

    hb_buffer_add_utf8(hb_buffer, line.c_str(), -1, 0, -1);
    hb_buffer_guess_segment_properties(hb_buffer);
    hb_shape(hb_font, hb_buffer, NULL, 0);

    /* Get glyph information and positions out of the buffer. */
	info = hb_buffer_get_glyph_infos (hb_buffer, NULL);
	pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);

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
            
            // generate texture and dynamically add to character map
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

    std::pair<float, float> maxWH = std::make_pair(0.0f, 0.0f);

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

