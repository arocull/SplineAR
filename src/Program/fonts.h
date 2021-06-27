#pragma once

#include "src/config.h"

#include <cstdio>
#include <vector>
#include <map>

#include <glm/vec2.hpp>

#include "src/Program/gpu.h"
#include "src/Program/gl_texture_handler.h"

/** Include FreeType Headers
 * This was kinda weird because you may need to tell VSCode they exist by going to `.vscode/c_cpp_properties.json`
 * and adding to "includePath" something like `"/usr/include/freetype2"`
*/
#include <ft2build.h>
#include FT_FREETYPE_H

/** Fonts - Global namespace for handling FreeType
 * TODO: Refactor Fonts system?
 *
 * Built originally using this guide:
 * https://learnopengl.com/In-Practice/Text-Rendering
 * 
 */
namespace Fonts {
    // Types and Structs //
    struct FCharacter {
        GLTextureHandler* texture;
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint advance;
    };
    struct FFont {
        int fontHeight;
        std::map<char, Fonts::FCharacter*> chars;
    };

    // Data //
    extern FT_Library freetype;
    extern std::vector<Fonts::FFont*> fonts;

    // Functions //
    void Initialize();
    void Deallocate();

    FFont* LoadFont(GPU* gpu, const char* sourceFile, int fontHeight);
    void FreeFont(Fonts::FFont* f);
}