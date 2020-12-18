#pragma once

#include "src/config.h"
#include "src/Objects/Enum/enums.h"

#include <cstdlib>
#include <cstdio>
#include <string.h>

class Workspace {
    public:
        Workspace(const char* workspaceName);
        ~Workspace();

    protected:
        char* name;
        EWorkMode mode;

    public:
        char* getName();
        EWorkMode getMode();
};