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


// Get Interfaces - Returns the vector array of UI Frames
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
// Removes all nullptrs from the vector array and calls shrink_to_fit
void UIManager::collapseInterfaces() {
    /* if (interfaces.size() == 0) { return; }

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
    } */

    VECTOR_collapse(interfaces);
}


void UIManager::tick(float delta) {

}