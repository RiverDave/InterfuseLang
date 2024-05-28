//
// Created by David Rivera on 2024-04-20.
// This file represents Pourer's AST/Parse Tree
//
#ifndef AST_H
#define AST_H

#include "Token.h"
#include <llvm/IR/Value.h>
#include <vector>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement *> StatementList;
typedef std::vector<NExpression *> ExpressionList;
typedef std::vector<NVariableDeclaration *> VariableList;

class Node {
public:
  // Will be used in each node to then be export IR to bytecode through llvm's
  // API
  virtual ~Node() {}
  virtual llvm::Value *codeGen(CodeGenContext &context) { return NULL; }
};

class NExpression : public Node {};

class NStatement : public Node {};

class NBlock : public NExpression {
public:
  StatementList statements;

  NBlock() = default;

  virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIdentifier : public NExpression {
public:
  std::string name;

  explicit NIdentifier(const std::string &name) : name(std::move(name)) {}
  llvm::Type *getType(CodeGenContext &context) const;

  std::string getName() const { return name; }
  virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExpressionStatement : public NStatement {
public:
  NExpression &expression;

  explicit NExpressionStatement(NExpression &expression)
      : expression(expression) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NReturnStatement : public NStatement {
public:
  NExpression *expression; // Optional since it could be a void function

  explicit NReturnStatement(NExpression *expression = nullptr)
      : expression(expression) {}
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

// NOTE: Not used yet...
class NWhileStatement : public NStatement {
public:
  NExpression *condition;
  NBlock *loopBlock;

  NWhileStatement(NExpression *condition, NBlock *loopBlock)
      : condition(condition), loopBlock(loopBlock) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NForStatement : public NStatement {
public:
  NIdentifier *initialization; // This is an id 
  NExpression *condition;
  NExpression *iteration;
  NBlock *loopBlock;

  NForStatement(NIdentifier *initialization, NExpression *condition,
                NExpression *iteration, NBlock *loopBlock)
      : initialization(initialization), condition(condition),
        iteration(iteration), loopBlock(loopBlock) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NFnDeclaration : public NStatement {
public:
  NIdentifier &id; // NOTE: This could be replaced for a var declaration for
                   // default parameters in functions
  VariableList params;
  NIdentifier &retType;
  NBlock *fnBlock; // This should be optional(in theory)

  NFnDeclaration(NIdentifier &id, VariableList &args, NIdentifier &type,
                 NBlock *fnBlock = nullptr)
      : id(id), params(args), retType(type), fnBlock(fnBlock) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NInteger : public NExpression {
public:
  long long value;

  explicit NInteger(long long value) : value(value) {}
  explicit NInteger() : value(0) {} // empty int declaration

  // for testing purposes only
  long long getValue() const { return value; }
  virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NDouble : public NExpression {
public:
  double value;

  explicit NDouble(double value) : value(value) {}

  // for testing purposes only
  double getValue() const { return value; }

  virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBinaryOperator : public NExpression {
public:
  NExpression &lhs;
  Token &op; // doubtful 'bout this
  NExpression &rhs;

  explicit NBinaryOperator(NExpression &lhs, Token &op, NExpression &rhs)
      : lhs(lhs), op(op), rhs(rhs) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NUnaryOperator : public NExpression {
public:
  Token &op;
  NExpression &operand;

  explicit NUnaryOperator(Token &op, NExpression &operand)
      : op(op), operand(operand) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NFnCall : public NExpression {
public:
  const NIdentifier &id;
  ExpressionList arguments;

  NFnCall(const NIdentifier &id, ExpressionList &args)
      : id(id), arguments(args) {}

  // No args func call
  NFnCall(const NIdentifier &id) : id(id) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NString : public NExpression {
  //   std::string name;

  // It would be better to allocate memory on the heap for this member
  // sm_ptr or shared?
  std::string value;

  explicit NString(const std::string &value) : value(value) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NVariableDeclaration : public NStatement {
public:
  const NIdentifier &type; // Could be automatically infered
  NIdentifier *id;
  // TODO: assignment shall be optional!!, include type as well()
  NExpression *assignmentExpr;

  NVariableDeclaration(const NIdentifier &type, NIdentifier *id,
                       NExpression *assignmentExpr = nullptr)
      : type(type), id(id), assignmentExpr(assignmentExpr) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

// Assignment expr
class NAssignment : public NExpression {

public:
  NIdentifier &lhs;
  NExpression &rhs;
  NAssignment(NIdentifier &id, NExpression &assignmentExpr)
      : lhs(id), rhs(assignmentExpr) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NElseStatement : public NStatement {
public:
  NBlock *block;

  explicit NElseStatement(NBlock *block) : block(block) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NIfStatement : public NStatement {
public:
  NExpression *condition;
  const NBlock *trueBlock;
  const NElseStatement *falseBlock; // Optional, may be nullptr
  NIfStatement(NExpression *condition, const NBlock *trueBlock,
               const NElseStatement *falseBlock = nullptr)
      : condition(condition), trueBlock(trueBlock), falseBlock(falseBlock) {}

  virtual llvm::Value *codeGen(CodeGenContext &context);
};

inline std::unique_ptr<Node> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

#endif // AST_H
