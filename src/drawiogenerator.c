#include "drawiogenerator.h"
#include <stdio.h>
#include "mxml.h"
#include "umlclass.h"

char* drawio_generateUniqueClassId() {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "class%d", class_id_counter++);
    return strdup(buffer);
}

int drawio_classesToFile(struct UMLClass** classes, unsigned int numClasses, char* filePath)
{
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
    // Append each UML class as a node to the <root>
    for (unsigned int i = 0; i < numClasses; ++i) {
        if (drawio_classToXML(classes[i], root) != 0) {
            fprintf(stderr, "Failed to convert UMLClass[%u] to XML\n", i);
            mxmlDelete(xml);
            return 1;
        }
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

int drawio_classToXML(struct UMLClass* class, mxml_node_t* parent)
{
    if (!class || !class->name || !parent) return 1;

    // Generate the unique class Id
    char* classId = drawio_generateUniqueClassId();

    // Base class cell
    mxml_node_t* classCell = mxmlNewElement(parent, "mxCell");
    mxmlElementSetAttr(classCell, "id", classId);
    mxmlElementSetAttr(classCell, "value", class->name);
    mxmlElementSetAttr(classCell, "style", "swimlane;fontStyle=1;align=center;verticalAlign=top;childLayparent=stackLayparent;horizontal=1;startSize=26;horizontalStack=0;resizeParent=1;resizeParentMax=0;resizeLast=0;collapsible=1;marginBottom=0;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(classCell, "vertex", "1");
    mxmlElementSetAttr(classCell, "parent", "1");

    mxml_node_t* geo = mxmlNewElement(classCell, "mxGeometry");
    mxmlElementSetAttr(geo, "x", "140");
    mxmlElementSetAttr(geo, "y", "70");
    mxmlElementSetAttr(geo, "width", "160");
    mxmlElementSetAttr(geo, "height", "86");
    mxmlElementSetAttr(geo, "as", "geometry");

    // Example field
    mxml_node_t* field = mxmlNewElement(parent, "mxCell");
    char fieldId[64];
    snprintf(fieldId, sizeof(fieldId), "%s-field1", classId);   
    mxmlElementSetAttr(field, "id", fieldId);
    mxmlElementSetAttr(field, "value", "+ field: type");
    mxmlElementSetAttr(field, "style", "text;strokeColor=none;fillColor=none;align=left;verticalAlign=top;spacingLeft=4;spacingRight=4;overflow=hidden;rotatable=0;points=[[0,0.5],[1,0.5]];portConstraint=eastwest;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(field, "vertex", "1");
    mxmlElementSetAttr(field, "parent", classId);

    mxml_node_t* fieldGeo = mxmlNewElement(field, "mxGeometry");
    mxmlElementSetAttr(fieldGeo, "y", "26");
    mxmlElementSetAttr(fieldGeo, "width", "160");
    mxmlElementSetAttr(fieldGeo, "height", "26");
    mxmlElementSetAttr(fieldGeo, "as", "geometry");

    // Divider line
    mxml_node_t* divider = mxmlNewElement(parent, "mxCell");
    char lineId[64];
    snprintf(lineId, sizeof(lineId), "%s-line", classId);
    mxmlElementSetAttr(divider, "id", lineId);
    mxmlElementSetAttr(divider, "style", "line;strokeWidth=1;fillColor=none;align=left;verticalAlign=middle;spacingTop=-1;spacingLeft=3;spacingRight=3;rotatable=0;labelPosition=right;points=[];portConstraint=eastwest;strokeColor=inherit;");
    mxmlElementSetAttr(divider, "vertex", "1");
    mxmlElementSetAttr(divider, "parent", classId);

    mxml_node_t* lineGeo = mxmlNewElement(divider, "mxGeometry");
    mxmlElementSetAttr(lineGeo, "y", "52");
    mxmlElementSetAttr(lineGeo, "width", "160");
    mxmlElementSetAttr(lineGeo, "height", "8");
    mxmlElementSetAttr(lineGeo, "as", "geometry");

    // Example method
    mxml_node_t* method = mxmlNewElement(parent, "mxCell");
    char methodId[64];
    snprintf(methodId, sizeof(methodId), "%s-method1", classId);
    mxmlElementSetAttr(method, "id", methodId);
    mxmlElementSetAttr(method, "value", "+ method(type): type");
    mxmlElementSetAttr(method, "style", "text;strokeColor=none;fillColor=none;align=left;verticalAlign=top;spacingLeft=4;spacingRight=4;overflow=hidden;rotatable=0;points=[[0,0.5],[1,0.5]];portConstraint=eastwest;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(method, "vertex", "1");
    mxmlElementSetAttr(method, "parent", classId);

    mxml_node_t* methodGeo = mxmlNewElement(method, "mxGeometry");
    mxmlElementSetAttr(methodGeo, "y", "60");
    mxmlElementSetAttr(methodGeo, "width", "160");
    mxmlElementSetAttr(methodGeo, "height", "26");
    mxmlElementSetAttr(methodGeo, "as", "geometry");

    free(classId);

    return 0;
}
