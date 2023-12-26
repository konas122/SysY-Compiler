#ifndef __TREE_H__
#define __TREE_H__

#include "global.h"
#include "type.h"


enum NodeType {
	NODE_OP,
	NODE_CONST, 
	NODE_VAR,
	NODE_FUNCALL,

	NODE_PROG,
	NODE_STMT,
	NODE_EXPR,
    NODE_TYPE,
	NODE_VARLIST,
	NODE_PARAM,
};

enum OperatorType {
	OP_EQ,  	    // ==
	OP_NEQ, 	    // !=
	OP_GRAEQ,	    // >=
	OP_LESEQ,	    // <=
	OP_ADDASSIGN,	// +=
	OP_SUBASSIGN,	// -=
	OP_MULASSIGN,	// *=
	OP_DIVASSIGN,	// /=
    OP_DECLASSIGN,	// = (仅在常变量声明赋值时使用)
	OP_ASSIGN,	    // =
	OP_GRA,		    // >
	OP_LES,		    // <
	OP_INC,		    // ++
	OP_DEC,		    // --
	OP_ADD,		    // +
	OP_SUB,		    // -
	OP_POS,		    // + (一元运算符)
	OP_NAG,		    // - (一元运算符)
	OP_MUL,		    // *
	OP_DIV,		    // /
	OP_MOD,		    // %
	OP_NOT,		    // !
	OP_AND, 	    // &&
	OP_OR,		    // ||
	OP_INDEX,	    // [] 下标运算符
};

enum StmtType {
	STMT_SKIP,
	STMT_BLOCK,
	STMT_DECL,
	STMT_CONSTDECL,
	STMT_FUNCDECL,
	STMT_IFELSE,
	STMT_IF,
	STMT_WHILE,
	STMT_FOR,
	STMT_RETURN,
	STMT_CONTINUE,
	STMT_BREAK,
};


struct Label {
    string true_label;
    string false_label;
    string begin_label;
    string next_label;
};


struct TreeNode {

public:
	int lineno;

    // ================== 语法树构造 =====================

	TreeNode* child = nullptr;
	TreeNode* sibling = nullptr;

	NodeType nodeType;
	OperatorType optype;    // 运算符类型
	StmtType stype;		    // 表达式类型
	Type* type;			    // 变量、类型、表达式结点，有类型。
	int int_val;
	char ch_val;
	bool b_val;
	string str_val;
	string var_name;
	string var_scope;	    // 变量作用域标识符
    int pointLevel;

	TreeNode(int lineno, NodeType type);
    // 仅用于叶节点拷贝，函数不复制子节点，也不复制子节点指针
	TreeNode(TreeNode* node);

    void addChild(TreeNode *);
    void addSibling(TreeNode *);

    int getChildNum();
	int getVal();

    int nodeID;
	void genNodeId();

    // =================== 输出语法树 =====================

    void printNodeInfo();
	void printChildrenId();

	void printAST();	// 先输出自己 + 孩子们的id；再依次让每个孩子输出 AST。
    void printAST_brief(int tab = 0);

    void printSpecialInfo();
	void printConstVal();

    // ==================== 类型检查 =======================

    void semanticCheck();
    void findReturn(vector<TreeNode *> &retList);

public:
	static string nodeType2String (NodeType type);
	static string opType2String (OperatorType type);
	static string sType2String (StmtType type);
};


void InitIOFunctionNode();
// 设置几个常量 TreeNode, 节省空间开销
static TreeNode* nodeScanf = new TreeNode(0, NODE_VAR);
static TreeNode* nodePrintf = new TreeNode(0, NODE_VAR);

extern bool typeError;

#endif
