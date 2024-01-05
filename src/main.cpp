#include <fstream>
#include "common.h"

extern TreeNode *root;
extern FILE *yyin;
extern int yyparse();

bool parserError = false;
bool semanticError = false;

using namespace std;


int main(int argc, char *argv[]) {
    cerr << endl;

    if (argc == 2) {
        FILE *fin = fopen(argv[1], "r");
        if (fin != nullptr)
            yyin = fin;
        else {
            cerr << "failed to open file: " << argv[1] << endl;
            return -1;
        }
    }

    InitIOFunctionNode();
    yyparse();

    if (parserError || root == nullptr)
        return -2;

    root->genNodeId();

#ifdef AST
    cout << "# --------------------------------------------------" << endl;
    // root->printAST();
    root->printAST_brief();
    cout << "# --------------------------------------------------" << endl;
#endif

    root->semanticCheck();
    if (semanticError)
        return -3;

#ifndef AST
    root->genCode();
#endif


    return 0;
}
