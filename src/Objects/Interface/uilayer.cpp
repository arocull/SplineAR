#include "src/Objects/Interface/uilayer.h"

// Constructor - Size hint tells the vector how much memory to reserve (if > 0)
UILayer::UILayer(int sizeHint) {
    std::vector<UIFrame*> frames = std::vector<UIFrame*>();
    
    if (sizeHint > 0) {
        frames.reserve(sizeHint);
    }
}
UILayer::~UILayer() {
    for (int i = 0; i < frames.size(); i++) {
        if (frames[i]) {
            delete frames[i];
        }
    }

    frames.clear();
}

// Push Frame - Appends a UI Frame to the UI Layer
void UILayer::push(UIFrame* newFrame) {
    frames.push_back(newFrame);
}
void UILayer::insert(std::vector<UIFrame*>::iterator idx, UIFrame* newFrame) {
    frames.insert(idx, newFrame);
}
std::vector<UIFrame*>::iterator UILayer::begin() {
    return frames.begin();
}
std::vector<UIFrame*>::iterator UILayer::end() {
    return frames.end();
}
// Number of Frames - Returns the total number of frames in the UI Layer
int UILayer::numFrames() {
    return frames.size();
}
// Get Frame - Returns the frame at a given index
UIFrame* UILayer::getFrame(int index) {
    return frames[index];
}
UIFrame* UILayer::operator[](int index) {
    return frames[index];
}

// Locked - Returns true if this layer locks the stack
bool UILayer::locked() {
    return lockStack;
}
// Set Lock - If true, the layer "locks" the stack, and mouse and user inputs do not fall through the layer
void UILayer::setLock(bool newLock) {
    lockStack = newLock;
}