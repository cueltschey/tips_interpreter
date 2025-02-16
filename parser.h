#ifndef PARSER_H
#define PARSER_H

#include "parse_tree_nodes.h"
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

extern std::map<std::string, float> symbolTable;

extern int nextToken;

extern "C" {
extern int yylex();
extern char *yytext;
}

ProgramNode *program();
BlockNode *block();
CompoundStatementNode *compound_statement();
StatementNode *statement();
WriteStatementNode *write();
ReadStatementNode *read();

void declare_ident();
AssignmentStatementNode *assignment();

ExpressionNode *expression();
SimpleExpressionNode *simple_exp();

TermNode *term();
FactorNode *factor();

IfStatementNode *if_statement();
WhileStatementNode *while_statement();

#endif /* PARSER_H */
