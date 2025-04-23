#ifndef DRAWIO_GENERATOR_H
#define DRAWIO_GENERATOR_H

#include "umlclass.h"
#include "mxml.h" 

static unsigned int class_id_counter = 1;

/**
 * @brief Represents a single diagram page in a draw.io file.
 */
struct Diagram {
    char* name;                   ///< Name of the diagram/page.
    struct UMLClass* classes;     ///< Array of UML class objects to be rendered.
    unsigned int numClasses;      ///< Number of UML classes in the diagram.
};

/**
 * @brief Represents a draw.io file that may contain multiple diagrams.
 */
struct DrawIOFile {
    char* name;                  ///< File name or identifier for output.
    struct Diagram* diagrams;   ///< Array of diagram structures.
    unsigned int numDiagrams;   ///< Number of diagrams in the file.
};

/**
 * @brief Generates a unique class ID string like "class1", "class2", etc.
 * @return Newly allocated string (caller must free)
 */
char* drawio_generateUniqueClassId();

/**
 * @brief Generates the draw.io file from an array of classes
 * 
 * @param classes An array of UMLClasses to include in the outputted diagram.
 * @param numClasses The number of classes in the array
 * @param filePath The file path to save to.
 * @returns 0 on success, 1 on error.
 */
int drawio_classesToFile(struct UMLClass** classes, unsigned int numClasses, char* filePath);

/**
 * @brief Generates a blank drawio xml file node.
 * 
 * @param out A pointer where the generated node will be stored.
 * @returns 0 on success, non-zero on error.
 */
int drawio_generateFileNode(mxml_node_t** out);

/**
 * @brief Generates the XML headers for a drawio file.
 * 
 * @param out A pointer to the xml parent node of the xml header node.
 * @returns 0 on success, non-zero on error.
 */
int drawio_generateHeader(mxml_node_t* out);

/**
 * @brief Generates the draw.io XML from the given UMLClass.
 *
 * @param class Pointer to the UMLClass to convert into XML format.
 * @param parent Pointer to a mini-xml node, the generated XML nodes will be 
 *               stored as children of this node
 * @param x The x-coordinate for the class block.
 * @param y The y-coordinate for the class block.
 * @return int 0 on success, non-zero on error.
 */
int drawio_classToXML(struct UMLClass* class, mxml_node_t* parent, unsigned int x, unsigned int y);

/**
 * @brief Calculates the required width of a UML class block based on its attributes and methods.
 * 
 * @param class Pointer to the UMLClass whose width is to be calculated.
 * @return The calculated width in pixels.
 */
unsigned int drawio_calculateWidth(struct UMLClass* class);

/**
 * @brief Generates the XML for the class cell in the draw.io diagram.
 * 
 * @param class Pointer to the UMLClass to generate the cell for.
 * @param parent Pointer to the parent XML node.
 * @param classId Unique ID for the class cell.
 * @param width The width of the class cell in pixels.
 * @param x The x-coordinate of the class cell.
 * @param y The y-coordinate of the class cell.
 * @return 0 on success, non-zero on error.
 */
int drawio_generateClassCell(struct UMLClass* class, mxml_node_t* parent, char* classId, unsigned int width, unsigned int x, unsigned int y);

/**
 * @brief Adds the attributes of a UML class to the draw.io diagram.
 * 
 * @param class Pointer to the UMLClass whose attributes are to be added.
 * @param parent Pointer to the parent XML node.
 * @param classId Unique ID for the class cell.
 * @param y Pointer to the current y-coordinate (will be updated).
 * @param width The width of the attribute cells in pixels.
 * @return 0 on success, non-zero on error.
 */
int drawio_addAttributes(struct UMLClass* class, mxml_node_t* parent, char* classId, unsigned int* y, unsigned int width);

/**
 * @brief Adds a divider between attributes and methods in the UML class block.
 * 
 * @param parent Pointer to the parent XML node.
 * @param classId Unique ID for the class cell.
 * @param y Pointer to the current y-coordinate (will be updated).
 * @param width The width of the divider in pixels.
 * @return 0 on success, non-zero on error.
 */
int drawio_addDivider(mxml_node_t* parent, char* classId, unsigned int* y, unsigned int width);

/**
 * @brief Adds the methods of a UML class to the draw.io diagram.
 * 
 * @param class Pointer to the UMLClass whose methods are to be added.
 * @param parent Pointer to the parent XML node.
 * @param classId Unique ID for the class cell.
 * @param y Pointer to the current y-coordinate (will be updated).
 * @param width The width of the method cells in pixels.
 * @return 0 on success, non-zero on error.
 */
int drawio_addMethods(struct UMLClass* class, mxml_node_t* parent, char* classId, unsigned int* y, unsigned int width);

#endif // DRAWIO_GENERATOR_H
