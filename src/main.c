#include <clang-c/Index.h>
#include <clang-c/CXString.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "umlclass.h"
#include "drawiogenerator.h"

#define FILE_NAME "test.h"

struct UMLClass** classes = NULL;
int numClasses = 0;


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

enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
    // Check that cursor source file is our current file
    CXString fileNameStr = getFileName(cursor);
    const char* fileName = clang_getCString(fileNameStr);
    if(strcmp(fileName, FILE_NAME) != 0) {
        clang_disposeString(fileNameStr);
        return CXChildVisit_Continue;
    }

    enum CXCursorKind kind = clang_getCursorKind(cursor);
    CXString spelling = clang_getCursorSpelling(cursor);

    if (kind == CXCursor_ClassDecl && clang_isCursorDefinition(cursor)) {
        const char* tmp = clang_getCString(spelling);
    
        // Allocate new class
        struct UMLClass* newClass = malloc(sizeof(struct UMLClass));
        newClass->name = strdup(tmp);
        // You can add fields/methods list here later
        printf("Assigning class name: %s\n", tmp);
    
        // Resize and store in array
        classes = realloc(classes, sizeof(struct UMLClass*) * (numClasses + 1));
        classes[numClasses++] = newClass;
    }
    

    clang_disposeString(spelling);

    return CXChildVisit_Recurse;

}

int main()
{
    const char* testFile = FILE_NAME;

    CXIndex index = clang_createIndex(0, 0);

    const char *args[] = {
        "-x", "c++",          // treat as C++
        "-std=c++20",         // c++20
        "-I.",                // current dir for headers
    };

    CXTranslationUnit tu;
    enum CXErrorCode CXRet = clang_parseTranslationUnit2(
        index,
        testFile,
        args, sizeof(args)/sizeof(args[0]),
        NULL, 0,
        CXTranslationUnit_None,
        &tu
    );

    if(checkError(CXRet)) {
        return 1;
    }

    CXCursor root = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(root, visitor, NULL);

    for (int i = 0; i < numClasses; ++i) {
        printf("Class[%d]: %s\n", i, classes[i]->name);
    }

    if (drawio_classesToFile(classes, numClasses, "out.drawio")) {
        puts("Problem when outputting");
    }
    
    for (int i = 0; i < numClasses; ++i) {
        free(classes[i]->name);
        free(classes[i]);
    }
    free(classes);
    
    return 0;
}