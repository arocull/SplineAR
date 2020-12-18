#pragma once

#include "src/config.h"
#include "src/Objects/Enum/enums.h"

#include <cstdlib>
#include <cstdio>
#include <string.h>

#include "src/Objects/Tools/brush.h"
#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"

// Workspace - Used for organizing stroke data, work mode, animation timing, and more
// - This is essentially a loaded-up project file
class Workspace {
    public:
        Workspace(const char* workspaceName);
        ~Workspace();

    protected:
        char* name;
        EWorkMode mode;

        float time = 0.0f;
        float timeScale = 1.0f;
        int frameStart = 0;
        int frameEnd = 250;
        int frameCurrent;
        int FramesPerSecond = 60;
        bool animationPlaying = false;

        Brush** brushes;

        Stroke** strokes;
        Stroke** rigging;

    public:
        char* getName();
        EWorkMode getMode();
        float getTime();

        Brush* getBrush();
        Brush* getBrush(EWorkMode workModeToFetch);
        Stroke* getActiveStroke();

        Stroke** getStrokeArray(EWorkMode workModeToFetch = EWorkMode::EMDraw);


        void tick(float DeltaTime);
        void playAnimation();
        void playAnimation(int animationStartFrame);
        void stopAnimation();


        bool addStroke(Stroke* newStroke);
};