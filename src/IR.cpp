#include "../include/IR.h"

//Entry point for code generation
  void CodeGenContext::emitIR(NBlock& srcRoot){

    //TODO::::

  }

// Helper
llvm::Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

//CODE GEN STUFF:
// TODO: Figure out what to do with unused param(Might not be needed at all).

llvm::Value *NDouble::codeGen(CodeGenContext &context) {
  std::cout << "Generating code for double: " << value << std::endl;
  return llvm::ConstantFP::get(*context.TheContext, llvm::APFloat(this->value));
}

llvm::Value *NInteger::codeGen(CodeGenContext &context) {
  std::cout << "Generating code for Integer: " << value << std::endl;
  // return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*TheContext), value,
  // true);
  return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context.TheContext), value,
                                true);
}

llvm::Value *NBlock::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NIdentifier::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NExpressionStatement::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NReturnStatement::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NWhileStatement::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NForStatement::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NFnDeclaration::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NBinaryOperator::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NUnaryOperator::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NFnCall::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NString::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NVariableDeclaration::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NAssignment::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NIfStatement::codeGen(CodeGenContext &context) { return nullptr; }
llvm::Value *NElseStatement::codeGen(CodeGenContext &context) { return nullptr; }
