#pragma once

#include <string>

// Work Mode - Current mode a given workspace is in. Defines program functionality.
enum EWorkMode {
    EMDraw,
    EMRig,
    EMAnimate,
    EM_MAX,
};
std::string ENUM_WorkMode_GetName(EWorkMode mode);