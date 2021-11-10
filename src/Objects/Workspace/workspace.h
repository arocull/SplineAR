#pragma once

#include "src/config.h"
#include "src/Objects/Enum/enums.h"

#include <cstdlib>
#include <cstdio>
#include <string>

#include "src/Objects/Tools/brush.h"
#include "src/Objects/Art/point.h"
#include "src/Objects/Art/stroke.h"

// Workspace - Used for organizing stroke data, work mode, animation timing, and more
// - This is essentially a loaded-up project file
class Workspace {
    public:
        Workspace(std::string workspaceName);
        ~Workspace();

    protected:
        std::string name;
        EWorkMode mode;

        bool changed = false;
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
        std::string getName();
        EWorkMode getMode();
        float getTime();
        bool hasChanged();

        Brush* getBrush();
        Brush* getBrush(EWorkMode workModeToFetch);
        Stroke* getActiveStroke();

        Stroke** getStrokeArray(EWorkMode workModeToFetch = EWorkMode::EMDraw);

        void setMode(EWorkMode newWorkMode);

        void tick(float DeltaTime);
        void playAnimation();
        void playAnimation(int animationStartFrame);
        void stopAnimation();


        bool addStroke(Stroke* newStroke);
};