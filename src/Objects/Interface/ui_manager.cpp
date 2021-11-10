#include "src/Objects/Interface/ui_manager.h"

// Special UI item that lays ontop of everything
UIFrame* tooltipUI = new UIFrame();
// Interface layers for the UI manager
std::vector<UILayer*> UIManager::layers = std::vector<UILayer*>();

void UIManager::init() {
    UILayer* baseLayer = new UILayer();
    pushLayer(baseLayer);

    UIFrame* fileButton = new UIFrame();
    fileButton->setPositionScale(glm::vec2(0.0, 0.0), glm::vec2(0.05, 0.025));
    fileButton->interactable = true;
    fileButton->newLabel();
    fileButton->getLabel()->text = "File";
    addInterface(fileButton, 0);
    
    UIFrame* editButton = new UIFrame();
    editButton->setPositionScale(glm::vec2(0.05, 0.0), glm::vec2(0.05, 0.025));
    editButton->interactable = true;
    editButton->newLabel();
    editButton->getLabel()->text = "Edit";
    addInterface(editButton, 0);
}
void UIManager::close() {
    for (int i = 0; i < layers.size(); i++) {
        if (layers[i]) {
            delete layers[i];
        }
    }
    layers.clear();
}


// Appends the given UI layer to the Layer Stack
void UIManager::pushLayer(UILayer* newLayer) {
    layers.push_back(newLayer);
}
// Removes the topmost UI layer from the layer stack
void UIManager::popLayer() {
    UILayer* lastLayer = layers[layers.size() - 1];
    layers.pop_back();
    delete lastLayer;
}
void UIManager::popLayer(UILayer* layer) {
    if (layer == nullptr) return;

    for (auto i = layers.begin(); i < layers.end(); i++) {
        if (*i == layer) {
            layers.erase(i);
            delete layer;
            break;
        }
    }
}


// Get Interfaces - Returns an empty vector array of UI Frames
std::vector<UIFrame*> UIManager::getInterfaces() { return std::vector<UIFrame*>(); }
// Add Interface - Adds the given UI Frame to the topmost layer, based off the given ZIndex
// - If the given ZIndex is -1, it will append it to the end of the array
void UIManager::addInterface(UIFrame* item, int zindex) {
    if (item == nullptr || layers.size() == 0) { return; } // Don't accept nullptrs or if there are no UI Layers

    UILayer* layer = layers[layers.size() - 1]; // Pick top-most layer to edit
    item->zindex = zindex; // Set z-index of item when appending it

    if (layer->numFrames() == 0 || zindex <= -1) { // If there are no items in the list, or no zindex provided, Z-Index does not need to be sorted, put it on top
        if (zindex <= -1) { // Don't allow negative numbers in zindex, set on top of stack
            item->zindex = 1000;
        }
        layer->push(item);
        return;
    }

    std::vector<UIFrame*>::iterator iterator = layer->begin();
    int i = 0;

    int lastZIndex = -1000;
    for (; iterator < layer->end(); iterator++, i++) {
        if (*iterator == nullptr) continue;

        // If we find something that has a higher ZIndex than before, and we are above the last ZIndex, but not above the new one
        if ((*iterator)->zindex > lastZIndex && zindex >= lastZIndex && zindex < (*iterator)->zindex) {
            layer->insert(iterator, item);
            return;
        }
        lastZIndex = (*iterator)->zindex;
    }

    // If we got to the end of the array, just push the new item (Z Index is greater than all currently there)
    layer->push(item);
}

// Clicks the given interface button
void UIManager::clickButton(UIFrame* button, struct IEClick* event) {
    button->click(event);

    // Click event was handled, return it
    free(event);
}
// Sorts through the given interface array, and returns the current button that the mouse cursor is over
UIFrame* UIManager::checkButtons(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    // Get mouse position proportional to screen
    glm::vec2 clickPos = glm::vec2(mouseX / windowWidth, mouseY / windowHeight);

    // Iterate through *all* UI frames, store any frame that is containing the click event
    // If a new clicked frame is a child of the current stored UI frame, replace the stored frame
    // If a new clicked frame is a parent of the given current stored frame, ignore it (need hierchary check function)

    // If a later on a new element is overlapping the current one, and there is no relationship, pick the new one
    // - Being lower in the list currently acts as our Z-Index method

    UIFrame* selected = nullptr;
    for (int j = layers.size() - 1; j >= 0; j--) { // Start from top layer, and move downward
        for (int i = 0; i < layers[j]->numFrames(); i++) {
            UIFrame* frame = layers[j]->getFrame(i);
            if (
                frame == nullptr ||
                !frame->interactable ||
                !frame->containsPosition(clickPos)
            ) { continue; } // Ignore non-existant / non-interactable / non-clicked frames

            if (nullptr == selected) { selected = frame; } // Take first option if one has not been selected yet
            else if (frame->isParentedBy(selected)) { selected = frame; } // New parent
            else if (selected->isParentedBy(frame)) { continue; } // Currently selected frame is parented by us, ignore
            else { selected = frame; } // If no relationship is present, overrite earlier UI frames, as new ones have higher Z Index order
        }
        if (layers[j]->locked()) { // If this layer is locked, then do not move past this layer
            break;
        }
    }

    return selected;
}

// Tick - Ticks UI by the given time in seconds
void UIManager::tick(float delta) {

}