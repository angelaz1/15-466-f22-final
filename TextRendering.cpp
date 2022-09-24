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

    // create hb buffer
    hb_buffer = hb_buffer_create();
}

TextRenderer::~TextRenderer() {
    hb_buffer_destroy(hb_buffer);
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);
}

void TextRenderer::parseText(std::string text) {
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