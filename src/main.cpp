#include <fstream>
#include "common.h"

extern TreeNode *root;
extern FILE *yyin;
extern int yyparse();

bool parserError = false;
bool typeError = false;

using namespace std;


int main(int argc, char *argv[]) {
    cerr << endl;

    if (argc == 2) {
        FILE *fin = fopen(argv[1], "r");
        if (fin != nullptr)
            yyin = fin;
        else
            cerr << "failed to open file: " << argv[1] << endl;
    }

    InitIOFunctionNode();
    yyparse();

    if (parserError || root == nullptr)
        return 0;

    root->genNodeId();

    cout << "# --------------------------------------------------" << endl;
    // root->printAST();
    root->printAST_brief();
    cout << "# --------------------------------------------------" << endl;
    
    root->semanticCheck();

    if (typeError)
        return 0;

    return 0;
}
