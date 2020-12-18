#pragma once

// Work Mode - Current mode a given workspace is in. Defines program functionality.
enum EWorkMode {
    EMDraw = 1,
    EMRig = 2,
    EMAnimate = 3,
};
const char* ENUM_WorkMode_GetName(EWorkMode mode);