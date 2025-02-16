#define EPSILON 0.001

#include "parse_tree_nodes.h"
#include "lexer.h"
#include "parser.h"
#include <ostream>

extern std::map<std::string, float> symbolTable;

static void indent(int level) {
  for (int i = 0; i < level; i++)
    std::cout << ("|  ");
}

ProgramNode::ProgramNode() {}
ProgramNode::~ProgramNode() { delete program_block; }

std::ostream &operator<<(std::ostream &os, ProgramNode &pn) {
  os << std::endl;
  os << "(program ";
  os << *(pn.program_block);
  os << std::endl;
  os << "program) ";
  return os;
}
float ProgramNode::interpret() { return program_block->interpret(); }

BlockNode::BlockNode(int level) { _level = level; }
BlockNode::~BlockNode() {}

std::ostream &operator<<(std::ostream &os, BlockNode &bn) {
  os << std::endl;
  indent(bn._level);
  os << "(block ";
  os << std::endl;
  bn.compound_stmt->printTo(os);
  os << std::endl;
  indent(bn._level);
  os << "block) ";
  return os;
}

float BlockNode::interpret() { return compound_stmt->interpret(); }

CompoundStatementNode::CompoundStatementNode(int level) { _level = level; }
CompoundStatementNode::~CompoundStatementNode() {
  for (auto it = statement_vector.begin(); it != statement_vector.end(); ++it)
    delete (*it);
}

void CompoundStatementNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(compound_stmt ";
  os << std::endl;
  for (auto it = statement_vector.begin(); it != statement_vector.end(); ++it) {
    (*it)->printTo(os);
    os << std::endl;
  }
  indent(_level);
  os << "compound_stmt) ";
}

float CompoundStatementNode::interpret() {
  float result = 0.0;
  for (auto it = statement_vector.begin(); it != statement_vector.end(); ++it)
    result = (*it)->interpret();

  return result;
}

StatementNode::StatementNode() {}
StatementNode::~StatementNode() {}

WriteStatementNode::WriteStatementNode(int level) { _level = level; }
WriteStatementNode::~WriteStatementNode() {}

void WriteStatementNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(write_stmt ( " << write_text << " ) \n";
  indent(_level);
  os << "write_stmt) ";
}

float WriteStatementNode::interpret() {
  if (is_identifier) {
    auto var = symbolTable.find(write_text);
    if (var == symbolTable.end())
      throw("Write failed: variable not found");
    cout << var->second << "\n";
    return 0.0;
  }
  std::cout << write_text << "\n";
  return 0.0;
}

ReadStatementNode::ReadStatementNode(int level) { _level = level; }
ReadStatementNode::~ReadStatementNode() {}

void ReadStatementNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(read_stmt ( " << read_text << " ) \n";
  indent(_level);
  os << "read_stmt) ";
}

float ReadStatementNode::interpret() {
  std::string input;
  cin >> input;
  auto var = symbolTable.find(read_text);
  if (var == symbolTable.end())
    throw("Read failed: identifier not found");
  var->second = std::stof(input);
  return var->second;
}

AssignmentStatementNode::AssignmentStatementNode(int level) { _level = level; }
AssignmentStatementNode::~AssignmentStatementNode() { delete assignment_expr; }

void AssignmentStatementNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(assignment_stmt ( " << identifier << " := ) \n";
  assignment_expr->printTo(os);
  os << std::endl;
  indent(_level);
  os << "assignment_stmt) ";
}

float AssignmentStatementNode::interpret() {
  auto var = symbolTable.find(identifier);
  if (var == symbolTable.end())
    throw("Variable assignment failed: Variable not found");
  var->second = assignment_expr->interpret();
  return var->second;
}

IfStatementNode::IfStatementNode(int level) { _level = level; }
IfStatementNode::~IfStatementNode() {
  delete if_expression;
  delete then_statement;
  delete else_statement;
}

void IfStatementNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(if_stmt \n";
  if_expression->printTo(os);
  os << std::endl;
  indent(_level);
  os << "(then ";
  os << std::endl;
  then_statement->printTo(os);
  os << std::endl;
  indent(_level);
  os << "then) \n";
  if (has_else) {
    indent(_level);
    os << "(else ";
    os << std::endl;
    else_statement->printTo(os);
    os << std::endl;
    indent(_level);
    os << "else) \n";
  }
  indent(_level);
  os << "if_stmt) ";
}

float IfStatementNode::interpret() {
  if (if_expression->interpret() > EPSILON)
    return then_statement->interpret();
  else if (has_else)
    return else_statement->interpret();
  return 0.0;
}

WhileStatementNode::WhileStatementNode(int level) { _level = level; }
WhileStatementNode::~WhileStatementNode() {
  delete while_statement;
  delete while_expression;
}

void WhileStatementNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(while_stmt ";
  os << std::endl;
  while_expression->printTo(os);
  os << std::endl;
  while_statement->printTo(os);
  os << std::endl;
  indent(_level);
  os << "while_stmt) ";
}

float WhileStatementNode::interpret() {
  float result = 0.0;
  while (while_expression->interpret() == 1.0)
    result = while_statement->interpret();
  return result;
}

ExpressionNode::ExpressionNode(int level) { _level = level; }
ExpressionNode::~ExpressionNode() {}

void ExpressionNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(expression \n";
  first_simple_exp->printTo(os);
  os << std::endl;
  if (simple_exp_operator != TOK_UNKNOWN) {
    indent(_level);
    switch (simple_exp_operator) {
    case TOK_LESSTHAN:
      os << "< ";
      break;
    case TOK_GREATERTHAN:
      os << "> ";
      break;
    case TOK_EQUALTO:
      os << "= ";
      break;
    case TOK_NOTEQUALTO:
      os << "<> ";
      break;
    default:
      break;
    }
    os << std::endl;
    second_simple_exp->printTo(os);
    os << std::endl;
  }
  indent(_level);
  os << "expression) ";
}

float ExpressionNode::interpret() {
  float first_exp_result = first_simple_exp->interpret();
  if (simple_exp_operator != TOK_UNKNOWN) {
    float second_exp_result = second_simple_exp->interpret();
    switch (simple_exp_operator) {
    case TOK_LESSTHAN:
      if (first_exp_result - second_exp_result < 0.0)
        return 1.0;
      else
        return 0.0;
    case TOK_GREATERTHAN:
      if (first_exp_result - second_exp_result >= EPSILON)
        return 1.0;
      else
        return 0.0;
      break;
    case TOK_EQUALTO:
      if (std::abs(first_exp_result - second_exp_result) <= EPSILON)
        return 1.0;
      else
        return 0.0;
      break;
    case TOK_NOTEQUALTO:
      if (std::abs(first_exp_result - second_exp_result) > EPSILON)
        return 1.0;
      else
        return 0.0;
      break;
    default:
      break;
    }
  }
  return first_exp_result;
}

SimpleExpressionNode::SimpleExpressionNode(int level) { _level = level; }
SimpleExpressionNode::~SimpleExpressionNode() {
  delete first_term;
  for (auto it = following_terms.begin(); it != following_terms.end(); ++it)
    delete (*it);
}

void SimpleExpressionNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(simple_exp \n";
  first_term->printTo(os);
  os << std::endl;
  for (unsigned int i = 0; i < following_operators.size(); i++) {
    indent(_level);
    switch (following_operators[i]) {
    case TOK_PLUS:
      os << "+ ";
      break;
    case TOK_MINUS:
      os << "- ";
      break;
    case TOK_OR:
      os << "OR ";
      break;
    default:
      break;
    }
    os << std::endl;
    following_terms[i]->printTo(os);
    os << std::endl;
  }
  indent(_level);
  os << "simple_exp) ";
}

float SimpleExpressionNode::interpret() {
  float result = first_term->interpret();
  for (unsigned int i = 0; i < following_operators.size(); i++) {
    switch (following_operators[i]) {
    case TOK_PLUS:
      result += following_terms[i]->interpret();
      break;
    case TOK_MINUS:
      result -= following_terms[i]->interpret();
      break;
    case TOK_OR:
      result = (result >= EPSILON) ? result >= EPSILON
                                   : following_terms[i]->interpret() >= EPSILON;
      break;
    default:
      break;
    }
  }
  return result;
}

TermNode::TermNode(int level) { _level = level; }
TermNode::~TermNode() {
  delete first_factor;
  for (auto it = following_factors.begin(); it != following_factors.end(); ++it)
    delete (*it);
}

void TermNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(term \n";
  first_factor->printTo(os);
  os << std::endl;
  for (unsigned int i = 0; i < following_operators.size(); i++) {
    indent(_level);
    switch (following_operators[i]) {
    case TOK_MULTIPLY:
      os << "* ";
      break;
    case TOK_DIVIDE:
      os << "/ ";
      break;
    case TOK_AND:
      os << "AND ";
      break;
    default:
      break;
    }
    os << std::endl;
    following_factors[i]->printTo(os);
    os << std::endl;
  }
  indent(_level);
  os << "term) ";
}

float TermNode::interpret() {
  float result = first_factor->interpret();
  for (unsigned int i = 0; i < following_operators.size(); i++) {
    switch (following_operators[i]) {
    case TOK_MULTIPLY:
      result *= following_factors[i]->interpret();
      break;
    case TOK_DIVIDE:
      result /= following_factors[i]->interpret();
      break;
    case TOK_AND:
      if (result >= EPSILON && following_factors[i]->interpret() >= EPSILON)
        result = 1.0;
      else
        result = 0.0;
      break;
    default:
      break;
    }
  }
  return result;
}

FactorNode::FactorNode() {}
FactorNode::~FactorNode() {}

FloatFactorNode::FloatFactorNode(int level, std::string float_str) {
  _level = level;
  float_literal = std::stof(float_str);
}
FloatFactorNode::~FloatFactorNode() {}

void FloatFactorNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(factor ( FLOATLIT: " << float_literal << " ) \n";
  indent(_level);
  os << "factor) ";
}

float FloatFactorNode::interpret() { return float_literal; }

IntFactorNode::IntFactorNode(int level, std::string lit) {
  _level = level;
  int_literal = lit;
}
IntFactorNode::~IntFactorNode() {}

void IntFactorNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(factor ( INTLIT: " << int_literal << " ) \n";
  indent(_level);
  os << "factor) ";
}

float IntFactorNode::interpret() { return std::stof(int_literal); }

IdFactorNode::IdFactorNode(int level, std::string ident) {
  _level = level;
  identifier = ident;
}
IdFactorNode::~IdFactorNode() {}

void IdFactorNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(factor ( IDENT: " << identifier << " ) \n";
  indent(_level);
  os << "factor) ";
}

float IdFactorNode::interpret() {
  auto var = symbolTable.find(identifier);
  if (var == symbolTable.end())
    throw("Id Factor Node failed: var undefined");
  return var->second;
}

MinusFactorNode::MinusFactorNode(int level, FactorNode *child) {
  _level = level;
  child_factor = child;
}
MinusFactorNode::~MinusFactorNode() { delete child_factor; }

float MinusFactorNode::interpret() { return -child_factor->interpret(); }

void MinusFactorNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(factor (- \n";
  child_factor->printTo(os);
  os << ") ";
  os << std::endl;
  indent(_level);
  os << "factor) ";
}

NotFactorNode::NotFactorNode(int level, FactorNode *child) {
  _level = level;
  child_factor = child;
}
NotFactorNode::~NotFactorNode() { delete child_factor; }

void NotFactorNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(factor (NOT \n";
  child_factor->printTo(os);
  os << ") ";
  os << std::endl;
  indent(_level);
  os << "factor) ";
}

float NotFactorNode::interpret() {
  return child_factor->interpret() >= EPSILON ? 0.0 : 1.0;
}

ExpressionFactorNode::ExpressionFactorNode(int level, ExpressionNode *child) {
  _level = level;
  child_expression = child;
}
ExpressionFactorNode::~ExpressionFactorNode() { delete child_expression; }

void ExpressionFactorNode::printTo(std::ostream &os) {
  indent(_level);
  os << "(factor ( \n";
  child_expression->printTo(os);
  os << ") ";
  os << std::endl;
  indent(_level);
  os << "factor) ";
}

float ExpressionFactorNode::interpret() {
  return child_expression->interpret();
}
