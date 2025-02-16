#ifndef PARSE_TREE_NODES_H
#define PARSE_TREE_NODES_H

#include "lexer.h"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

class ProgramNode;
class BlockNode;

class StatementNode;
class CompoundStatementNode;
class WriteStatementNode;
class ReadStatementNode;
class AssignmentStatementNode;
class IfStatementNode;
class WhileStatementNode;

class ExpressionNode;
class SimpleExpressionNode;

class TermNode;
class FactorNode;

class FloatFactorNode;
class IdFactorNode;
class IntFactorNode;
class MinusFactorNode;
class NotFactorNode;
class ExpressionFactorNode;

class ProgramNode {
public:
  BlockNode *program_block = nullptr;

  ProgramNode();
  ~ProgramNode();
  float interpret();
};
std::ostream &operator<<(std::ostream &, ProgramNode &);

class BlockNode {
public:
  CompoundStatementNode *compound_stmt = nullptr;
  int _level = 0;

  BlockNode(int level);
  ~BlockNode();
  float interpret();
};
std::ostream &operator<<(std::ostream &, BlockNode &);

class StatementNode {
public:
  int _level = 0;
  StatementNode();
  virtual ~StatementNode();
  virtual void printTo(std::ostream &os) = 0;
  virtual float interpret() = 0;
};

class CompoundStatementNode : public StatementNode {
public:
  int _level = 0;
  std::vector<StatementNode *> statement_vector;
  CompoundStatementNode(int level);
  ~CompoundStatementNode();
  void printTo(std::ostream &os);
  float interpret();
};

class WriteStatementNode : public StatementNode {
public:
  int _level = 0;
  bool is_identifier = false;
  std::string write_text;
  WriteStatementNode(int level);
  ~WriteStatementNode();
  void printTo(std::ostream &os);
  float interpret();
};

class ReadStatementNode : public StatementNode {
public:
  int _level = 0;
  std::string read_text;
  ReadStatementNode(int level);
  ~ReadStatementNode();
  void printTo(std::ostream &os);
  float interpret();
};

class IfStatementNode : public StatementNode {
public:
  int _level = 0;
  ExpressionNode *if_expression = nullptr;
  StatementNode *then_statement = nullptr;
  bool has_else = false;
  StatementNode *else_statement = nullptr;
  IfStatementNode(int level);
  ~IfStatementNode();
  void printTo(std::ostream &os);
  float interpret();
};

class WhileStatementNode : public StatementNode {
public:
  int _level = 0;
  ExpressionNode *while_expression = nullptr;
  StatementNode *while_statement = nullptr;
  WhileStatementNode(int level);
  ~WhileStatementNode();
  void printTo(std::ostream &os);
  float interpret();
};

class AssignmentStatementNode : public StatementNode {
public:
  int _level = 0;
  std::string identifier;
  ExpressionNode *assignment_expr = nullptr;
  AssignmentStatementNode(int level);
  ~AssignmentStatementNode();
  void printTo(std::ostream &os);
  float interpret();
};

class ExpressionNode {
public:
  int _level = 0;
  int simple_exp_operator = TOK_UNKNOWN;
  SimpleExpressionNode *first_simple_exp = nullptr;
  SimpleExpressionNode *second_simple_exp = nullptr;

  ExpressionNode(int level);
  ~ExpressionNode();
  void printTo(std::ostream &os);
  float interpret();
};

class SimpleExpressionNode {
public:
  int _level = 0;
  TermNode *first_term = nullptr;
  std::vector<int> following_operators;
  std::vector<TermNode *> following_terms;

  SimpleExpressionNode(int level);
  ~SimpleExpressionNode();
  void printTo(std::ostream &os);
  float interpret();
};

class TermNode {
public:
  int _level = 0;
  FactorNode *first_factor = nullptr;
  std::vector<int> following_operators;
  std::vector<FactorNode *> following_factors;

  TermNode(int level);
  ~TermNode();
  void printTo(std::ostream &os);
  float interpret();
};

class FactorNode {
public:
  int _level = 0;

  FactorNode();
  virtual ~FactorNode();
  virtual void printTo(std::ostream &os) = 0;
  virtual float interpret() = 0;
};

class FloatFactorNode : public FactorNode {
public:
  int _level = 0;
  float float_literal = 0.0;
  FloatFactorNode(int level, std::string float_str);
  ~FloatFactorNode();
  void printTo(std::ostream &os);
  float interpret();
};

class IdFactorNode : public FactorNode {
public:
  int _level = 0;
  std::string identifier = "";
  IdFactorNode(int level, std::string ident);
  ~IdFactorNode();
  void printTo(std::ostream &os);
  float interpret();
};

class IntFactorNode : public FactorNode {
public:
  int _level = 0;
  std::string int_literal = "";
  IntFactorNode(int level, std::string lit);
  ~IntFactorNode();
  void printTo(std::ostream &os);
  float interpret();
};

class MinusFactorNode : public FactorNode {
public:
  int _level = 0;
  FactorNode *child_factor = nullptr;
  MinusFactorNode(int level, FactorNode *child);
  ~MinusFactorNode();
  void printTo(std::ostream &os);
  float interpret();
};

class NotFactorNode : public FactorNode {
public:
  int _level = 0;
  FactorNode *child_factor = nullptr;
  NotFactorNode(int level, FactorNode *child);
  ~NotFactorNode();
  void printTo(std::ostream &os);
  float interpret();
};

class ExpressionFactorNode : public FactorNode {
public:
  int _level = 0;
  ExpressionNode *child_expression = nullptr;
  ExpressionFactorNode(int level, ExpressionNode *child);
  ~ExpressionFactorNode();
  void printTo(std::ostream &os);
  float interpret();
};

#endif /* PARSE_TREE_NODES_H */
