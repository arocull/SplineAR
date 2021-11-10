#pragma once

#include <string>

// Work Mode - Current mode a given workspace is in. Defines program functionality.
enum EWorkMode {
    EMDraw = 0,
    EMRig = 1,
    EMAnimate = 2,
};
std::string ENUM_WorkMode_GetName(EWorkMode mode);