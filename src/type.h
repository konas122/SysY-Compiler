#ifndef __TYPE_H__
#define __TYPE_H__

#include "global.h"

using namespace std;

#define MAX_PARAM 16
#define MAX_ARRAY_DIM 8
#define MAX_SCOPE_STACK 32
#define SCOPT_ID_BASE '1'


// #define DEBUG
#ifdef DEBUG
#define PARSER_DEBUG

#ifdef  PARSER_DEBUG
#define SCOPE_DEBUG
#define ID_REDUCE_DEBUG
#define DECL_DEBUG
#define POINT_DEBUG
#define ASSIGN_DEBUG
#define IF_DEBUG
#define IFELSE_DEBUG
#define WHILE_DEBUG
#define FUNCALL_DEBUG
#endif

#define typeCheck_debug
#define childNumdebug
#define varDeclDebug
#define EXIT_AT_GRAMMA_ERROR
#endif


enum ValueType {
    NOTYPE,
    VALUE_BOOL,
    VALUE_INT,
    VALUE_CHAR,
    VALUE_STRING,
    VALUE_VOID,
    VALUE_ARRAY,
    COMPOSE_FUNCTION
};


class Type {

public:
    bool constvar;
    ValueType type;
    Type(ValueType valueType);
    void copy(Type* a);

public:
    int pointLevel = 0;

    unsigned short paramNum;    // for function
    Type* paramType[MAX_PARAM];
    Type* retType;

    unsigned int dim;           // for array
    ValueType elementType;
    int dimSize[MAX_ARRAY_DIM];

    // 下一次使用下标运算符会访问的维度
    unsigned int visitDim = 0;

    int getSize();

    void addParam(Type* t);
    void addRet(Type* t);

public:
    string getTypeInfo();
    string getTypeInfo(ValueType type);
};

// 设置几个常量 Type, 节省空间开销
static Type* TYPE_INT = new Type(VALUE_INT);
static Type* TYPE_CHAR = new Type(VALUE_CHAR);
static Type* TYPE_BOOL = new Type(VALUE_BOOL);
static Type* TYPE_STRING = new Type(VALUE_STRING);
static Type* TYPE_VOID = new Type(VALUE_VOID);
static Type* TYPE_NONE = new Type(NOTYPE);

int getSize(Type* type);

#endif
