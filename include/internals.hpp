// This module defines the core functions supported in Interfuse
// David Rivera - 14/05/2024

#include "IR.h"
#include "intercore.hpp"
#include <cstdio>
#include <llvm/IR/Type.h>

#ifndef INTERNALS_HPP
#define INTERNALS_HPP

//Taken from toycompiler for now by ISEGAL

using namespace llvm;

extern void printi(long long val);

llvm::Function *createCPrint(CodeGenContext &context) {
    llvm::Type *int8Type = llvm::IntegerType::get(*context.TheContext, 8);
    PointerType *pointerToI8 = llvm::PointerType::get(int8Type, 0);

    std::vector<llvm::Type *> printf_arg_types;
    printf_arg_types.push_back(pointerToI8);//char*

    llvm::FunctionType *printf_type =
            llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*context.TheContext), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
            printf_type, llvm::Function::ExternalLinkage,
            llvm::Twine("printf"), //Reference to the std printf
            *context.TheModule);
    func->setCallingConv(llvm::CallingConv::C);

//    func->print(llvm::errs());
    return func;
}

void createEchoFunction(CodeGenContext &context, llvm::Function *printfFn) {
    std::vector<llvm::Type *> out_arg_types;
    out_arg_types.push_back(llvm::Type::getInt64Ty(*context.TheContext));

    llvm::FunctionType *out_type =
            llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*context.TheContext), out_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
            out_type, llvm::Function::InternalLinkage,
            llvm::Twine("out"),
            *context.TheModule);
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(*context.TheContext, "entry", func, 0);
    context.pushBlock(bblock);

    const char *constValue = "%lld\n";
    llvm::Constant *format_const = llvm::ConstantDataArray::getString(*context.TheContext, constValue);
    llvm::GlobalVariable *var =
            new llvm::GlobalVariable(
                    *context.TheModule, llvm::ArrayType::get(llvm::IntegerType::get(*context.TheContext, 8), strlen(constValue) + 1),
                    true, llvm::GlobalValue::PrivateLinkage, format_const, ".str");
    llvm::Constant *zero =
            llvm::Constant::getNullValue(llvm::IntegerType::getInt64Ty(*context.TheContext));

    std::vector<llvm::Constant *> indices;
    indices.push_back(zero);
    indices.push_back(zero);

    llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(
            llvm::ArrayType::get(llvm::IntegerType::get(*context.TheContext, 8), strlen(constValue) + 1),
            var, indices);

    std::vector<Value *> args;
    args.push_back(var_ref);

    Function::arg_iterator argsValues = func->arg_begin();
    Value *toPrint = &*argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

    // Call printf to the stdout
    CallInst::Create(printfFn, ArrayRef(args), "", bblock);

    ReturnInst::Create(*context.TheContext, bblock);
    context.popBlock();
    context.globals[func->getName().str()] =
            fuseData::globalInfo{func, func->getReturnType(), func->getFunctionType()};

//    func->print(llvm::errs());
}

void createCoreFunctions(CodeGenContext &context) {
    llvm::Function *printfFn = createCPrint(context);
    createEchoFunction(context, printfFn);
}

#endif
