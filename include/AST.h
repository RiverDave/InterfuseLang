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
typedef std::vector<std::shared_ptr<NStatement>> StatementList;
typedef std::vector<std::shared_ptr<NExpression>> ExpressionList;
typedef std::vector<std::shared_ptr<NVariableDeclaration>> VariableList;

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
    std::shared_ptr<NExpression> expression;

    explicit NExpressionStatement(std::shared_ptr<NExpression> expression)
        : expression(expression) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NReturnStatement : public NStatement {
public:
    std::shared_ptr<NExpression> expression;// Optional since it could be a void function

    explicit NReturnStatement(std::shared_ptr<NExpression> expression = nullptr)
        : expression(expression) {}
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

// NOTE: Not used yet...
class NWhileStatement : public NStatement {
public:
    std::shared_ptr<NExpression> condition;
    std::shared_ptr<NBlock> loopBlock;

    NWhileStatement(std::shared_ptr<NExpression> condition, std::shared_ptr<NBlock> loopBlock)
        : condition(condition), loopBlock(loopBlock) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NForStatement : public NStatement {
public:
    std::shared_ptr<NIdentifier> init;
    std::shared_ptr<NExpression> condition;
    std::shared_ptr<NExpression> iteration;
    std::shared_ptr<NBlock> loopBlock;

    NForStatement(std::shared_ptr<NIdentifier> initialization, std::shared_ptr<NExpression> condition,
                  std::shared_ptr<NExpression> iteration, std::shared_ptr<NBlock> loopBlock)
        : init(initialization), condition(condition),
          iteration(iteration), loopBlock(loopBlock) {}

    NForStatement(std::shared_ptr<NForStatement> forStmt)
        : init(forStmt->init), condition(forStmt->condition),
          iteration(forStmt->iteration), loopBlock(forStmt->loopBlock) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NFnDeclaration : public NStatement {
public:
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<VariableList> params;
    std::shared_ptr<NIdentifier> retType;
    std::shared_ptr<NBlock> fnBlock;// This should be optional(in theory)

    NFnDeclaration(std::shared_ptr<NIdentifier> id, std::shared_ptr<VariableList> args, std::shared_ptr<NIdentifier> type,
                   std::shared_ptr<NBlock> fnBlock = nullptr)
        : id(id), params(args), retType(type), fnBlock(fnBlock) {
        assert(args);
    }

    NFnDeclaration(std::shared_ptr<NFnDeclaration> fnDecl)
        : id(fnDecl->id), params(fnDecl->params), retType(fnDecl->retType), fnBlock(fnDecl->fnBlock) {
    }

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
    std::shared_ptr<NExpression> lhs;
    std::shared_ptr<Token> op;
    std::shared_ptr<NExpression> rhs;

    explicit NBinaryOperator(std::shared_ptr<NExpression> lhs, std::shared_ptr<Token> op, std::shared_ptr<NExpression> rhs)
        : lhs(lhs), op(op), rhs(rhs) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NUnaryOperator : public NExpression {
public:
    std::shared_ptr<Token> op;
    std::shared_ptr<NExpression> exp;

    explicit NUnaryOperator(std::shared_ptr<Token> op, std::shared_ptr<NExpression> operand)
        : op(op), exp(operand) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NFnCall : public NExpression {
public:
    const std::shared_ptr<NIdentifier> id;
    std::shared_ptr<ExpressionList> arguments;

    NFnCall(std::shared_ptr<NIdentifier> id, std::shared_ptr<ExpressionList> args)
        : id(id), arguments(args) {}

    // No args func call
    NFnCall(std::shared_ptr<NIdentifier> id) : id(id) {}

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
    std::shared_ptr<NIdentifier> type;// Could be automatically infered
    std::shared_ptr<NIdentifier> id;
    std::shared_ptr<NExpression> assignmentExpr;

    NVariableDeclaration(std::shared_ptr<NIdentifier> type, std::shared_ptr<NIdentifier> id,
                         std::shared_ptr<NExpression> assignmentExpr = nullptr)
        : type(type), id(id), assignmentExpr(assignmentExpr) {}

    //Unusual constructor, used solely for bison
    NVariableDeclaration(std::shared_ptr<NVariableDeclaration> varDecl)
        : type(varDecl->type), id(varDecl->id), assignmentExpr(varDecl->assignmentExpr) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

// Assignment expr
class NAssignment : public NExpression {

public:
    std::shared_ptr<NIdentifier> lhs;
    std::shared_ptr<NExpression> rhs;
    NAssignment(std::shared_ptr<NIdentifier> id, std::shared_ptr<NExpression> assignmentExpr)
        : lhs(id), rhs(assignmentExpr) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NElseStatement : public NStatement {
public:
    std::shared_ptr<NBlock> block;

    explicit NElseStatement(std::shared_ptr<NBlock> block) : block(block) {}

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIfStatement : public NStatement {
public:
    std::shared_ptr<NExpression> condition;
    std::shared_ptr<NBlock> trueBlock;
    std::shared_ptr<NElseStatement> falseBlock;// Optional, may be nullptr
                                               //
    NIfStatement(std::shared_ptr<NExpression> condition, std::shared_ptr<NBlock> trueBlock,
                 std::shared_ptr<NElseStatement> falseBlock = nullptr)
        : condition(condition), trueBlock(trueBlock), falseBlock(falseBlock) {}

    NIfStatement(std::shared_ptr<NIfStatement> ifStmt)
        : condition(ifStmt->condition), trueBlock(ifStmt->trueBlock), falseBlock(ifStmt->falseBlock) {}

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

inline std::shared_ptr<Node> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

#endif// AST_H
