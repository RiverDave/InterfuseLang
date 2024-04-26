//
// Created by David Rivera on 2024-04-20.
// This file represents Pourer's AST/Parse Tree
//

#include <vector>
//#include <llvm/IR/Value.h>


#ifndef AST_H
#define AST_H

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node
{
public:
    virtual ~Node(){}

    // Will be used in each node to then be export IR to bytecode through llvm's API
    //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpression : public Node
{
};

class NStatement
{
};

class NBlock : public NExpression
{
public:
    StatementList statements;
    NBlock(){}
    //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement
{
public:
    NExpression& expression;
    explicit NExpressionStatement(NExpression& expression) : expression(expression){}
    //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement {
public:
	NExpression& expression;
	explicit NReturnStatement(NExpression& expression) : 
		expression(expression) { }
	// virtual llvm::Value* codeGen(CodeGenContext& context);
};



class NInteger : public NExpression
{
public:
    long long value;
    explicit NInteger(long long value) : value(value){}

    //for testing purposes only
    long long getValue() const { return value; }
    //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression
{
public:
    std::string name;
    explicit NIdentifier(const std::string &name) :name(name){}
    //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NString : public  NExpression
{
 //   std::string name;

    //It would be better to allocate memory on the heap for this member
    //sm_ptr or shared?
    std::string value;
    explicit NString(const std::string &value) :value(value){}
    //virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public Node
{

public:

    NIdentifier* id;
    //TODO: assignment shall be optional!!, include type as well
    NExpression* assignmentExpr;
    NVariableDeclaration(NIdentifier* id, NExpression* assignmentExpr)
        : id(id), assignmentExpr(assignmentExpr) {}
    //virtual llvm::Value* codeGen(CodeGenContext& context);

};






#endif //AST_H
