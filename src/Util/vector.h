#pragma once

#include <vector>

template <typename T> void VECTOR_collapse(std::vector<T> vect) {
    if (vect.size() == 0) { return; }

    // Index of the last nullptr we found in the array
    bool foundNullptr = false;
    int i = 0;
    int lastNullptr = 0;

    // Iterator through all elements
    for (int i = 0; i < vect.size(); i++) {
        if (!foundNullptr) {                // If we have not set the last Nullptr index yet...
            if (vect[i] == nullptr) { // If we find a nullptr..
                lastNullptr = i;            // Keep track of it
                foundNullptr = true;        // Mark as found
            }
        
        } else {                            // We have set the last nullptr
            if (vect[i]) {            // Swap current element to the nullptr position, and
                vect[lastNullptr] = vect[i];
                vect[i] = nullptr;
                lastNullptr++;              // We filled the last nullptr in, move index up
            }
            // Don't iterate lastNullptr if we found a nullptr
        }
    }

    if (foundNullptr) { // If we found a nullptr, remove all nullptr elements, and then shrink memory to fit elements
        vect.erase(vect.begin() + lastNullptr, vect.end());
        vect.shrink_to_fit();
    }
}