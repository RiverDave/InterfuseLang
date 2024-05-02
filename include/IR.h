//
// Created by David Rivera on 2024-04-30.
// Pourer's Intermediate Representation

#ifndef IR_H
#define IR_H
#include "AST.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instruction.h>
#include <map>
#include <memory>
#include <stack>




// static std::map<std::string, llvm::Value*> scopeVars;

//NOTE: Some of the code was referenced from llvm.org

llvm::Value *LogErrorV(const char *Str); 

//Represents Every block of code
class CodeGenBlock {
public:

  //TODO: Provide smart ptrs for members below
  llvm::BasicBlock* block;
  llvm::Value* return_value;
  std::map<std::string, llvm::Value*> locals;

  CodeGenBlock() : block(nullptr), return_value(nullptr), locals() {
    std::cout << "Creating new block" << std::endl;
  }
};

class CodeGenContext final {

 public:

  std::unique_ptr<llvm::LLVMContext> TheContext;
  std::stack<std::unique_ptr<CodeGenBlock>>blocks;
  std::unique_ptr<llvm::Module> TheModule;
  std::unique_ptr<llvm::IRBuilder<>> Builder;



  CodeGenContext() : blocks(){
    //Init core llvm
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("Pourer", *TheContext);
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
  }
  void emitIR(NBlock& srcRoot);
  llvm::GenericValue runCode();

  void pushNewBlock(){
    auto nblock = std::make_unique<CodeGenBlock>();
    blocks.push(std::move(nblock));
  }





};
#endif
