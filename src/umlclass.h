#ifndef UMLCLASS_H
#define UMLCLASS_H

typedef enum {
    PUBLIC,
    PRIVATE,
    PROTECTED
} AccessModifier;

/**
 * @brief Converts an AccessModifier enum to its string representation.
 * 
 * @param modifier The AccessModifier enum value.
 * @return A string representing the access modifier.
 */
const char* accessModifierToString(AccessModifier modifier);

struct UMLAttribute {
    char* name;          // Attribute name
    char* type;          // Attribute type
    char* defaultValue;  // Default value (if applicable)
    int isStatic;        // 1 if static, 0 otherwise
    AccessModifier accessModifier; // Access modifier (public/private/protected)
};

struct UMLMethod {
    char* name;          // Method name
    char* returnType;    // Return type
    char** paramNames;   // Array of parameter names
    char** paramTypes;   // Array of parameter types
    unsigned int paramCount;      // Number of parameters
    AccessModifier accessModifier; // Access modifier (public/private/protected)
    int isStatic;        // 1 if static, 0 otherwise
};

struct UMLClass {
    char* name;                // Class name
    struct UMLAttribute* attributes; // Array of attributes
    int attributeCount;        // Number of attributes
    struct UMLMethod* methods; // Array of methods
    int methodCount;           // Number of methods
};

#endif // UMLCLASS_H