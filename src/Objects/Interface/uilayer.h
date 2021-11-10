#pragma once

#include <vector>
#include "src/Objects/Interface/uiframe.h"

// UI Layer - A layer for holding a group of UI Frames, for better organization
class UILayer {
    public:
        UILayer(int sizeHint = 0);
        ~UILayer();
    
    private:
        std::vector<UIFrame*> frames;
        bool lockStack = false;

    public:
        void push(UIFrame* newFrame);
        void insert(std::vector<UIFrame*>::iterator idx, UIFrame* newFrame);
        std::vector<UIFrame*>::iterator begin();
        std::vector<UIFrame*>::iterator end();
        
        int numFrames();
        UIFrame* getFrame(int index);
        UIFrame* operator[](int index);

        bool locked();
        void setLock(bool newLock);
};