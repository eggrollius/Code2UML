#include <clang-c/Index.h>
#include <clang-c/CXString.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "umlclass.h"
#include "drawiogenerator.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define INITIAL_PATHS_BUFFER_SIZE 10

char** filePathsBuf = NULL;
int filePathsBufSize = 0;
int numFiles = 0;

struct UMLClass** classes = NULL;
int numClasses = 0;

typedef struct {
    const char* curFileName; // Current file name being processed
} VisitorData;

int checkError(enum CXErrorCode code)
{
    switch(code) {
        case CXError_Failure:
            puts("CX Failure");
            return 1;
        case CXError_Crashed:
            puts("CX Crashed");
            return 1;
        case CXError_InvalidArguments:
            puts("CX Invalid Arguments");
            return 1;
        case CXError_ASTReadError:
            puts("CX AST Read Error");
            return 1;
        break;
    }

    return 0;
}

CXString getFileName(CXCursor cursor)
{
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned line, col, offset;
    clang_getSpellingLocation(location, &file, &line, &col, &offset);
    CXString fileName = clang_getFileName(file);

    return fileName;
}

void handleClassDeclaration(CXCursor cursor, CXString spelling) {
    const char* tmp = clang_getCString(spelling);

    // Allocate new class
    struct UMLClass* newClass = malloc(sizeof(struct UMLClass));
    newClass->name = strdup(tmp);
    newClass->attributes = NULL;
    newClass->attributeCount = 0;
    newClass->methods = NULL;
    newClass->methodCount = 0;

    printf("Assigning class name: %s\n", tmp);

    // Resize and store in array
    classes = realloc(classes, sizeof(struct UMLClass*) * (numClasses + 1));
    classes[numClasses++] = newClass;
}

void handleFieldDeclaration(CXCursor cursor, CXString spelling) {
    // Extract attribute details
    const char* attributeName = clang_getCString(spelling);
    CXType attributeType = clang_getCursorType(cursor);
    CXString typeSpelling = clang_getTypeSpelling(attributeType);
    const char* attributeTypeName = clang_getCString(typeSpelling);

    // Extract access specifier
    enum CX_CXXAccessSpecifier accessSpecifier = clang_getCXXAccessSpecifier(cursor);
    AccessModifier accessModifier;
    switch (accessSpecifier) {
        case CX_CXXPublic: accessModifier = PUBLIC; break;
        case CX_CXXPrivate: accessModifier = PRIVATE; break;
        case CX_CXXProtected: accessModifier = PROTECTED; break;
        default: accessModifier = PUBLIC; // Default to public if unspecified
    }

    printf("Attribute: %s, Type: %s, Access: %s\n", attributeName, attributeTypeName, accessModifierToString(accessModifier));

    // Add attribute to the last class
    struct UMLClass* currentClass = classes[numClasses - 1];
    currentClass->attributes = realloc(currentClass->attributes, sizeof(struct UMLAttribute) * (currentClass->attributeCount + 1));
    struct UMLAttribute* newAttribute = &currentClass->attributes[currentClass->attributeCount++];
    newAttribute->name = strdup(attributeName);
    newAttribute->type = strdup(attributeTypeName);
    newAttribute->defaultValue = NULL; // Default value extraction can be added later
    newAttribute->isStatic = 0; // all Fields are non-static
    newAttribute->accessModifier = accessModifier;

    clang_disposeString(typeSpelling);
}

void handleStaticDataMemberDecleration(CXCursor cursor, CXString spelling) {
    // Check that the var is static 
    enum CX_StorageClass storage = clang_Cursor_getStorageClass(cursor);
    if (storage != CX_SC_Static) {
        return;
    }

    // Check that the cursor is inside a class
    CXCursor semanticParent = clang_getCursorSemanticParent(cursor);
    if (clang_getCursorKind(semanticParent) != CXCursor_StructDecl &&
        clang_getCursorKind(semanticParent) != CXCursor_ClassDecl &&
        clang_getCursorKind(semanticParent) != CXCursor_ClassTemplate) {
        return;
    }

    // Extract variable details
    const char* varName = clang_getCString(spelling);
    CXType varType = clang_getCursorType(cursor);
    CXString typeSpelling = clang_getTypeSpelling(varType);
    const char* varTypeName = clang_getCString(typeSpelling);

    // Extract access specifier
    enum CX_CXXAccessSpecifier accessSpecifier = clang_getCXXAccessSpecifier(cursor);
    AccessModifier accessModifier;
    switch (accessSpecifier) {
        case CX_CXXPublic: accessModifier = PUBLIC; break;
        case CX_CXXPrivate: accessModifier = PRIVATE; break;
        case CX_CXXProtected: accessModifier = PROTECTED; break;
        default: accessModifier = PUBLIC; // Default to public if unspecified
    }

    printf("Variable: %s, Type: %s, Access: %s\n", varName, varTypeName, accessModifierToString(accessModifier));

    // Add variable to the last class
    struct UMLClass* currentClass = classes[numClasses - 1];
    currentClass->attributes = realloc(currentClass->attributes, sizeof(struct UMLAttribute) * (currentClass->attributeCount + 1));
    struct UMLAttribute* newVariable = &currentClass->attributes[currentClass->attributeCount++];
    newVariable->name = strdup(varName);
    newVariable->type = strdup(varTypeName);
    newVariable->defaultValue = NULL; // Default value extraction can be added later
    newVariable->isStatic = 1; // We checked that the var is static at top of function
    newVariable->accessModifier = accessModifier;
    clang_disposeString(typeSpelling);
}

void handleMethodDeclaration(CXCursor cursor, CXString spelling) {
    // Extract method details
    const char* methodName = clang_getCString(spelling);
    CXType returnType = clang_getCursorResultType(cursor);
    CXString returnTypeSpelling = clang_getTypeSpelling(returnType);
    const char* returnTypeName = clang_getCString(returnTypeSpelling);

    // Extract access specifier
    enum CX_CXXAccessSpecifier accessSpecifier = clang_getCXXAccessSpecifier(cursor);
    AccessModifier accessModifier;
    switch (accessSpecifier) {
        case CX_CXXPublic: accessModifier = PUBLIC; break;
        case CX_CXXPrivate: accessModifier = PRIVATE; break;
        case CX_CXXProtected: accessModifier = PROTECTED; break;
        default: accessModifier = PUBLIC; // Default to public if unspecified
    }

    printf("Method: %s, Return Type: %s\n", methodName, returnTypeName);

    // Add method to the last class
    struct UMLClass* currentClass = classes[numClasses - 1];
    currentClass->methods = realloc(currentClass->methods, sizeof(struct UMLMethod) * (currentClass->methodCount + 1));
    struct UMLMethod* newMethod = &currentClass->methods[currentClass->methodCount++];
    newMethod->name = strdup(methodName);
    newMethod->returnType = strdup(returnTypeName);
    newMethod->paramNames = NULL;
    newMethod->paramTypes = NULL;
    newMethod->paramCount = 0;
    newMethod->accessModifier = accessModifier;
    newMethod->isStatic = clang_CXXMethod_isStatic(cursor);

    // Extract parameters
    unsigned int numArgs = clang_Cursor_getNumArguments(cursor);
    newMethod->paramNames = malloc(sizeof(char*) * numArgs);
    newMethod->paramTypes = malloc(sizeof(char*) * numArgs);
    newMethod->paramCount = numArgs;

    for (unsigned int i = 0; i < numArgs; ++i) {
        CXCursor paramCursor = clang_Cursor_getArgument(cursor, i);
        CXString paramName = clang_getCursorSpelling(paramCursor);
        CXType paramType = clang_getCursorType(paramCursor);
        CXString paramTypeSpelling = clang_getTypeSpelling(paramType);

        newMethod->paramNames[i] = strdup(clang_getCString(paramName));
        newMethod->paramTypes[i] = strdup(clang_getCString(paramTypeSpelling));

        printf("  Param: %s, Type: %s\n", newMethod->paramNames[i], newMethod->paramTypes[i]);

        clang_disposeString(paramName);
        clang_disposeString(paramTypeSpelling);
    }

    clang_disposeString(returnTypeSpelling);
}

enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
    // Access the current file name from clientData
    VisitorData* visitorData = (VisitorData*)data;
    const char* curFileName = visitorData->curFileName;

    // Check that cursor source file is our current file
    CXString fileNameStr = getFileName(cursor);
    const char* fileName = clang_getCString(fileNameStr);
    if (strcmp(fileName, curFileName) != 0) {
        clang_disposeString(fileNameStr);
        return CXChildVisit_Continue;
    }

    enum CXCursorKind kind = clang_getCursorKind(cursor);
    CXString spelling = clang_getCursorSpelling(cursor);

    if (kind == CXCursor_ClassDecl && clang_isCursorDefinition(cursor)) {
        handleClassDeclaration(cursor, spelling);
    }
    else if (kind == CXCursor_FieldDecl) {
        handleFieldDeclaration(cursor, spelling);
    }
    else if (kind == CXCursor_VarDecl) {
        handleStaticDataMemberDecleration(cursor, spelling);
    }
    else if (kind == CXCursor_CXXMethod) {
        handleMethodDeclaration(cursor, spelling);
    }

    clang_disposeString(spelling);
    clang_disposeString(fileNameStr);

    return CXChildVisit_Recurse;
}

/**
 * @brief Recursively processes a directory to find all files.
 * 
 * @param dirPath The path to the directory.
 */
void processDirectory(const char* dirPath) {
    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat pathStat;
        if (stat(fullPath, &pathStat) != 0) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(pathStat.st_mode)) {
            // Recursively process subdirectories
            processDirectory(fullPath);
        } else if (S_ISREG(pathStat.st_mode)) {
            // Add file to the buffer
            if (numFiles >= filePathsBufSize) {
                filePathsBufSize *= 2;
                filePathsBuf = realloc(filePathsBuf, sizeof(char*) * filePathsBufSize);
                if (!filePathsBuf) {
                    perror("realloc");
                    closedir(dir);
                    exit(1);
                }
            }
            filePathsBuf[numFiles++] = strdup(fullPath);
        }
    }

    closedir(dir);
}

/**
 * @brief Processes all files in the filePathsBuf array.
 */
void processFiles() {
    for (int i = 0; i < numFiles; ++i) {
        const char* filePath = filePathsBuf[i];
        printf("Processing file: %s\n", filePath);

        CXIndex index = clang_createIndex(0, 0);

        const char* args[] = {
            "-x", "c++",          // Treat as C++
            "-std=c++20",         // Use C++20 standard
            "-I.",                // Include current directory for headers
        };

        CXTranslationUnit tu;
        enum CXErrorCode CXRet = clang_parseTranslationUnit2(
            index,
            filePath,
            args, sizeof(args) / sizeof(args[0]),
            NULL, 0,
            CXTranslationUnit_None,
            &tu
        );

        if (checkError(CXRet)) {
            clang_disposeIndex(index);
            continue;
        }

        CXCursor root = clang_getTranslationUnitCursor(tu);

        // Create VisitorData and set the current file name
        VisitorData visitorData = { .curFileName = filePath };

        // Pass VisitorData to the visitor function
        clang_visitChildren(root, visitor, &visitorData);

        clang_disposeTranslationUnit(tu);
        clang_disposeIndex(index);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path> [<path> ...]\n", argv[0]);
        return 1;
    }

    // Initialize file paths buffer
    filePathsBufSize = INITIAL_PATHS_BUFFER_SIZE;
    filePathsBuf = malloc(sizeof(char*) * filePathsBufSize);
    if (!filePathsBuf) {
        perror("malloc");
        return 1;
    }

    // Process each argument
    for (int i = 1; i < argc; ++i) {
        struct stat pathStat;
        if (stat(argv[i], &pathStat) != 0) {
            perror("stat");
            continue;
        }

        if (S_ISREG(pathStat.st_mode)) {
            // Add file to the buffer
            if (numFiles >= filePathsBufSize) {
                filePathsBufSize *= 2;
                filePathsBuf = realloc(filePathsBuf, sizeof(char*) * filePathsBufSize);
                if (!filePathsBuf) {
                    perror("realloc");
                    exit(1);
                }
            }
            filePathsBuf[numFiles++] = strdup(argv[i]);
        } else if (S_ISDIR(pathStat.st_mode)) {
            // Process directory
            processDirectory(argv[i]);
        } else {
            printf("Skipping unsupported path: %s\n", argv[i]);
        }
    }

    // Process all collected files
    processFiles();

    // Output UML classes to a draw.io file
    if (drawio_classesToFile(classes, numClasses, "out.drawio")) {
        puts("Problem when outputting");
    }

    // Free allocated memory
    for (int i = 0; i < numFiles; ++i) {
        free(filePathsBuf[i]);
    }
    free(filePathsBuf);

    for (int i = 0; i < numClasses; ++i) {
        free(classes[i]->name);
        free(classes[i]);
    }
    free(classes);

    return 0;
}