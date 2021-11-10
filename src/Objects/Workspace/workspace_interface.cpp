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


// WORKSPACE INTERACTION

void WInterface::updateWindowTitle(PWindow* window){
    window->UpdateTitle(active->getName(), active->getMode(), active->hasChanged());
}

// Apply Input - Applies a keystroke to the workspace, if possible. Returns false if nothing happened
void WInterface::applyInput(Keystroke* input) {
    if (input->modifiers == 0 && !input->capsMode) {
        int mode = input->key - '1'; // Make input key relative to workspace mode

        // Make sure mode is a real mode
        if (mode >= EWorkMode::EMDraw && mode < EWorkMode::EM_MAX) {
            active->setMode((EWorkMode) mode); // Swap work mode
        }
    }
}