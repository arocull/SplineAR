#include "Program/ui_manager.h"

UIManager::UIManager() {
    interfaces = std::vector<UIFrame*>();
    interfaces.reserve(5); // Pre-allocate 5 spaces
}
UIManager::~UIManager() {
    for (int i = 0; i < interfaces.size(); i++) {
        if (interfaces[i]) {
            delete interfaces[i];
            interfaces[i] = nullptr;
        }
    }
    interfaces.clear();


}


// Get Interfaces - Returns a copy of the vector array of UI Frames
std::vector<UIFrame*> UIManager::getInterfaces() { return interfaces; }
// Add Interface - Adds the given UI Frame to the vector array, based off the given ZIndex
// - If the given ZIndex is -1, it will append it to the end of the array
void UIManager::addInterface(UIFrame* item, int zindex) {
    if (item == nullptr) { return; } // Don't accept nullptrs
    if (interfaces.size() == 0 || zindex <= -1) { // If there are no items in the list, or no zindex provided, Z-Index does not need to be sorted
        interfaces.push_back(item);
        return;
    }

    std::vector<UIFrame*>::iterator iterator = interfaces.begin();
    int i = 0;

    int lastZIndex = -1000;
    for (; iterator < interfaces.end(); iterator++, i++) {
        if (interfaces[i] == nullptr) continue;

        // If we find something that has a higher ZIndex than before, and we are above the last ZIndex, but not above the new one
        if (interfaces[i]->zindex > lastZIndex && zindex >= lastZIndex && zindex < interfaces[i]->zindex) {
            interfaces.insert(iterator, item);
            return;
        }
        lastZIndex = interfaces[i]->zindex;
    }

    // If we got to the end of the array, just push the new item (Z Index is greater than all currently there)
    interfaces.push_back(item);
}
// Pushes variables to the front of the array, deletes all nullptrs from the vector array, and calls shrink_to_fit
void UIManager::collapseInterfaces() {
    if (interfaces.size() == 0) { return; }

    // Index of the last nullptr we found in the array
    bool foundNullptr = false;
    int i = 0;
    int lastNullptr = 0;

    // Iterator through all elements
    for (int i = 0; i < interfaces.size(); i++) {
        if (!foundNullptr) {                // If we have not set the last Nullptr index yet...
            if (interfaces[i] == nullptr) { // If we find a nullptr..
                lastNullptr = i;            // Keep track of it
                foundNullptr = true;        // Mark as found
            }
        
        } else {                            // We have set the last nullptr
            if (interfaces[i]) {            // Swap current element to the nullptr position, and
                interfaces[lastNullptr] = interfaces[i];
                interfaces[i] = nullptr;
                lastNullptr++;              // We filled the last nullptr in, move index up
            }
            // Don't iterate lastNullptr if we found a nullptr
        }
    }

    if (foundNullptr) { // If we found a nullptr, remove all nullptr elements, and then shrink memory to fit elements
        interfaces.erase(interfaces.begin() + lastNullptr, interfaces.end());
        interfaces.shrink_to_fit();
    }

    //VECTOR_collapse(interfaces);
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
    for (int i = 0; i < interfaces.size(); i++) {
        if (
            interfaces[i] == nullptr ||
            !interfaces[i]->interactable ||
            !interfaces[i]->containsPosition(clickPos)
        ) { continue; } // Ignore non-existant / non-interactable / non-clicked frames

        if (nullptr == selected) { selected = interfaces[i]; } // Take first oprtion if one has not been selected yet
        else if (interfaces[i]->isParentedBy(selected)) { selected = interfaces[i]; } // New parent
        else if (selected->isParentedBy(interfaces[i])) { continue; } // Currently selected frame is parented by us, ignore
        else { selected = interfaces[i]; } // If no relationship is present, overrite earlier UI frames, as new ones have higher Z Index order
    }

    return selected;
}


void UIManager::tick(float delta) {

}