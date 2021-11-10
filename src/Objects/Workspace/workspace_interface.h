#pragma once

#include <vector>
#include <string>

#include "src/Program/ui_manager.h"
#include "src/Objects/Workspace/workspace.h"

// Workspace Interface - Basic manager for workspaces, allowing easier data management
namespace WInterface {
    // Active workspace that is being worked in
    extern std::vector<Workspace*> workspaces;
    
    void setActive(Workspace* workspace);
    Workspace* getActive();
    
    Workspace* buildWorkspace(std::string name);
    void closeWorkspace(Workspace* workspace);
    void close();
}