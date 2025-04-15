#ifndef UMLCLASS_H
#define UMLCLASS_H

struct UMLAttribute {
    char* name;          // Attribute name
    char* type;          // Attribute type
    char* defaultValue;  // Default value (if applicable)
    int isStatic;        // 1 if static, 0 otherwise
    char* accessModifier; // Access modifier (public/private/protected)
};

struct UMLMethod {
    char* name;          // Method name
    char* returnType;    // Return type
    char** paramNames;   // Array of parameter names
    char** paramTypes;   // Array of parameter types
    int paramCount;      // Number of parameters
    char* accessModifier; // Access modifier (public/private/protected)
};

struct UMLClass {
    char* name;                // Class name
    struct UMLAttribute* attributes; // Array of attributes
    int attributeCount;        // Number of attributes
    struct UMLMethod* methods; // Array of methods
    int methodCount;           // Number of methods
};

#endif // UMLCLASS_H