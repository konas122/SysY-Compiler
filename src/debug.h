#ifndef __DEBUG_H__
#define __DEBUG_H__


// #define EXIT_AT_GRAMMA_ERROR
// #define DEBUG

#ifdef DEBUG

#define AST
#define PARSER_DEBUG
#ifdef PARSER_DEBUG
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

#define semanticCheck_debug
#define childNumdebug
#define varDeclDebug
#endif


#endif
