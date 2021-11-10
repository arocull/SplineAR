#pragma once

#include "src/config.h"

#include <vector>

#include <glm/vec2.hpp>
#include "src/Util/gl_headers.h"
#include "src/Util/vector.h"

#include "src/Program/input_manager.h"
#include "src/Objects/Input/click.h"
#include "src/Objects/Interface/uiframe.h"
#include "src/Objects/Interface/uilayer.h"

namespace UIManager {
    extern std::vector<UILayer*> layers;

    void init();
    void close();

    void pushLayer(UILayer* newLayer);
    void popLayer();
    void popLayer(UILayer* layer);
    
    std::vector<UIFrame*> getInterfaces();
    void addInterface(UIFrame* item, int zindex = -1);

    void clickButton(UIFrame* button, struct IEClick* event);
    UIFrame* checkButtons(double mouseX, double mouseY, int windowWidth, int windowHeight);

    void tick(float delta);
};