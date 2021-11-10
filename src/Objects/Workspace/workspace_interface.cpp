#include "src/Objects/Workspace/workspace_interface.h"

Workspace* active = nullptr;
UILayer* hud = nullptr;
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

void WInterface::workspaceToolUI(EWorkMode mode) {
    UIManager::popLayer(hud);

    hud = new UILayer();
    if (mode == EWorkMode::EMDraw) {
        UIFrame* propPanel = new UIFrame();
        propPanel->setPositionScale(glm::vec2(0.9, 0.0), glm::vec2(0.1, 0.05));
        propPanel->interactable = false;
        propPanel->newLabel();
        propPanel->getLabel()->text = "Brush Properties";
        hud->push(propPanel);
    } else if (mode == EWorkMode::EMRig) {
        UIFrame* propPanel = new UIFrame();
        propPanel->setPositionScale(glm::vec2(0.9, 0.0), glm::vec2(0.1, 0.05));
        propPanel->interactable = false;
        propPanel->newLabel();
        propPanel->getLabel()->text = "Brush Properties";
        hud->push(propPanel);

        UIFrame* bonePanel = new UIFrame();
        bonePanel->setPositionScale(glm::vec2(0.9, 0.05), glm::vec2(0.1, 0.05));
        bonePanel->interactable = false;
        bonePanel->newLabel();
        bonePanel->getLabel()->text = "Bone Properties";
        hud->push(bonePanel);
    } else if (mode == EWorkMode::EMAnimate) {
        UIFrame* propPanel = new UIFrame();
        propPanel->setPositionScale(glm::vec2(0.9, 0.0), glm::vec2(0.1, 0.05));
        propPanel->interactable = false;
        propPanel->newLabel();
        propPanel->getLabel()->text = "Rig Properties";
        hud->push(propPanel);
    }

    UIManager::pushLayer(hud);
}

// Apply Input - Applies a keystroke to the workspace, if possible. Returns false if nothing happened
void WInterface::applyInput(Keystroke* input) {
    if (input->modifiers == 0 && !input->capsMode) {
        int mode = input->key - '1'; // Make input key relative to workspace mode

        // Make sure mode is a real mode
        if (mode >= EWorkMode::EMDraw && mode < EWorkMode::EM_MAX) {
            active->setMode((EWorkMode) mode); // Swap work mode
            workspaceToolUI(active->getMode());
        }
    }
}