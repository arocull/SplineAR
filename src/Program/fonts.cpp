#include "src/Program/fonts.h"

FT_Library Fonts::freetype;
std::vector<Fonts::FFont*> Fonts::fonts;

void Fonts::Initialize() {
    if (FT_Init_FreeType(&freetype)) {
        perror("Error: Could not initialize FreeType Library\n");
    }

    fonts = {};
}
void Fonts::Deallocate() {
    FT_Done_FreeType(freetype);

    for (int i = 0; i < fonts.size(); i++) {
        if (fonts[i]) {
            FreeFont(fonts[i]);
            delete fonts[i];
        }
    }

    fonts.clear();
}

Fonts::FFont* Fonts::LoadFont(GPU* gpu, const char* sourceFile, int fontHeight) {
    FT_Face face;
    Fonts::FFont* f = new FFont();
    
    if (FT_New_Face(freetype, sourceFile, 0, &(face))) {
        perror("Error: Could not load font file\n");
        printf("\t...from source %s\n", sourceFile);
        delete f;
        return nullptr;
    }

    FT_Set_Pixel_Sizes(face, 0, fontHeight);
    f->fontHeight = fontHeight;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Tell OpenGL we only need to pack into one color

    for (unsigned char c = 0; c < 128; c++) { // We only really need to support ASCII characters, right?
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            perror("Error: FONT failed to load glyph\n");
            printf("...Glyph %c from font source %s\n", c, sourceFile);
            continue;
        }

        FT_GlyphSlot glyph = face->glyph;

        Fonts::FCharacter* character = new Fonts::FCharacter();
        character->size = glm::ivec2(glyph->bitmap.width, glyph->bitmap.rows);
        character->bearing = glm::ivec2(glyph->bitmap_left, glyph->bitmap_top);
        character->advance = glyph->advance.x;
        character->texture = new GLTextureHandler(gpu, character->size.x, character->size.y,
            GL_UNSIGNED_BYTE, GL_RED, GL_RED, GL_LINEAR, glyph->bitmap.buffer);
        
        f->chars.insert(std::pair<char, Fonts::FCharacter*>(c, character));
    }
    FT_Done_Face(face);
    fonts.push_back(f);

    return f;
}
void Fonts::FreeFont(Fonts::FFont* f) {
    for (unsigned char c = 0; c < 128; c++) {
        if (f->chars.at(c)) {
            delete f->chars.at(c)->texture;
            delete f->chars.at(c);
        }
    }

    f->chars.clear();
}