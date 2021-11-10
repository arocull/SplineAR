#include "src/Objects/Workspace/workspace_interface.h"

Workspace* active = nullptr;
std::vector<Workspace*> WInterface::workspaces = std::vector<Workspace*>();

// Sets the active workspace to use
void WInterface::setActive(Workspace* newWorkspace) {
    active = newWorkspace;
}
// Gets the active workspace that's in use
Workspace* WInterface::getActive() {
    return active;
}

// Creates a new, blank workspace with the given name
Workspace* WInterface::buildWorkspace(std::string name) {
    Workspace* newSpace = new Workspace(name);
    workspaces.push_back(newSpace);
    return newSpace;
}
// Finds the given workspace in the workspace array, removes it from the array, and deletes it
void WInterface::closeWorkspace(Workspace* workspace) {
    for (auto iterator = workspaces.begin(); iterator < workspaces.cend(); iterator++) {
        if (*iterator == workspace) {
            workspaces.erase(iterator);
            delete workspace;
            return;
        }
    }
}
// Deallocates and closes all workspaces
void WInterface::close() {
    for (int i = 0; i < workspaces.size(); i++) {
        delete workspaces[i];
    }
    workspaces.clear();
}