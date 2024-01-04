#include "tree.h"

using namespace std;


/* ============================ 全局变量 =============================== */

// multimap <标识符名称,  作用域> 变量名列表
multimap<string, string> idNameList;
// map <<标识符名称,  作用域>, 结点指针> 变量列表
map<pair<string, string>, TreeNode *> idList;

// map <字符串,  标签序列号> 字符串表
map<string, int> strList;

// map <作用域+变量名,  变量相对于ebp偏移量> 局部变量表, 在每次函数定义前清空
// <"11a", "-12"> 表示第一个函数的栈上第一个分配的局部变量（前3个4字节为bx,cx,dx备份用, 始终保留）
map<string, int> LocalVarList;
// 栈上为局部变量分配的空间总大小, 在return时进行清理
int stackSize;

// 当前所处函数的声明结点指针, return使用
TreeNode *pFunction;

// 循环体栈, 为continue与break配对使用
TreeNode *cycleStack[10];
int cycleStackTop = -1;


// =========================== 语法树构造 =======================================

TreeNode::TreeNode(int lineno, NodeType type) {
    this->lineno = lineno;
    this->nodeType = type;
    this->pointLevel = 0;
}

TreeNode::TreeNode(TreeNode* node) {
    this->lineno = node->lineno;
    this->nodeType = node->nodeType;
    this->optype = node->optype;
	this->stype = node->stype;
	this->type = node->type;
	this->int_val = node->int_val;
	this->ch_val = node->ch_val;
	this->b_val = node->b_val;
	this->str_val = node->str_val;
	this->var_name = node->var_name;
	this->var_scope = node->var_scope;
    this->pointLevel = node->pointLevel;
}

void TreeNode::addChild(TreeNode* child) {
    if (this->child == nullptr) {
        this->child = child;
    }
    else {
        this->child->addSibling(child);
    }
}

void TreeNode::addSibling(TreeNode* sibling) {
    TreeNode *p = this;
    while (p->sibling != nullptr) {
        p = p->sibling;
    }
    p->sibling = sibling;
    sibling->sibling = nullptr;
}

int TreeNode::getChildNum() {
    int num = 0;
    for (TreeNode *p = child; p != nullptr; p = p->sibling)
        num++;
    return num;
}

int TreeNode::getVal() {
    if (nodeType == NODE_CONST) {
        switch (type->type)
        {
        case VALUE_BOOL:
            return (b_val ? 1 : 0);
        case VALUE_CHAR:
            return (int)ch_val;
        case VALUE_INT:
            return int_val;
        default:
            return 0;
        }
    }
    else if (child->nodeType == NODE_CONST) {
        return child->getVal();
    }
    return 0;
}

void TreeNode::genNodeId() {
    static unsigned int maxid = 0;
    this->nodeID = maxid++;
    if (this->child)
        this->child->genNodeId();
    if (this->sibling)
        this->sibling->genNodeId();
}


// ============================= 类型检查 ===================================

void TreeNode::semanticCheck() {

#ifdef semanticCheck_debug
    cout << "# Type check reach @" << nodeID << endl;
#endif

    // 类型检查时记录循环层数, 为 continue 和 break 提供循环外错误检查
    if (nodeType == NODE_STMT && (stype == STMT_FOR || stype == STMT_WHILE))
        cycleStackTop++;
    
    // 先遍历子节点进行 type 计算
    TreeNode *p = this->child;
    while (p != nullptr) {
        p->semanticCheck();
        p = p->sibling;
    }

#ifdef semanticCheck_debug
    cout << "# Type check reach @" << nodeID << endl;
#endif

    // 分情况检查类型错误并对部分情况作强制类型转换
    switch (this->nodeType)
    {

    case NODE_FUNCALL:
        // 函数调用要求标识符是一个函数, 且形参表与函数定义一致
        if (child->type->type == COMPOSE_FUNCTION) {
            if (child->var_name == "printf" || child->var_name == "scanf") {
                if (child->sibling->child->type->type != VALUE_STRING) {
                    cerr << "Error: paramater type doesn't fit function " << child->var_name
                         << "need <string>, got " << child->sibling->child->type->getTypeInfo()
                         << " , at line " << lineno << endl;
                    typeError = true;
                }
                break;
            }
            if (child->sibling->getChildNum() == child->type->paramNum) {
                int paracnt = 0;
                TreeNode *param = child->sibling->child;
                while (param!=nullptr) {
                    if (child->type->paramType[paracnt] != TYPE_NONE    // 无类型表示支持任意类型, 在 scanf 和 printf 上使用
                        && child->type->paramType[paracnt]->type != param->type->type) {
                        cerr << "Error: paramater type doesn't fit function " << child->var_name
                                << " got " << param->type->getTypeInfo()
                                << " need " << child->type->paramType[paracnt]->getTypeInfo()
                                << ", at line " << lineno << endl;
                        typeError = true;
                    }
                    paracnt++;
                    param = param->sibling;
                }
            }
            else {
                cerr << "Error: paramater numbers doesn't fit function " << child->var_name << " , at line " << lineno << endl;
                typeError = true;
            }
        }
        else {
            cerr << "Error: identifier " << child->var_name << " isn't a function, at line " << lineno << endl;
            typeError = true;
        }
        if (!type)
            this->type = new Type(NOTYPE);
        this->type->copy(child->type->retType);
        break;

    case NODE_STMT:
        // statement 无类型
        this->type = TYPE_NONE;
        switch (stype)
        {
        case STMT_FUNCDECL: {
            vector<TreeNode *> retList;
            findReturn(retList);
            int size = retList.size();
            if (child->sibling->type->retType->type == VALUE_VOID) {
                // void 类型函数无 return 或 return 无子节点
                for (int i = 0; i < size; i++) {
                    if (retList[i]->child) {
                        cerr << "Wrong return: none void return in void function, at line " << retList[i]->lineno << endl;
                        typeError = true;
                    }
                }
            }
            else {
                // 其它函数必须 return 且类型一致
                if (size == 0) {
                    cerr << "Wrong return: none void function without any return statement, function decl at line " << child->sibling->lineno << endl;
                    typeError = true;
                }
                else {
                    for (int i = 0; i < size; i++) {
                        if (retList[i]->child) {
                            if (retList[i]->child->type->type != child->type->type) {
                                cerr << "Error: return type can't fit function return type, at line " 
                                << retList[i]->lineno << endl;
                            typeError = true;
                            }
                        }
                        else {
                            cerr << "Wrong return: return nothing in none void function, at line " << retList[i]->lineno << endl;
                            typeError = true;
                        }
                    }
                }
            }
            break;
        }

        case STMT_IF:
        case STMT_IFELSE:
        case STMT_WHILE:
            if (child->type->type != VALUE_BOOL) {
                if (child->type->type == VALUE_INT) {
                    // 强制类型转换, 添加一个"!=0"运算过程
                    TreeNode *eq = new TreeNode(child->lineno, NODE_OP);
                    eq->sibling = child->sibling;
                    eq->child = child;
                    eq->type = TYPE_BOOL;
                    eq->optype = OP_NEQ;
                    child->sibling = new TreeNode(child->lineno, NODE_CONST);
                    child->sibling->type = TYPE_INT;
                    child->sibling->int_val = 0;
                    child = eq;
                    cerr << "# Type Cast from <int> to <bool> because of a \""
                         << this->sType2String(stype)
                         << "\" statement, at line " << child->lineno << endl;
                }
                else {
                    cerr << "Error: need <bool>, got " << child->type->getTypeInfo()
                         << ", at line " << child->lineno << endl;
                    typeError = true;
                }
            }
            if (stype == STMT_WHILE)
                cycleStackTop--;
            break;

        case STMT_FOR:
            if (child->sibling->type->type != VALUE_BOOL) {
                if (child->sibling->type->type == VALUE_INT) {
                    cerr << "# Type Cast from <int> to <bool> because of a \"for\" statement, at line "
                         << child->sibling->lineno << endl;
                }
                else {
                    cerr << "Error: need <bool>, got " << child->sibling->type->getTypeInfo()
                         << ", at line " << child->sibling->lineno << endl;
                    typeError = true;
                }
            }
            if (stype == STMT_FOR)
                cycleStackTop--;
            break;

        case STMT_BREAK:
        case STMT_CONTINUE:
            if (cycleStackTop < 0) {
                cerr << "Error cycle control statement: " << sType2String(stype)
                     << ", outside a cycle, at line " << lineno << endl;
                typeError = true;
            }
            break;

        case STMT_RETURN:

            break;
        default:
            break;
        }
        break;

    case NODE_EXPR:
        this->type = this->child->type;
        this->pointLevel = this->child->pointLevel;
        break;

    case NODE_OP:
        if (optype == OP_INC || optype == OP_DEC || optype == OP_POS || optype == OP_NAG) {
            // 一元运算符, 输入 int, 输出 int, ++, --, +(一元), -(一元)
            if (this->child->type->type != VALUE_INT) {
                cerr << "Error: need <int>, got <" << child->type->getTypeInfo()
                     << ">, operator is " << opType2String(optype) << ", at line " << lineno << endl;
                typeError = true;
            }
            this->type = TYPE_INT;
        } 
        else if (optype == OP_EQ || optype == OP_NEQ || optype == OP_ASSIGN || optype == OP_DECLASSIGN) {
            // 二元运算符, 输入无限制, 两侧同类型, ==,!=,=
            // ------------------- 不要在指针声明时候初始化 --------------------
            if (this->child->type->type != this->child->sibling->type->type) {
                cerr << "Error: type in two sides of " << opType2String(optype)
                     << " operator mismatched, got <" << child->type->getTypeInfo()
                     << "> and <" << child->sibling->type->getTypeInfo()
                     << ">, at line " << lineno << endl;
                typeError = true;
            }
            if (optype == OP_ASSIGN && child->type->constvar) {
                cerr << "Wrong assign: assign to a const varable, at line " << lineno;
                typeError = true;
            }
            if (optype == OP_ASSIGN || optype == OP_DECLASSIGN)
                this->type = this->child->type;
            else
                this->type = TYPE_BOOL;
        }
        else if (optype == OP_GRA || optype == OP_LES || optype == OP_GRAEQ || optype == OP_LESEQ) {
            // 二元运算符, 输入 int, 输出 bool, >,<,>=,<=
            if (this->child->type->type != this->child->sibling->type->type || this->child->type->type != VALUE_INT) {
                cerr << "Error: need <int>, got <" << child->type->getTypeInfo()
                     << "> and <" << child->sibling->type->getTypeInfo()
                     << ">, operator is " << opType2String(optype) << ", at line " << lineno << endl;
                typeError = true;
            }
            this->type = TYPE_BOOL;
        }
        else if (optype == OP_ADDASSIGN || optype == OP_SUBASSIGN || optype == OP_MULASSIGN || optype == OP_DIVASSIGN) {
            // 二元运算符, 输入 int, 输出 int, +=,-=,*=,/=
            if (this->child->type->type != this->child->sibling->type->type || this->child->type->type != VALUE_INT) {
                cerr << "Error: need <int>, got <" << child->type->getTypeInfo()
                     << "> and <" << child->sibling->type->getTypeInfo()
                     << ">, operator is " << opType2String(optype) << ", at line " << lineno << endl;
                typeError = true;
            } 
            else if (child->type->constvar) {
                cerr << "Wrong assign: assign to a const varable, at line " << lineno;
                typeError = true;
            }
            this->type = TYPE_INT;
        }
        else if (optype == OP_NOT || optype == OP_AND || optype == OP_OR) {
            // 二元运算符, 输入 bool, 输出 bool, !,&&,||
            if (this->child->type->type != VALUE_BOOL 
                || (optype != OP_NOT 
                && this->child->type->type != this->child->sibling->type->type)) {
                cerr << "Error: need <bool>, got <" << child->type->getTypeInfo()
                     << "> and <" << child->sibling->type->getTypeInfo()
                     << ">, operator is " << opType2String(optype) << ", at line " << lineno << endl;
                typeError = true;
            }
            this->type = TYPE_BOOL;
        }
        else if (optype == OP_INDEX) {
            // 二元运算符, 输入 int, 输出左值类型, [] 下标运算符
            if (this->child->sibling->type->type != VALUE_INT) {
                cerr << "Error: need <int>, got <" << child->sibling->type->getTypeInfo()
                     << ">, operator is " << opType2String(optype) << ", at line " << lineno << endl;
                typeError = true;
            }
            this->type = this->child->type;
        }
        else {
            // 二元运算符, 输入 int, 输出 int, +,-,*,/,%
            if (this->child->type->type != this->child->sibling->type->type || this->child->type->type != VALUE_INT) {
                cerr << "Error: need <int>, got <" << child->type->getTypeInfo()
                     << "> and <" << child->sibling->type->getTypeInfo()
                     << ">, operator is " << opType2String(optype) << ", at line " << lineno << endl;
                typeError = true;
            }
            this->type = TYPE_INT;
        }
        break;

    case NODE_PROG:
        this->type = TYPE_NONE;
        break;

    case NODE_VARLIST:
    case NODE_PARAM:
        if (this->child)
            this->type = this->child->type;
        break;
    default:
        break;
    }
    
#ifdef semanticCheck_debug
    cout << "# Type check end @" << nodeID << endl;
#endif

}

void TreeNode::findReturn(vector<TreeNode *> &retList) {
    if (nodeType == NODE_STMT && stype == STMT_RETURN)
        retList.push_back(this);
    else {
        TreeNode *p = child;
        while (p) {
            p->findReturn(retList);
            p = p->sibling;
        }
    }
}


// ============================ 输出语法树 =====================================

void TreeNode::printAST() {
    printNodeInfo();
    printChildrenId();
    cout << endl;
    TreeNode *p = this->child;
    while (p != nullptr) {
        p->printAST();
        p = p->sibling;
    }
}

void TreeNode::printAST_brief(int tab) {
    cout << "line " << lineno << "\t";
    for (int i = 0; i < tab; i++)
        cout << "\t";
    printNodeInfo();
    cout << endl;
    TreeNode *p = this->child;
    while (p != nullptr) {
        p->printAST_brief(tab + 1);
        p = p->sibling;
    }
}

void TreeNode::printNodeInfo() {
    // cout << "@" << this->nodeID << "\t";
    // cout << "line " << lineno << "\t";
    cout << nodeType2String(this->nodeType);
    this->printSpecialInfo();
}

void TreeNode::printChildrenId() {
    cout << ",\tchild:";
    TreeNode *p = this->child;
    if (p == nullptr)
        cout << "-";
    while (p) {
        cout << "\t@" << p->nodeID;
        p = p->sibling;
    }
    cout << "\t";
}

void TreeNode::printSpecialInfo() {
    switch(this->nodeType){
        case NODE_CONST:
            cout << ", ";
            this->printConstVal();
            break;
        case NODE_VAR:
            cout << ",\tname: ";
            if (this->type->pointLevel != 0) {
                // 为指针类型添加前缀(*和&)
                string prefix = this->type->pointLevel > 0 ? "*" : "&";
                for (int i=0; i < abs(this->type->pointLevel); i++)
                    cout << prefix;
            }
            cout << var_name;
            // cout << ",\tscope: ";
            // for (unsigned int i = 0; i < var_scope.length(); i++)
            //     cout << var_scope[i] << " ";
            break;
        case NODE_EXPR:
            break;
        case NODE_STMT:
            cout << ", " << sType2String(this->stype) << "\t";
            if (this->stype == STMT_DECL || this->stype == STMT_CONSTDECL) {
                if (this->child && this->child->sibling && this->child->sibling->type)
                    cout << this->child->sibling->type->getTypeInfo() << "\t";
            }
            break;
        case NODE_TYPE:
            cout << ", " << this->type->getTypeInfo();
            break;
        case NODE_OP:
            cout << ", " << opType2String(this->optype) << "\t";
            break;
        default:
            break;
    }
}

string TreeNode::nodeType2String (NodeType type){
    switch (type)
    {
    case NODE_CONST:
        return "<const>";
    case NODE_VAR:
        return "<var>";
    case NODE_EXPR:
        return "<expression>";
    case NODE_TYPE:
        return "<type>";
    case NODE_FUNCALL:
        return "function call";
    case NODE_STMT:
        return "<statment>";
    case NODE_PROG:
        return "<program>";
    case NODE_VARLIST:
        return "<variable list>";
    case NODE_PARAM:
        return "function format parameter";
    case NODE_OP:
        return "<operation>";
    default:
        return "<?>";
    }
}

string TreeNode::sType2String(StmtType type) {
    switch (type)
    {
    case STMT_SKIP:
        return "skip";
    case STMT_DECL:
        return "declaration";
    case STMT_CONSTDECL:
        return "const declaration";
    case STMT_FUNCDECL:
        return "function declaration";
    case STMT_BLOCK:
        return "block";
    case STMT_IF:
        return "if";
    case STMT_IFELSE:
        return "if with else";
    case STMT_WHILE:
        return "while";
    case STMT_FOR:
        return "for";
    case STMT_RETURN:
        return "return";
    case STMT_CONTINUE:
        return "continue";
    case STMT_BREAK:
        return "break";
    default:
        return "?";
    }
}

string TreeNode::opType2String(OperatorType type) {
    switch (type)
    {
	case OP_EQ:
		return "equal";
	case OP_NEQ:
		return "not equal";
	case OP_GRAEQ:
		return "grater equal";
	case OP_LESEQ:
		return "less equal";
	case OP_ASSIGN:
		return "assign";
	case OP_DECLASSIGN:
		return "assign(decl)";
	case OP_ADDASSIGN:
		return "add assign";
	case OP_SUBASSIGN:
		return "sub assign";
	case OP_MULASSIGN:
		return "multiply assign";
	case OP_DIVASSIGN:
		return "divide assign";
	case OP_GRA:
		return "grater";
	case OP_LES:
		return "less";
    case OP_INC:
        return "auto increment";
    case OP_DEC:
        return "auto decrement";
    case OP_ADD:
        return "add";
	case OP_SUB:
		return "sub";
	case OP_POS:
		return "positive";
	case OP_NAG:
		return "nagative";
	case OP_MUL:
		return "multiply";
	case OP_DIV:
		return "divide";
	case OP_MOD:
		return "Modulo";
	case OP_NOT:
		return "not";
	case OP_AND:
		return "and";
	case OP_OR:
        return "or";
    case OP_INDEX:
        return "index";
    default:
        return "?";
    }
}

void TreeNode::printConstVal() {
    if (this->nodeType == NODE_CONST){
        cout << this->type->getTypeInfo() << ":";
        switch (this->type->type)
        {
        case VALUE_BOOL:
            if (this->b_val)
                cout << "true";
            else
                cout << "fasle";
            break;
        case VALUE_CHAR:
            switch (ch_val)
            {
            case '\n':
                cout << "\\n";
                break;
            case '\r':
                cout << "\\r";
                break;
            case '\t':
                cout << "\\t";
                break;
            case '\0':
                cout << "\\0";
                break;
            default:
                cout << ch_val;
                break;
            }
            break;
        case VALUE_INT:
            cout << int_val;
            break;
        case VALUE_STRING:
            cout << str_val;
            break;
        default:
            cout << "-";
            break;
        }
    }
}


void InitIOFunctionNode() {
    int k = 4;

    nodeScanf->lineno = -1;
    nodeScanf->var_name = "scanf";
    nodeScanf->var_scope = "1";
    nodeScanf->type = new Type(COMPOSE_FUNCTION);
    nodeScanf->type->retType = TYPE_VOID;
    nodeScanf->type->paramType[nodeScanf->type->paramNum++] = TYPE_STRING;

    for (int i = 0; i < k;i++)
        nodeScanf->type->paramType[nodeScanf->type->paramNum++] = TYPE_INT;

    idNameList.insert(make_pair("scanf", "1"));
    idList[make_pair("scanf", "1")] = nodeScanf;
    nodePrintf->lineno = -1;
    nodePrintf->var_name = "printf";
    nodePrintf->var_scope = "1";
    nodePrintf->type = new Type(COMPOSE_FUNCTION);
    nodePrintf->type->retType = TYPE_VOID;
    nodePrintf->type->paramType[nodePrintf->type->paramNum++] = TYPE_STRING;

    for (int i = 0; i < k;i++)
        nodePrintf->type->paramType[nodePrintf->type->paramNum++] = TYPE_INT;

    idNameList.insert(make_pair("printf", "1"));
    idList[make_pair("printf", "1")] = nodePrintf;
}
