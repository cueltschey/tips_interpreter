#include "parser.h"
#include "lexer.h"
#include "parse_tree_nodes.h"
#include <iostream>
#include <map>
#include <stdlib.h>

using namespace std;

int nextToken = 0;

static int level = 0;

extern bool printParse;

std::map<std::string, float> symbolTable;

string psp(void) {
  string str("");
  for (int i = 0; i < level; i++)
    str += "|  ";
  return str;
}
void output(string what) {
  if (printParse)
    cout << psp() << "found |" << yytext << "| " << what << endl;
}

void parse_log(string what) {
  if (printParse)
    std::cout << psp() << what << "\n";
}

ProgramNode *program() {
  if (nextToken != TOK_PROGRAM) // Check for PROGRAM
    throw "3: 'PROGRAM' expected";

  ProgramNode *new_program = new ProgramNode();

  output("PROGRAM");
  parse_log("enter <program>");
  ++level;

  nextToken = yylex();
  if (nextToken != TOK_IDENT) {
    throw("2: identifier expected");
  }
  output("IDENTIFIER");
  nextToken = yylex();
  if (nextToken != TOK_SEMICOLON) {
    throw("14: ';' expected");
  }
  output("SEMICOLON");

  nextToken = yylex();

  if (nextToken != TOK_BEGIN && nextToken != TOK_VAR)
    throw("<block> does not start with VAR or BEGIN");

  output("BLOCK");
  new_program->program_block = block();
  --level;
  parse_log("exit <program>");

  // NOTE: get EOF
  while (nextToken != TOK_EOF)
    nextToken = yylex();

  return new_program;
}

BlockNode *block() {
  BlockNode *new_block = new BlockNode(level);
  parse_log("enter <block>");
  ++level;
  for (;;) {
    switch (nextToken) {
    case TOK_BEGIN:
      new_block->compound_stmt = compound_statement();
      break;
    case TOK_VAR:
    case TOK_END:
      break;
    case TOK_IDENT:
      declare_ident();
      break;
    default:
      nextToken = TOK_END;
      break;
    }
    if (nextToken == TOK_END)
      break;
    nextToken = yylex();
  }
  --level;
  parse_log("exit <block>");
  return new_block;
}

CompoundStatementNode *compound_statement() {
  CompoundStatementNode *new_compound = new CompoundStatementNode(level);
  output("BEGIN");
  parse_log("enter <compound_stmt>");
  ++level;
  if (nextToken != TOK_BEGIN)
    throw("17: 'BEGIN' expected");
  for (;;) {
    nextToken = yylex();
    new_compound->statement_vector.push_back(statement());
    if (nextToken == TOK_END) {
      break;
    }
    if (nextToken != TOK_SEMICOLON)
      throw("14: ';' expected");
    output("SEMICOLON");
  }

  --level;
  output("END");
  nextToken = yylex();
  parse_log("exit <compound_stmt>");
  return new_compound;
}

StatementNode *statement() {
  StatementNode *new_statement = nullptr;
  switch (nextToken) {
  case TOK_BEGIN:
    output("STATEMENT");
    new_statement = (StatementNode *)compound_statement();
    break;
  case TOK_IF:
    output("STATEMENT");
    new_statement = (StatementNode *)if_statement();
    break;
  case TOK_WHILE:
    output("STATEMENT");
    new_statement = (StatementNode *)while_statement();
    break;
  case TOK_READ:
    output("STATEMENT");
    new_statement = (StatementNode *)read();
    nextToken = yylex();
    break;
  case TOK_WRITE:
    output("STATEMENT");
    new_statement = (StatementNode *)write();
    nextToken = yylex();
    break;
  case TOK_IDENT:
    output("STATEMENT");
    new_statement = (StatementNode *)assignment();
    break;
  default:
    throw("900: illegal type of statement");
    break;
  }
  return new_statement;
}

WriteStatementNode *write() {
  WriteStatementNode *new_write = new WriteStatementNode(level);
  parse_log("enter <write>");
  ++level;
  nextToken = yylex();
  if (nextToken != TOK_OPENPAREN)
    throw("4: ')' expected");
  output("OPENPAREN");

  nextToken = yylex();
  switch (nextToken) {
  case TOK_IDENT:
    output("WRITE");
    if (symbolTable.find(yytext) != symbolTable.end()) {
      if (printParse)
        cout << psp() << yytext << "\n";
      new_write->write_text = yytext;
      new_write->is_identifier = true;
    } else {
      throw("104: identifier not declared");
    }
    break;
  case TOK_STRINGLIT:
    output("WRITE");
    if (printParse)
      cout << psp() << yytext << "\n";
    new_write->write_text = yytext;
    break;
  default:
    throw("2: identifier expected");
    break;
  }

  nextToken = yylex();
  if (nextToken != TOK_CLOSEPAREN)
    throw("4: ')' expected");
  output("CLOSEPAREN");
  --level;
  parse_log("exit <write>");
  return new_write;
}

ReadStatementNode *read() {
  ReadStatementNode *new_read = new ReadStatementNode(level);
  parse_log("enter <read>");
  ++level;
  nextToken = yylex();
  if (nextToken != TOK_OPENPAREN)
    throw("4: ')' expected");
  output("OPENPAREN");

  nextToken = yylex();
  if (nextToken != TOK_IDENT)
    throw("2: identifier expected");
  output("IDENTIFIER");

  if (symbolTable.find(yytext) != symbolTable.end()) {
    if (printParse)
      cout << psp() << yytext << "\n";
    new_read->read_text = yytext;
  } else {
    throw("104: identifier not declared");
  }

  nextToken = yylex();
  if (nextToken != TOK_CLOSEPAREN)
    throw("4: ')' expected");
  output("CLOSEPAREN");
  --level;
  parse_log("exit <read>");
  return new_read;
}

void declare_ident() {
  if (nextToken != TOK_IDENT)
    throw("2: identifier expected");
  output("IDENTIFIER");

  string iden_name(yytext);

  nextToken = yylex();
  if (nextToken != TOK_COLON)
    throw("5: ':' expected");
  output("COLON");

  string iden_type = "NONE";

  nextToken = yylex();
  switch (nextToken) {
  case TOK_REAL:
    iden_type = "REAL";
    break;
  case TOK_INTEGER:
    iden_type = "INTEGER";
    break;
  default:
    throw("10: error in type");
    break;
  }
  output("TYPE");

  nextToken = yylex();
  if (nextToken != TOK_SEMICOLON)
    throw("14: ';' expected");
  output("SEMICOLON");

  if (printParse)
    cout << psp() << "-- idName: |" << iden_name << "| idType: |" << iden_type
         << "| --\n";

  if (symbolTable.find(iden_name) != symbolTable.end())
    throw("101: identifier declared twice");
  symbolTable.emplace(iden_name, 0.0);
}

AssignmentStatementNode *assignment() {
  AssignmentStatementNode *new_assignment = new AssignmentStatementNode(level);
  parse_log("enter <assignment>");
  ++level;
  if (nextToken != TOK_IDENT)
    throw("2: identifier expected");
  output("IDENTIFIER");
  if (printParse)
    cout << psp() << yytext << "\n";
  new_assignment->identifier = yytext;

  nextToken = yylex();
  if (nextToken != TOK_ASSIGN)
    throw("51: ':=' expected");

  output("ASSIGN");

  nextToken = yylex();
  output("EXPRESSION");
  new_assignment->assignment_expr = expression();

  --level;
  parse_log("exit <assignment>");
  return new_assignment;
}

ExpressionNode *expression() {
  ExpressionNode *new_expression = new ExpressionNode(level);
  parse_log("enter <expression>");
  ++level;
  output("SIMPLE_EXP");
  new_expression->first_simple_exp = simple_exp();

  bool next_expression = true;
  switch (nextToken) {
  case TOK_LESSTHAN:
    output("LESSTHAN");
    if (printParse)
      cout << psp() << yytext << "\n";
    break;
  case TOK_EQUALTO:
    output("EQUALTO");
    if (printParse)
      cout << psp() << yytext << "\n";
    break;
  case TOK_GREATERTHAN:
    output("GREATERTHAN");
    if (printParse)
      cout << psp() << yytext << "\n";
    break;
  case TOK_NOTEQUALTO:
    output("NOTEQUALTO");
    if (printParse)
      cout << psp() << yytext << "\n";
    break;
  default:
    next_expression = false;
    break;
  }
  if (next_expression) {
    new_expression->simple_exp_operator = nextToken;
    nextToken = yylex();
    output("SIMPLE_EXP");
    new_expression->second_simple_exp = simple_exp();
  }

  --level;
  parse_log("exit <expression>");
  return new_expression;
}

SimpleExpressionNode *simple_exp() {
  SimpleExpressionNode *new_simple_exp = new SimpleExpressionNode(level);
  parse_log("enter <simple_exp>");
  ++level;
  output("TERM");
  new_simple_exp->first_term = term();

  for (;;) {
    bool exp_ended = false;
    switch (nextToken) {
    case TOK_MINUS:
      output("MINUS");
      if (printParse)
        cout << psp() << yytext << "\n";
      break;
    case TOK_PLUS:
      output("PLUS");
      if (printParse)
        cout << psp() << yytext << "\n";
      break;
    case TOK_OR:
      output("OR");
      if (printParse)
        cout << psp() << yytext << "\n";
      break;
    default:
      exp_ended = true;
      break;
    }
    if (exp_ended)
      break;
    new_simple_exp->following_operators.push_back(nextToken);
    nextToken = yylex();
    output("TERM");
    new_simple_exp->following_terms.push_back(term());
  }

  --level;
  parse_log("exit <simple_exp>");
  return new_simple_exp;
}

TermNode *term() {
  TermNode *new_term = new TermNode(level);
  parse_log("enter <term>");
  ++level;
  output("FACTOR");
  new_term->first_factor = factor();

  for (;;) {
    bool factor_ended = false;
    nextToken = yylex();
    switch (nextToken) {
    case TOK_MULTIPLY:
      output("MULTIPLY");
      if (printParse)
        cout << psp() << yytext << "\n";
      break;
    case TOK_DIVIDE:
      output("DIVIDE");
      if (printParse)
        cout << psp() << yytext << "\n";
      break;
    case TOK_AND:
      output("AND");
      if (printParse)
        cout << psp() << yytext << "\n";
      break;
    default:
      factor_ended = true;
      break;
    }
    if (factor_ended)
      break;
    new_term->following_operators.push_back(nextToken);
    nextToken = yylex();
    output("FACTOR");
    new_term->following_factors.push_back(factor());
  }

  --level;
  parse_log("exit <term>");
  return new_term;
}

FactorNode *factor() {
  FactorNode *new_factor = nullptr;
  int factor_level = level;
  parse_log("enter <factor>");
  ++level;

  switch (nextToken) {
  case TOK_FLOATLIT:
    output("FLOATLIT");
    if (printParse)
      cout << psp() << yytext << "\n";
    new_factor = (FactorNode *)new FloatFactorNode(factor_level, yytext);
    break;
  case TOK_INTLIT:
    output("INTLIT");
    if (printParse)
      cout << psp() << yytext << "\n";
    new_factor = (FactorNode *)new IntFactorNode(factor_level, yytext);
    break;
  case TOK_IDENT:
    output("IDENTIFIER");
    if (printParse)
      cout << psp() << yytext << "\n";
    if (symbolTable.find(yytext) == symbolTable.end())
      throw("104: identifier not declared");
    new_factor = (FactorNode *)new IdFactorNode(factor_level, yytext);
    break;
  case TOK_OPENPAREN:
    output("OPENPAREN");
    if (printParse)
      cout << psp() << yytext << "\n";
    nextToken = yylex();
    output("EXPRESSION");
    new_factor =
        (FactorNode *)new ExpressionFactorNode(factor_level, expression());
    if (nextToken != TOK_CLOSEPAREN)
      throw("4: ')' expected");
    output("CLOSEPAREN");
    break;
  case TOK_NOT:
    output("NOT");
    if (printParse)
      cout << psp() << yytext << "\n";
    nextToken = yylex();
    output("FACTOR");
    new_factor = (FactorNode *)new NotFactorNode(factor_level, factor());
    break;
  case TOK_MINUS:
    output("MINUS");
    if (printParse)
      cout << psp() << yytext << "\n";
    nextToken = yylex();
    output("FACTOR");
    new_factor = (FactorNode *)new MinusFactorNode(factor_level, factor());
    break;
  default:
    throw("903: illegal type of factor");
    break;
  }

  --level;
  parse_log("exit <factor>");
  return new_factor;
}

IfStatementNode *if_statement() {
  IfStatementNode *new_if = new IfStatementNode(level);
  parse_log("enter <if>");
  ++level;
  if (nextToken != TOK_IF)
    throw("999: an error has occurred");
  nextToken = yylex();
  output("EXPRESSION");
  new_if->if_expression = expression();

  --level;
  if (nextToken != TOK_THEN)
    throw("52: 'THEN' expected");
  output("THEN");
  parse_log("enter <then>");
  ++level;

  nextToken = yylex();
  new_if->then_statement = statement();

  --level;
  parse_log("exit <then>");

  if (nextToken == TOK_ELSE) {
    new_if->has_else = true;
    output("ELSE");
    parse_log("enter <else>");
    ++level;
    nextToken = yylex();
    new_if->else_statement = statement();
    --level;
    parse_log("exit <else>");
  }
  parse_log("exit <if>");
  return new_if;
}

WhileStatementNode *while_statement() {
  WhileStatementNode *new_while = new WhileStatementNode(level);
  parse_log("enter <while>");
  ++level;
  nextToken = yylex();

  output("EXPRESSION");
  new_while->while_expression = expression();

  new_while->while_statement = statement();

  --level;
  parse_log("exit <while>");
  return new_while;
}
