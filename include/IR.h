//
// Created by David Rivera on 2024-04-30.
// Pourer's Intermediate Representation through LLVM

#ifndef IR_H
#define IR_H
#include "AST.h"
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Type.h>

#include <map>
#include <memory>
#include <stack>

// static std::map<std::string, llvm::Value*> scopeVars;

// NOTE: Some of the code was referenced from llvm.org

llvm::Value *LogErrorV(const char *Str);

// Represents Every block of code
// Wrapper around basic blocks
class CodeGenBlock {
public:
  llvm::BasicBlock *blockWrapper;
  llvm::Value *return_value;
  // Allocainst stores memory block occupied by that variable
  std::map<std::string, llvm::AllocaInst *> locals;

  CodeGenBlock() : blockWrapper(nullptr), return_value(nullptr), locals() {}
};

//Its purpose is to mainly store relevant information
//necessary to call a function
struct fnInfo {
  llvm::Value* fn;
  llvm::Type* retType;
  llvm::FunctionType* fnType;
};

class CodeGenContext {

public:
  std::unique_ptr<llvm::LLVMContext> TheContext;
  std::stack<std::unique_ptr<CodeGenBlock>> blocks;
  std::unique_ptr<llvm::Module> TheModule;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::map<std::string, fnInfo> globals; //will store Functions & global vaiables

  llvm::Function *globalFn; // Entry point function aka 'Main'

  CodeGenContext() : blocks() {
    // Init core llvm
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("Pourer", *TheContext);
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    globalFn = nullptr;
  }
  void emitIR(NBlock &srcRoot);
  void setTarget();
  llvm::GenericValue runCode();

  llvm::Function *genPrototype();

  // Insert memory block on fn block(used to instantiate variables)
  llvm::AllocaInst *insertMemOnFnBlock(llvm::Function *fn, std::string &id,
                                       llvm::Type *);

  void pushBlock(llvm::BasicBlock *block) {
    blocks.push(std::make_unique<CodeGenBlock>());
    blocks.top()->return_value = nullptr;
    blocks.top()->blockWrapper = block;
  }

  void popBlock() { blocks.pop(); }
};

#endif
