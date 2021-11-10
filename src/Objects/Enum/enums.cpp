#include "src/Objects/Enum/enums.h"

// ENUM - Name List - List of names of the given work modes
std::string ENUM_WorkMode_Names[3] = {
    "Drawing Mode",
    "Rigging Mode",
    "Animation Mode",
};
// ENUM - Get Name - Returns the name of the given Work Mode
std::string ENUM_WorkMode_GetName(EWorkMode mode) {
    return ENUM_WorkMode_Names[mode];
}