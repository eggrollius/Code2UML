#include "umlclass.h"

const char* accessModifierToString(AccessModifier modifier)
{
    switch (modifier) {
        case PUBLIC: return "+";
        case PRIVATE: return "-";
        case PROTECTED: return "#";
        default: return "?";
    }
}