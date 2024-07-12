//
// Created by David Rivera on 2024-04-20.
// This file represents Pourer's AST/Parse Tree
//
#ifndef AST_H
#define AST_H

#include "Token.h"
#include <llvm/IR/Value.h>
#include <memory>
#include <vector>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

//ptr to vector of ptr of Node objects
typedef std::vector<std::unique_ptr<NStatement>>StatementList;
typedef std::vector<std::unique_ptr<NExpression>> ExpressionList;
typedef std::vector<std::unique_ptr<NVariableDeclaration>> VariableList;

class Node {
public:
    // Will be used in each node to then be export IR to bytecode through llvm's
    // API
    virtual ~Node() {}
    virtual llvm::Value *codeGen(CodeGenContext &context) { return NULL; }
};

class NExpression : public Node {};

class NStatement : public Node {
public:
    virtual std::string getStatementType() { return "NStatement"; }
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() = default;

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIdentifier : public NExpression {
public:
    std::string name;

    explicit NIdentifier(const std::string name) : name(name) {}
    llvm::Type *getType(CodeGenContext &context) const;//If applies

    std::string getName() const { return name; }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExpressionStatement : public NStatement {
public:
    std::unique_ptr<NExpression> expression;

    explicit NExpressionStatement(std::unique_ptr<NExpression>expression)
        : expression(std::move(expression)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NReturnStatement : public NStatement {
public:
    std::unique_ptr<NExpression> expression;// Optional since it could be a void function

    explicit NReturnStatement(std::unique_ptr<NExpression>expression = nullptr)
        : expression(std::move(expression)) {}
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

// NOTE: Not used yet...
class NWhileStatement : public NStatement {
public:
    std::unique_ptr<NExpression> condition;
    std::unique_ptr<NBlock> loopBlock;

    NWhileStatement(std::unique_ptr<NExpression>condition, std::unique_ptr<NBlock>loopBlock)
        : condition(std::move(condition)), loopBlock(std::move(loopBlock)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NForStatement : public NStatement {
public:
    std::unique_ptr<NIdentifier> init;
    std::unique_ptr<NExpression> condition;
    std::unique_ptr<NExpression> iteration;
    std::unique_ptr<NBlock> loopBlock;

    NForStatement(std::unique_ptr<NIdentifier>initialization, std::unique_ptr<NExpression>condition,
                  std::unique_ptr<NExpression>iteration, std::unique_ptr<NBlock>loopBlock)
        : init(std::move(initialization)), condition(std::move(condition)),
          iteration(std::move(iteration)), loopBlock(std::move(loopBlock)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NFnDeclaration : public NStatement {
public:
    std::unique_ptr<NIdentifier> id;
    std::unique_ptr<VariableList> params;
    std::unique_ptr<NIdentifier> retType;
    std::unique_ptr<NBlock> fnBlock;// This should be optional(in theory)

    NFnDeclaration(std::unique_ptr<NIdentifier>id, std::unique_ptr<VariableList>args, std::unique_ptr<NIdentifier>type,
                   std::unique_ptr<NBlock>fnBlock = nullptr)
        : id(std::move(id)), params(std::move(args)), retType(std::move(type)), fnBlock(std::move(fnBlock)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NInteger : public NExpression {
public:
    long long value;

    //Is such a big int necessary?
    explicit NInteger(long long value) : value(value) {}
    explicit NInteger() : value(0) {}// empty int declaration

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
    std::unique_ptr<NExpression> lhs;
    std::unique_ptr<Token> op;
    std::unique_ptr<NExpression> rhs;

    explicit NBinaryOperator(std::unique_ptr<NExpression> lhs, Token *op, std::unique_ptr<NExpression> rhs)
        : lhs(std::move(lhs)), op(op), rhs(std::move(rhs)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NUnaryOperator : public NExpression {
public:
    std::unique_ptr<Token> op;
    std::unique_ptr<NExpression> exp;

    explicit NUnaryOperator(Token *op, std::unique_ptr<NExpression>operand)
        : op(op), exp(std::move(operand)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NFnCall : public NExpression {
public:
    const std::unique_ptr<NIdentifier> id;
    std::unique_ptr<ExpressionList> arguments;

    NFnCall(std::unique_ptr<NIdentifier> id, std::unique_ptr<ExpressionList> args)
        : id(std::move(id)), arguments(std::move(args)) {}

    // No args func call
    NFnCall(std::unique_ptr<NIdentifier> id) : id(std::move(id)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NString : public NExpression {

public:
    std::string value;
    explicit NString(const std::string &value) : value(value) {}
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NVariableDeclaration : public NStatement {
public:
    std::unique_ptr<NIdentifier> type;// Could be automatically infered
    std::unique_ptr<NIdentifier> id;
    std::unique_ptr<NExpression> assignmentExpr;

    NVariableDeclaration(std::unique_ptr<NIdentifier> type, std::unique_ptr<NIdentifier> id,
                         std::unique_ptr<NExpression> assignmentExpr = nullptr)
        : type(std::move(type)), id(std::move(id)), assignmentExpr(std::move(assignmentExpr)) {}

    //Unusual constructor, used solely for bison
    NVariableDeclaration(std::unique_ptr<NVariableDeclaration> varDecl)
        : type(std::move(varDecl->type)), id(std::move(varDecl->id)), assignmentExpr(std::move(varDecl->assignmentExpr)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

// Assignment expr
class NAssignment : public NExpression {

public:
    std::unique_ptr<NIdentifier> lhs;
    std::unique_ptr<NExpression> rhs;
    NAssignment(std::unique_ptr<NIdentifier> id, std::unique_ptr<NExpression> assignmentExpr)
        : lhs(std::move(id)), rhs(std::move(assignmentExpr)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NElseStatement : public NStatement {
public:
    std::unique_ptr<NBlock> block;

    explicit NElseStatement(std::unique_ptr<NBlock> block) : block(std::move(block)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIfStatement : public NStatement {
public:
  std::unique_ptr<NExpression> condition;
  std::unique_ptr<NBlock> trueBlock;
  std::unique_ptr<NElseStatement> falseBlock;// Optional, may be nullptr
                                             //
    NIfStatement(std::unique_ptr<NExpression> condition, std::unique_ptr<NBlock>trueBlock,
                 std::unique_ptr<NElseStatement>falseBlock = nullptr)
        : condition(std::move(condition)), trueBlock(std::move(trueBlock)), falseBlock(std::move(falseBlock)) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBreakStatement : public NStatement {
public:
    NBreakStatement() = default;
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
    //Really clunky solution, Im aware. ideally one should check the type
    //of a derived class by casting but it didn't work in my case...
    virtual std::string getStatementType() override { return "NBreakStatement"; }
};

class NContinueStatement : public NStatement {
public:
    NContinueStatement() = default;
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

inline std::unique_ptr<Node> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

#endif// AST_H
