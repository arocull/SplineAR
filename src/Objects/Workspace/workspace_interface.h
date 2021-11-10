#pragma once

#include <vector>
#include <string>

#include "src/Program/ui_manager.h"
#include "src/Objects/Workspace/workspace.h"
#include "src/Objects/Input/keystroke.h"
#include "src/Program/window.h"

// Workspace Interface - Basic manager for workspaces, allowing easier data management
namespace WInterface {
    // Active workspace that is being worked in
    extern std::vector<Workspace*> workspaces;
    
    void setActive(Workspace* workspace);
    Workspace* getActive();
    
    Workspace* buildWorkspace(std::string name);
    void closeWorkspace(Workspace* workspace);
    void close();

    // WORKSPACE INTERFACING //
    void updateWindowTitle(PWindow* window);
    void applyInput(Keystroke* input);
}