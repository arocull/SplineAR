#include "src/Objects/Workspace/workspace.h"

Workspace::Workspace(const char* workspaceName) {
    name = strdup(workspaceName); // Allocate new string for workspace name

    mode = EWorkMode::EMDraw;
}
Workspace::~Workspace() {
    free(name); // Free workspace name
}

char* Workspace::getName() {
    return name;
}
EWorkMode Workspace::getMode() {
    return mode;
}