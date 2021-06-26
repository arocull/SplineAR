#pragma once

struct Keystroke {
    // Modifiers
    bool shift = false;
    bool control = false;
    bool alt = false;
    bool super = false;

    // User can tap Caps-Lock to enable CAPS mode, allowing a second input layer
    bool capsMode = false;

    // Keys
    int key = 0;
    int scancode = 0;
};