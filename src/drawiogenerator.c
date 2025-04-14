#include <stdio.h>
#include "mxml.h"
#include "umlclass.h"

int drawio_output(struct UMLClass* class)
{
    FILE *fp = fopen("output.drawio", "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    mxml_node_t *xml = mxmlNewXML("1.0");
    mxml_node_t *mxfile = mxmlNewElement(xml, "mxfile");
    mxmlElementSetAttr(mxfile, "host", "app.diagrams.net");
    mxmlElementSetAttr(mxfile, "agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    mxmlElementSetAttr(mxfile, "version", "26.1.0");

    mxml_node_t *diagram = mxmlNewElement(mxfile, "diagram");
    mxmlElementSetAttr(diagram, "name", "Page-1");
    mxmlElementSetAttr(diagram, "id", "QqX4JypGpUepcw7Oet94");

    mxml_node_t *graphModel = mxmlNewElement(diagram, "mxGraphModel");
    mxmlElementSetAttr(graphModel, "dx", "753");
    mxmlElementSetAttr(graphModel, "dy", "504");
    mxmlElementSetAttr(graphModel, "grid", "1");
    mxmlElementSetAttr(graphModel, "gridSize", "10");
    mxmlElementSetAttr(graphModel, "guides", "1");
    mxmlElementSetAttr(graphModel, "tooltips", "1");
    mxmlElementSetAttr(graphModel, "connect", "1");
    mxmlElementSetAttr(graphModel, "arrows", "1");
    mxmlElementSetAttr(graphModel, "fold", "1");
    mxmlElementSetAttr(graphModel, "page", "1");
    mxmlElementSetAttr(graphModel, "pageScale", "1");
    mxmlElementSetAttr(graphModel, "pageWidth", "850");
    mxmlElementSetAttr(graphModel, "pageHeight", "1100");
    mxmlElementSetAttr(graphModel, "math", "0");
    mxmlElementSetAttr(graphModel, "shadow", "0");

    mxml_node_t *root = mxmlNewElement(graphModel, "root");

    mxml_node_t *cell0 = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(cell0, "id", "0");
    
    mxml_node_t *cell1 = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(cell1, "id", "1");
    mxmlElementSetAttr(cell1, "parent", "0");
    

    // Class block (container)
    mxml_node_t *classCell = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(classCell, "id", "class1");
    mxmlElementSetAttr(classCell, "value", class->name);
    mxmlElementSetAttr(classCell, "style", "swimlane;fontStyle=1;align=center;verticalAlign=top;childLayout=stackLayout;horizontal=1;startSize=26;horizontalStack=0;resizeParent=1;resizeParentMax=0;resizeLast=0;collapsible=1;marginBottom=0;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(classCell, "vertex", "1");
    mxmlElementSetAttr(classCell, "parent", "1");

    mxml_node_t *classGeo = mxmlNewElement(classCell, "mxGeometry");
    mxmlElementSetAttr(classGeo, "x", "140");
    mxmlElementSetAttr(classGeo, "y", "70");
    mxmlElementSetAttr(classGeo, "width", "160");
    mxmlElementSetAttr(classGeo, "height", "86");
    mxmlElementSetAttr(classGeo, "as", "geometry");

    // Field row
    mxml_node_t *field = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(field, "id", "class1-field1");
    mxmlElementSetAttr(field, "value", "+ field: type");
    mxmlElementSetAttr(field, "style", "text;strokeColor=none;fillColor=none;align=left;verticalAlign=top;spacingLeft=4;spacingRight=4;overflow=hidden;rotatable=0;points=[[0,0.5],[1,0.5]];portConstraint=eastwest;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(field, "vertex", "1");
    mxmlElementSetAttr(field, "parent", "class1");

    mxml_node_t *fieldGeo = mxmlNewElement(field, "mxGeometry");
    mxmlElementSetAttr(fieldGeo, "y", "26");
    mxmlElementSetAttr(fieldGeo, "width", "160");
    mxmlElementSetAttr(fieldGeo, "height", "26");
    mxmlElementSetAttr(fieldGeo, "as", "geometry");

    // Divider line
    mxml_node_t *divider = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(divider, "id", "class1-line");
    mxmlElementSetAttr(divider, "value", "");
    mxmlElementSetAttr(divider, "style", "line;strokeWidth=1;fillColor=none;align=left;verticalAlign=middle;spacingTop=-1;spacingLeft=3;spacingRight=3;rotatable=0;labelPosition=right;points=[];portConstraint=eastwest;strokeColor=inherit;");
    mxmlElementSetAttr(divider, "vertex", "1");
    mxmlElementSetAttr(divider, "parent", "class1");

    mxml_node_t *lineGeo = mxmlNewElement(divider, "mxGeometry");
    mxmlElementSetAttr(lineGeo, "y", "52");
    mxmlElementSetAttr(lineGeo, "width", "160");
    mxmlElementSetAttr(lineGeo, "height", "8");
    mxmlElementSetAttr(lineGeo, "as", "geometry");

    // Method row
    mxml_node_t *method = mxmlNewElement(root, "mxCell");
    mxmlElementSetAttr(method, "id", "class1-method1");
    mxmlElementSetAttr(method, "value", "+ method(type): type");
    mxmlElementSetAttr(method, "style", "text;strokeColor=none;fillColor=none;align=left;verticalAlign=top;spacingLeft=4;spacingRight=4;overflow=hidden;rotatable=0;points=[[0,0.5],[1,0.5]];portConstraint=eastwest;whiteSpace=wrap;html=1;");
    mxmlElementSetAttr(method, "vertex", "1");
    mxmlElementSetAttr(method, "parent", "class1");

    mxml_node_t *methodGeo = mxmlNewElement(method, "mxGeometry");
    mxmlElementSetAttr(methodGeo, "y", "60");
    mxmlElementSetAttr(methodGeo, "width", "160");
    mxmlElementSetAttr(methodGeo, "height", "26");
    mxmlElementSetAttr(methodGeo, "as", "geometry");

    // Save XML to file
    mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
    fclose(fp);
    mxmlDelete(xml);

    return 0;
}
