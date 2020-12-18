#pragma once

// Work Mode - Current mode a given workspace is in. Defines program functionality.
enum EWorkMode {
    EMDraw = 0,
    EMRig = 1,
    EMAnimate = 2,
};
const char* ENUM_WorkMode_GetName(EWorkMode mode);