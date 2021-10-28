#include "src/Objects/Interface/Information/uilabel.h"

UILabel::UILabel(char* textContent) {
    hasText = false;
    
}
UILabel::~UILabel() {
    freeText();
}


void UILabel::setText(char* textContent) {
    if (hasText) { // Free existing text before replacing it
        freeText();
    }

    text = strdup(textContent);
    hasText = true;
}
void UILabel::freeText() {
    if (hasText) {
        free(text);
        hasText = false;
    }
}
void UILabel::paint() {
    UIFrame::paint();
}