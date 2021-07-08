#pragma once

#include "src/config.h"

#include <vector>

#include <glm/vec2.hpp>
#include "Util/gl_headers.h"
#include "Util/vector.h"

#include "src/Program/input_manager.h"
#include "src/Objects/Input/click.h"
#include "src/Objects/Interface/uiframe.h"

class UIManager {
    public:
        UIManager();
        ~UIManager();
    
    private:
        std::vector<UIFrame*> interfaces;
        UIFrame* tooltipUI; // Special UI item that lays ontop of everything

    public:
        std::vector<UIFrame*> getInterfaces();
        void addInterface(UIFrame* item, int zindex = -1);
        void collapseInterfaces();

        void clickButton(UIFrame* button, struct IEClick* event);
        UIFrame* checkButtons(double mouseX, double mouseY, int windowWidth, int windowHeight);

        void tick(float delta);
};