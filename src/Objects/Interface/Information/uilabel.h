#pragma once

#include "src/config.h"

#include <GL/gl.h>
#include <cstdlib>
#include <string.h>

#include "src/Objects/Interface/uiframe.h"

// UI Label - A basic, non-functional UI frame with text capabilities
class UILabel : public UIFrame {
    public:
        UILabel(char* textContent);
        ~UILabel();

        void setText(char* textContent);

    private:
        char* text;
        bool hasText;

        void freeText();
        void paint() override;
};