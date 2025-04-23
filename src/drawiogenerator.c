#include "drawiogenerator.h"
#include <stdio.h>
#include "mxml.h"
#include "umlclass.h"
#include <stdlib.h>
#include <string.h> 

#define MIN_CLASS_WIDTH 160
#define MIN_CLASS_HEIGHT 26
#define CELL_HEIGHT 26
#define DIVIDER_HEIGHT 8
#define FONT_SIZE 12 
#define FONT_WIDTH_FACTOR 0.5 // Approximation for Helvetica character width

char* drawio_generateUniqueClassId() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "class%d", class_id_counter++);
    return strdup(buffer);
}

int drawio_classesToFile(struct UMLClass** classes, unsigned int numClasses, char* filePath) {
    if (!classes || numClasses == 0 || !filePath) return 1;

    // Generate base XML structure
    mxml_node_t* xml = NULL;
    if (drawio_generateFileNode(&xml) != 0) {
        fprintf(stderr, "Failed to create XML document\n");
        return 1;
    }

    // Locate the <root> node
    mxml_node_t* root = mxmlFindElement(xml, xml, "root", NULL, NULL, MXML_DESCEND);
    if (!root) {
        fprintf(stderr, "Failed to locate <root> node\n");
        mxmlDelete(xml);
        return 1;
    }
    drawio_generateHeader(root);

    // Positioning variables
    unsigned int x = 40; // Initial x-coordinate
    unsigned int y = 60; // Initial y-coordinate
    unsigned int spacing = 50; // Spacing between class boxes

    // Append each UML class as a node to the <root>
    for (unsigned int i = 0; i < numClasses; ++i) {
        if (drawio_classToXML(classes[i], root, x, y) != 0) {
            fprintf(stderr, "Failed to convert UMLClass[%u] to XML\n", i);
            mxmlDelete(xml);
            return 1;
        }
        x += drawio_calculateWidth(classes[i]) + spacing; // Move to the right for the next class
    }

    // Write to file
    FILE* fp = fopen(filePath, "w");
    if (!fp) {
        perror("fopen");
        mxmlDelete(xml);
        return 1;
    }

    mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
    fclose(fp);
    mxmlDelete(xml);

    return 0;
}

int drawio_generateHeader(mxml_node_t* root)
{
    if (!root) return 1;

    mxml_node_t* cell0 = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(cell0, "id", "0");

    mxml_node_t* cell1 = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(cell1, "id", "1");
    mxmlElementSetAttr(cell1, "parent", "0");

    return 0;
}

int drawio_generateFileNode(mxml_node_t** out) {
    // Create the root XML document node
    mxml_node_t* xml = mxmlNewXML("1.0");

    // Add <mxfile>
    mxml_node_t* mxfile = mxmlNewElement(xml, "mxfile");
    mxmlElementSetAttr(mxfile, "host", "app.diagrams.net");
    mxmlElementSetAttr(mxfile, "agent", "code2uml");
    mxmlElementSetAttr(mxfile, "version", "26.1.0");

    // Add <diagram name="Page-1">
    mxml_node_t* diagram = mxmlNewElement(mxfile, "diagram");
    mxmlElementSetAttr(diagram, "name", "Page-1");

    // Add <mxGraphModel>
    mxml_node_t* model = mxmlNewElement(diagram, "mxGraphModel");
    mxmlElementSetAttr(model, "dx", "753");
    mxmlElementSetAttr(model, "dy", "504");
    mxmlElementSetAttr(model, "grid", "1");
    mxmlElementSetAttr(model, "gridSize", "10");
    mxmlElementSetAttr(model, "guides", "1");
    mxmlElementSetAttr(model, "tooltips", "1");
    mxmlElementSetAttr(model, "connect", "1");
    mxmlElementSetAttr(model, "arrows", "1");
    mxmlElementSetAttr(model, "fold", "1");
    mxmlElementSetAttr(model, "page", "1");
    mxmlElementSetAttr(model, "pageScale", "1");
    mxmlElementSetAttr(model, "pageWidth", "850");
    mxmlElementSetAttr(model, "pageHeight", "1100");

    // Add <root> inside model
    mxml_node_t* root = mxmlNewElement(model, "root");

    *out = xml;
    return 0;
}

unsigned int drawio_calculateWidth(struct UMLClass* class) {
    unsigned int maxLength = strlen(class->name); // Start with the class name length

    // Check attributes
    for (unsigned int i = 0; i < class->attributeCount; ++i) {
        struct UMLAttribute* attribute = &class->attributes[i];
        unsigned int length = strlen(accessModifierToString(attribute->accessModifier)) 
                            + strlen(attribute->name) + strlen(attribute->type) + 3;
        if (length > maxLength) {
            maxLength = length;
        }
    }

    // Check methods
    for (unsigned int i = 0; i < class->methodCount; ++i) {
        struct UMLMethod* method = &class->methods[i];
        // Calculate length of method signature without parameters
        unsigned int length = strlen(accessModifierToString(method->accessModifier)) 
                            + strlen(method->name) + strlen(method->returnType) + 3; // 3 for " " and ": " 
        // Add length for parameters
        for (unsigned int j = 0; j < method->paramCount; ++j) {
            length += strlen(method->paramNames[j]) + strlen(method->paramTypes[j]) + 4; // 3 for ", " and ": "
        }

        if (length > maxLength) {
            maxLength = length;
        }
    }

    // Calculate the width in pixels based on font size and width factor
    unsigned int ret = (unsigned int)(maxLength * FONT_SIZE * FONT_WIDTH_FACTOR);
    return ret < MIN_CLASS_WIDTH ? MIN_CLASS_WIDTH : ret;
}

int drawio_generateClassCell(struct UMLClass* class, mxml_node_t* parent, char* classId, unsigned int width, unsigned int x, unsigned int y) {
    if (!class || !class->name || !parent || !classId) return 1;

    mxml_node_t* classCell = mxmlNewElement(parent, "mxCell");
    mxmlElementSetAttr(classCell, "id", classId);
    mxmlElementSetAttr(classCell, "value", class->name);
    mxmlElementSetAttr(classCell, "style", "swimlane;fontStyle=1;align=center;verticalAlign=top;childLayparent=stackLayparent;horizontal=1;startSize=26;horizontalStack=0;resizeParent=1;resizeParentMax=0;resizeLast=1;collapsible=1;marginBottom=0;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(classCell, "vertex", "1");
    mxmlElementSetAttr(classCell, "parent", "1");

    mxml_node_t* geo = mxmlNewElement(classCell, "mxGeometry");
    char xStr[16];
    snprintf(xStr, sizeof(xStr), "%u", x);
    mxmlElementSetAttr(geo, "x", xStr);
    char yStr[16];
    snprintf(yStr, sizeof(yStr), "%u", y);
    mxmlElementSetAttr(geo, "y", yStr);
    char widthStr[16];
    snprintf(widthStr, sizeof(widthStr), "%u", width);
    mxmlElementSetAttr(geo, "width", widthStr);
    unsigned int classHeight = 26 + (class->attributeCount + class->methodCount) * CELL_HEIGHT + DIVIDER_HEIGHT;
    classHeight = classHeight < MIN_CLASS_HEIGHT ? MIN_CLASS_HEIGHT : classHeight;
    char classHeightStr[64];
    snprintf(classHeightStr, sizeof(classHeightStr), "%u", classHeight);
    mxmlElementSetAttr(geo, "height", classHeightStr);
    mxmlElementSetAttr(geo, "as", "geometry");

    return 0;
}

int drawio_addAttributes(struct UMLClass* class, mxml_node_t* parent, char* classId, unsigned int* y, unsigned int width) {
    if (!class || !parent || !classId || !y) return 1;

    for (unsigned int i = 0; i < class->attributeCount; ++i) {
        struct UMLAttribute* attribute = &class->attributes[i];

        mxml_node_t* field = mxmlNewElement(parent, "mxCell");
        char fieldId[64];
        snprintf(fieldId, sizeof(fieldId), "%s-field%d", classId, i + 1);
        mxmlElementSetAttr(field, "id", fieldId);

        char fieldValue[256];
        char beginTextDecorator[16] = "";
        char endTextDecorator[16] = "";
        // If static set the underline decorator
        if (attribute->isStatic) {
            snprintf(beginTextDecorator, sizeof(beginTextDecorator), "<u>");
            snprintf(endTextDecorator, sizeof(endTextDecorator), "</u>");
        }
        snprintf(fieldValue, sizeof(fieldValue), "%s%s %s: %s%s", 
                beginTextDecorator, accessModifierToString(attribute->accessModifier), 
                attribute->name, attribute->type, endTextDecorator);

        mxmlElementSetAttr(field, "value", fieldValue);
        mxmlElementSetAttr(field, "style", "text;strokeColor=none;fillColor=none;align=left;verticalAlign=top;spacingLeft=4;spacingRight=4;overflow=hidden;rotatable=0;points=[[0,0.5],[1,0.5]];portConstraint=eastwest;whiteSpace=wrap;html=1;");
        mxmlElementSetAttr(field, "vertex", "1");
        mxmlElementSetAttr(field, "parent", classId);

        mxml_node_t* fieldGeo = mxmlNewElement(field, "mxGeometry");
        char yStr[16];
        snprintf(yStr, sizeof(yStr), "%u", *y);
        mxmlElementSetAttr(fieldGeo, "y", yStr);
        char widthStr[16];
        snprintf(widthStr, sizeof(widthStr), "%u", width);
        mxmlElementSetAttr(fieldGeo, "width", widthStr);
        mxmlElementSetAttr(fieldGeo, "height", "26");
        mxmlElementSetAttr(fieldGeo, "as", "geometry");

        *y += CELL_HEIGHT; // Increment y for the next attribute
    }

    return 0;
}

int drawio_addDivider(mxml_node_t* parent, char* classId, unsigned int* y, unsigned int width) {
    if (!parent || !classId || !y) return 1;

    mxml_node_t* divider = mxmlNewElement(parent, "mxCell");
    char dividerId[64];
    snprintf(dividerId, sizeof(dividerId), "%s-divider", classId);
    mxmlElementSetAttr(divider, "id", dividerId);
    mxmlElementSetAttr(divider, "style", "line;strokeWidth=1;fillColor=none;align=left;verticalAlign=middle;spacingTop=-1;spacingLeft=3;spacingRight=3;rotatable=0;labelPosition=right;points=[];portConstraint=eastwest;strokeColor=inherit;");
    mxmlElementSetAttr(divider, "vertex", "1");
    mxmlElementSetAttr(divider, "parent", classId);

    mxml_node_t* dividerGeo = mxmlNewElement(divider, "mxGeometry");

    char dividerYStr[16];
    snprintf(dividerYStr, sizeof(dividerYStr), "%u", *y);
    mxmlElementSetAttr(dividerGeo, "y", dividerYStr);

    char dividerWidthStr[16];
    snprintf(dividerWidthStr, sizeof(dividerWidthStr), "%u", width);
    mxmlElementSetAttr(dividerGeo, "width", dividerWidthStr);

    mxmlElementSetAttr(dividerGeo, "height", "8");
    mxmlElementSetAttr(dividerGeo, "as", "geometry");
    *y += DIVIDER_HEIGHT; // Increment y for the divider

    return 0;
}

int drawio_addMethods(struct UMLClass* class, mxml_node_t* parent, char* classId, unsigned int* y, unsigned int width) {
    if (!class || !parent || !classId || !y) return 1;

    for (unsigned int i = 0; i < class->methodCount; ++i) {
        struct UMLMethod* method = &class->methods[i];
        mxml_node_t* methodNode = mxmlNewElement(parent, "mxCell");
        char methodId[64];
        snprintf(methodId, sizeof(methodId), "%s-method%d", classId, i + 1);
        mxmlElementSetAttr(methodNode, "id", methodId);
        char methodValue[256] = "";
        char methodParams[64] = "";
        for(int i = 0; i < method->paramCount; ++i) {
            if (i > 0) {
                strcat(methodParams, ", ");
            }
            strcat(methodParams, method->paramNames[i]);
            strcat(methodParams, ": ");
            strcat(methodParams, method->paramTypes[i]);
        }
        if (method->paramCount == 0) {
            methodParams[0] = '\0';
        }

        // If static set the underline decorator
        char beginTextDecorator[16] = "";
        char endTextDecorator[16] = "";
        if (method->isStatic) {
            snprintf(beginTextDecorator, sizeof(beginTextDecorator), "<u>");
            snprintf(endTextDecorator, sizeof(endTextDecorator), "</u>");
        }

        snprintf(methodValue, sizeof(methodValue), "%s%s %s(%s): %s%s", 
                beginTextDecorator, accessModifierToString(method->accessModifier), method->name, 
                methodParams, method->returnType, endTextDecorator);
                
        mxmlElementSetAttr(methodNode, "value", methodValue);
        mxmlElementSetAttr(methodNode, "style", "text;strokeColor=none;fillColor=none;align=left;verticalAlign=top;spacingLeft=4;spacingRight=4;overflow=hidden;rotatable=0;points=[[0,0.5],[1,0.5]];portConstraint=eastwest;whiteSpace=wrap;html=1;");
        mxmlElementSetAttr(methodNode, "vertex", "1");
        mxmlElementSetAttr(methodNode, "parent", classId);

        mxml_node_t* methodGeo = mxmlNewElement(methodNode, "mxGeometry");
        char methodYStr[16];
        snprintf(methodYStr, sizeof(methodYStr), "%u", *y);
        mxmlElementSetAttr(methodGeo, "y", methodYStr);
        char widthStr[16];
        snprintf(widthStr, sizeof(widthStr), "%u", width);
        mxmlElementSetAttr(methodGeo, "width", widthStr);
        mxmlElementSetAttr(methodGeo, "height", "26");
        mxmlElementSetAttr(methodGeo, "as", "geometry");

        *y += CELL_HEIGHT; // Increment y for the next method
    }

    return 0;
}

int drawio_classToXML(struct UMLClass* class, mxml_node_t* parent, unsigned int x, unsigned int y) {
    if (!class || !class->name || !parent)  {
        return 1;
    }

    // Generate a unique ID for the class
    char* classId = drawio_generateUniqueClassId();
    if (!classId){
        return 1;
    }
    unsigned int width = drawio_calculateWidth(class);

    if (drawio_generateClassCell(class, parent, classId, width, x, y) != 0) {
        free(classId);
        return 1;
    }

    unsigned int currentY = CELL_HEIGHT; // Start below the class header

    if (drawio_addAttributes(class, parent, classId, &currentY, width) != 0 ||
        drawio_addDivider(parent, classId, &currentY, width) != 0 ||
        drawio_addMethods(class, parent, classId, &currentY, width) != 0) {
        free(classId);
        return 1;
    }

    free(classId);
    return 0;
}
