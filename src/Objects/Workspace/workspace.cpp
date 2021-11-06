#include "src/Objects/Workspace/workspace.h"

Workspace::Workspace(const char* workspaceName) {
    name = strdup(workspaceName); // Allocate new string for workspace name
    mode = EWorkMode::EMDraw;

    frameCurrent = frameStart;

    brushes = (Brush**) calloc(NUM_WORK_MODES, sizeof(Brush*));
    for (int i = 0; i < NUM_WORK_MODES; i++) {
        brushes[i] = new Brush();
    }

    strokes = (Stroke**) calloc(MAX_STROKES, sizeof(Stroke*));
    rigging = (Stroke**) calloc(MAX_STROKES, sizeof(Stroke*));
    for (int i = 0; i < MAX_STROKES; i++) {
        strokes[i] = nullptr;
        rigging[i] = nullptr;
    }
}
Workspace::~Workspace() {
    free(name); // Free workspace name

    // Free brushes
    for (int i = 0; i < NUM_WORK_MODES; i++) {
        if (brushes[i]) delete brushes[i];
    }
    free(brushes);

    // Free stroke data
    for (int i = 0; i < MAX_STROKES; i++) {
        if (strokes[i]) delete strokes[i];
        if (rigging[i]) delete rigging[i];
    }
    free(rigging);
    free(strokes);
}


/* GETTERS */

// Workspace - Get Name - Returns the string name of this workspace
char* Workspace::getName() {
    return name;
}
// Workspace - Get Mode - Returns the current edit mode this workspace is in
EWorkMode Workspace::getMode() {
    return mode;
}
// Workspace - Get Brush - Returns the brush that is currently in use
Brush* Workspace::getBrush() {
    return brushes[mode];
}
// Workspace - Get Brush for Work Mode - Returns the brush that is used by the given Work Mode
Brush* Workspace::getBrush(EWorkMode workModeToFetch) {
    return brushes[workModeToFetch];
}
// Workspace - Get Active Stroke - Returns the stroke that is currently being drawn to
Stroke* Workspace::getActiveStroke() {
    return getBrush()->getActiveStroke();
}
// Workspace - Get Stroke Array - Returns an array of stroke pointers based off of the requested mode
Stroke** Workspace::getStrokeArray(EWorkMode workMode) {
    switch (workMode) {
        case EWorkMode::EMRig:
            return rigging;
        default:
            return strokes;
    }
}


/* USER INPUT */

// Workspace - Set Mode - Switches the workspace to the new work mode
void Workspace::setMode(EWorkMode newWorkMode) {
    mode = newWorkMode;
}

// Workspace - Apply Input - Applies a keystroke to the workspace, if possible. Returns false if nothing happened
bool Workspace::applyInput(Keystroke* input) {
    if (input->modifiers == 0 && !input->capsMode) {
        bool modeChanged;
        switch (input->key) {
            case '1':
                setMode(EWorkMode::EMDraw);
                modeChanged = true;
                break;
            case '2':
                setMode(EWorkMode::EMRig);
                modeChanged = true;
                break;
            case '3':
                setMode(EWorkMode::EMAnimate);
                modeChanged = true;
                break;
            default:
                modeChanged = false;
        }
        return modeChanged;
    }
    return false;
}



/* ANIMATION */

// Workspace - Get Time - Returns current time
float Workspace::getTime() {
    return time;
}
// Workspace - Tick - Adds DeltaTime to the Workspace timer, and plays the given animation
void Workspace::tick(float DeltaTime) {
    if (animationPlaying) {
        time += DeltaTime * timeScale;

        frameCurrent = floor(time * FramesPerSecond) + frameStart; // Convert float time to current frame
        if (frameCurrent > frameEnd) frameCurrent = frameEnd;
    } else {
        time += DeltaTime;
    }
}
// Workspace - Play Animation - Plays the current animation from the current viewed frame
void Workspace::playAnimation() {
    playAnimation(frameCurrent);
}
// Workspace - Play Animation from Given Start Frame - Plays the current animation
void Workspace::playAnimation(int animationStartFrame) {
    time = (float) animationStartFrame / FramesPerSecond; // Convert current frame into a timestamp for playing
    frameCurrent = animationStartFrame;

    animationPlaying = true;
}
// Workspace - Stop Animation - Stops playing the current animation
void Workspace::stopAnimation() {
    animationPlaying = false;
}



/* DRAWING */

// Workspace - Add Stroke - Adds the given stroke to an array of strokes based off of the current work mode
bool Workspace::addStroke(Stroke* newStroke) {
    bool placed = false; // If true, the stroke was placed into the right array

    Stroke** array = getStrokeArray(mode); // Array to add the stroke to (based off of mode)

    for (int i = 0; i < MAX_STROKES && !placed; i++) {
        if (!strokes[i]) {
            strokes[i] = newStroke;
            placed = true;
        }
    }

    return placed;
}